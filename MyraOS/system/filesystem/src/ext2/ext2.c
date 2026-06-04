#include "ext2/ext2.h"

#include "libc_kernel/string.h"
#include "heap/heap.h"
#include "block_device/block_device.h"

#include <stdbool.h>

#define EXT2_SUPERBLOCK_LBA 2
#define EXT2_SUPERBLOCK_SECTORS 2
#define EXT2_MAX_BLOCK_SIZE 4096

#define EXT2_ROOT_INODE 2
#define EXT2_PATH_SEPERATOR "/"

#define IS_REG(mode) (((mode) & 0xF000) == EXT2_S_IFREG)
#define IS_DIR(mode) (((mode) & 0xF000) == EXT2_S_IFDIR)

ext2_fs_t* root_fs;

static uint32_t block_to_lba(ext2_fs_t* fs, uint32_t block_num);
static void read_block_group_desc(ext2_fs_t* fs);
static bool read_inode(ext2_fs_t* fs, size_t inode_index, inode_t* out_inode);
static size_t read_dir_entry_list(ext2_fs_t* fs, inode_t* inode, dir_entry_t*** dir_entries);
static void read_dir_entry(ext2_fs_t* fs, inode_t* inode, uint16_t start_offset, dir_entry_t** dir_entry);
static bool resolve_path(ext2_fs_t* fs, const char* path, inode_t* inode);

bool ext2_mount(ext2_fs_t* fs, block_device_t* device) {
    fs->superblock = kmalloc(sizeof(superblock_t));
    fs->device = device;

    // Read the superblock sectors
    uint8_t superblock[EXT2_SUPERBLOCK_SIZE];

    device->read_sectors(EXT2_SUPERBLOCK_LBA, EXT2_SUPERBLOCK_SECTORS, superblock, device->driver_data);

    kmemcpy(fs->superblock, superblock, sizeof(superblock_t));

    // Verify ext2 signature
    if (fs->superblock->signature != EXT2_SUPERBLOCK_SIGNATURE) {
        return false;
    }

    fs->block_size = (1024 << fs->superblock->log_2_block_size);
    read_block_group_desc(fs);

    inode_t root;
    read_inode(fs, EXT2_ROOT_INODE, &root);

    return true;
}

uint8_t* ext2_read_file(ext2_fs_t* fs, const char* path, size_t* out_size, bool* succeeded) {
    *succeeded = false;

    inode_t file_inode;
    if (!resolve_path(fs, path, &file_inode)) {
        return NULL;
    }

    if (!IS_REG(file_inode.mode)) {
        return NULL;
    }

    *out_size = file_inode.size;
    if (*out_size == 0) {
        *succeeded = true;
        return NULL;
    }

    size_t total_bytes = *out_size;
    size_t total_blocks = (total_bytes + fs->block_size - 1) / fs->block_size;

    uint8_t* full_buffer = (uint8_t*) kmalloc(total_blocks * fs->block_size);
    if (!full_buffer) {
        return NULL;
    }

    size_t read_blocks = 0;

    // Direct blocks
    for (; read_blocks < total_blocks && read_blocks < EXT2_DIRECT_BLOCKS; read_blocks++) {
        fs->device->read_sectors(
            block_to_lba(fs, file_inode.direct_blocks[read_blocks]),
            fs->block_size / BLOCK_SECTOR_SIZE,
            full_buffer + read_blocks * fs->block_size,
            fs->device->driver_data
        );
    }

    // Single indirect
    if (read_blocks < total_blocks) {
        uint32_t* indirect_block = (uint32_t*) kmalloc(fs->block_size);
        fs->device->read_sectors(
            block_to_lba(fs, file_inode.single_indirect_block),
            fs->block_size / BLOCK_SECTOR_SIZE,
            indirect_block,
            fs->device->driver_data
        );

        size_t indirect_count = fs->block_size / sizeof(uint32_t);
        for (size_t i = 0; read_blocks < total_blocks && i < indirect_count; i++, read_blocks++) {
            fs->device->read_sectors(
                block_to_lba(fs, indirect_block[i]),
                fs->block_size / BLOCK_SECTOR_SIZE,
                full_buffer + read_blocks * fs->block_size,
                fs->device->driver_data
            );
        }

        kfree(indirect_block);
    }

    // Double indirect
    if (read_blocks < total_blocks) {
        uint32_t* dbl_indirect = (uint32_t*) kmalloc(fs->block_size);
        fs->device->read_sectors(
            block_to_lba(fs, file_inode.double_indirect_block),
            fs->block_size / BLOCK_SECTOR_SIZE,
            dbl_indirect,
            fs->device->driver_data
        );

        size_t indirect_count = fs->block_size / sizeof(uint32_t);
        for (size_t i = 0; read_blocks < total_blocks && i < indirect_count; i++) {
            uint32_t* indirect = (uint32_t*) kmalloc(fs->block_size);
            fs->device->read_sectors(
                block_to_lba(fs, dbl_indirect[i]),
                fs->block_size / BLOCK_SECTOR_SIZE,
                indirect,
                fs->device->driver_data
            );
            for (size_t j = 0; read_blocks < total_blocks && j < indirect_count; j++, read_blocks++) {
                fs->device->read_sectors(
                    block_to_lba(fs, indirect[j]),
                    fs->block_size / BLOCK_SECTOR_SIZE,
                    full_buffer + read_blocks * fs->block_size,
                    fs->device->driver_data
                );
            }
            kfree(indirect);
        }

        kfree(dbl_indirect);
    }

    // Triple indirect
    if (read_blocks < total_blocks) {
        uint32_t* tpl_indirect = (uint32_t*) kmalloc(fs->block_size);
        fs->device->read_sectors(
            block_to_lba(fs, file_inode.triple_indirect_block),
            fs->block_size / BLOCK_SECTOR_SIZE,
            tpl_indirect,
            fs->device->driver_data
        );

        size_t indirect_count = fs->block_size / sizeof(uint32_t);
        for (size_t i = 0; read_blocks < total_blocks && i < indirect_count; i++) {
            uint32_t* dbl = (uint32_t*) kmalloc(fs->block_size);
            fs->device->read_sectors(
                block_to_lba(fs, tpl_indirect[i]),
                fs->block_size / BLOCK_SECTOR_SIZE,
                dbl,
                fs->device->driver_data
            );

            for (size_t j = 0; read_blocks < total_blocks && j < indirect_count; j++) {
                uint32_t* indirect = (uint32_t*) kmalloc(fs->block_size);
                fs->device->read_sectors(
                    block_to_lba(fs, dbl[j]),
                    fs->block_size / BLOCK_SECTOR_SIZE,
                    indirect,
                    fs->device->driver_data
                );

                for (size_t k = 0; read_blocks < total_blocks && k < indirect_count; k++, read_blocks++) {
                    fs->device->read_sectors(
                        block_to_lba(fs, indirect[k]),
                        fs->block_size / BLOCK_SECTOR_SIZE,
                        full_buffer + read_blocks * fs->block_size,
                        fs->device->driver_data
                    );
                }

                kfree(indirect);
            }
            kfree(dbl);
        }

        kfree(tpl_indirect);
    }

    // Allocate actual buffer and copy only file size
    uint8_t* buffer = (uint8_t*) kmalloc(*out_size);
    if (!buffer) {
        kfree(full_buffer);
        return NULL;
    }

    kmemcpy(buffer, full_buffer, *out_size);
    kfree(full_buffer);

    *succeeded = true;
    return buffer;
}

size_t ext2_get_file_size(ext2_fs_t* fs, char* path) {
    inode_t file_inode;
    
    if (!resolve_path(fs, path, &file_inode)) {
        return 0;
    }

    if (!IS_REG(file_inode.mode)) {
        return 0;
    }
    
    return file_inode.size;
}

bool ext2_file_exists(ext2_fs_t* fs, char* path) {
    return ext2_is_file(fs, path);
}

bool ext2_is_file(ext2_fs_t* fs, char* path) {
    inode_t file_inode;

    if (!resolve_path(fs, path, &file_inode)) {
        return false;
    }

    return IS_REG(file_inode.mode);
}

size_t ext2_list_dir(ext2_fs_t* fs, char* path, dir_entry_t*** out_entries) {
    inode_t dir_inode;

    if (!resolve_path(fs, path, &dir_inode)) {
        return 0;
    }

    if (!IS_DIR(dir_inode.mode)) {
        return 0;
    }
    
    return read_dir_entry_list(fs, &dir_inode, out_entries);
}

bool ext2_is_dir(ext2_fs_t* fs, char* path) { 
    inode_t dir_inode;

    if (!resolve_path(fs, path, &dir_inode)) {
        return false;
    }

    return IS_DIR(dir_inode.mode);
}

static uint32_t block_to_lba(ext2_fs_t* fs, uint32_t block_num) {
    uint32_t sectors_per_block = fs->block_size / BLOCK_SECTOR_SIZE;
    return block_num * sectors_per_block;
}

static void read_block_group_desc(ext2_fs_t* fs) {
    fs->total_groups = (fs->superblock->total_blocks + fs->superblock->blocks_per_group - 1) / fs->superblock->blocks_per_group;
    if (fs->total_groups == 0) {
        fs->total_groups = 1;
    }

    uint32_t desc_size = fs->total_groups * sizeof(block_group_desc_t);
    uint32_t total_sectors = (desc_size + BLOCK_SECTOR_SIZE - 1) / BLOCK_SECTOR_SIZE;

    fs->group_desc = kmalloc(total_sectors * BLOCK_SECTOR_SIZE);

    uint32_t lba;
    if (fs->block_size == 1024) {
        lba = EXT2_SUPERBLOCK_LBA + EXT2_SUPERBLOCK_SECTORS;
    } else {
        lba = block_to_lba(fs, 1);
    }

    uint8_t* desc_buff = kmalloc(total_sectors * BLOCK_SECTOR_SIZE);
    fs->device->read_sectors(lba, total_sectors, desc_buff, fs->device->driver_data);
    fs->group_desc = (block_group_desc_t*) desc_buff;
}

static bool read_inode(ext2_fs_t* fs, size_t inode_index, inode_t* out_inode) {
    if (inode_index == 0 || inode_index > fs->superblock->total_inodes) {
        return false;
    }

    uint32_t inodes_per_group = fs->superblock->inodes_per_group;
    uint32_t block_size = fs->block_size;

    uint32_t group_index = (inode_index - 1) / inodes_per_group;
    uint32_t index_in_group = (inode_index - 1) % inodes_per_group;

    if (group_index >= fs->total_groups || !fs->group_desc) {
        return false;
    }

    uint32_t inode_size = (fs->superblock->rev_level == 0) ? 128 : fs->superblock->inode_size;
    if (inode_size < sizeof(inode_t)) {
        return false;
    }

    block_group_desc_t* group_desc = &fs->group_desc[group_index];
    uint32_t inode_table_block = group_desc->inode_table;

    uint32_t inode_offset = index_in_group * inode_size;
    uint32_t block_offset = inode_table_block + (inode_offset / block_size);
    uint32_t offset_in_block = inode_offset % block_size;

    uint32_t lba = block_to_lba(fs, block_offset);
    uint8_t* block = kmalloc(block_size);
    
    fs->device->read_sectors(lba, block_size / BLOCK_SECTOR_SIZE, block, fs->device->driver_data);

    if (offset_in_block + sizeof(inode_t) > block_size) {
        kfree(block);
        return false;
    }

    kmemcpy(out_inode, block + offset_in_block, sizeof(inode_t));
    kfree(block);

    return true;
}

static size_t read_dir_entry_list(ext2_fs_t* fs, inode_t* inode, dir_entry_t*** dir_entries) {
    if (!IS_DIR(inode->mode)) {
        return 0;
    }

    // The inode size is the size of all the dir entries
    *dir_entries = (dir_entry_t**) kmalloc(sizeof(dir_entry_t*) * (inode->size / 8));

    uint32_t inode_offset = 0;
    size_t dir_count = 0;
    while (inode_offset <= inode->size) {
        dir_entry_t* dir_entry;
        read_dir_entry(fs, inode, inode_offset, &dir_entry);
        
        if (!dir_entry || dir_entry->rec_len < 8 || inode_offset + dir_entry->rec_len > inode->size) {
            return dir_count;
        }

        (*dir_entries)[dir_count++] = dir_entry;
        inode_offset += dir_entry->rec_len;
    }

    return dir_count;
}

static void read_dir_entry(ext2_fs_t* fs, inode_t* inode, uint16_t offset, dir_entry_t** dir_entry) {
    uint32_t block_index = offset / fs->block_size;
    uint32_t offset_in_block = offset % fs->block_size;

    if (block_index >= EXT2_DIRECT_BLOCKS || inode->direct_blocks[block_index] == 0) {
        *dir_entry = NULL;
        return;
    }

    uint32_t lba = block_to_lba(fs, inode->direct_blocks[block_index]);
    uint8_t* block = kmalloc(fs->block_size);

    fs->device->read_sectors(lba, fs->block_size / BLOCK_SECTOR_SIZE, block, fs->device->driver_data);
    
    dir_entry_t* entry_in_block = (dir_entry_t*) (block + offset_in_block);

    *dir_entry = kmalloc(entry_in_block->rec_len);
    kmemcpy(*dir_entry, entry_in_block, entry_in_block->rec_len);
    kfree(block);
}

static bool resolve_path(ext2_fs_t *fs, const char *path, inode_t *out_inode) {
    if (kstrcmp(path, "/") == 0) {
        return read_inode(fs, EXT2_ROOT_INODE, out_inode);
    }

    // Path isn't absolute
    if (path[0] != '/') {
        return false;
    }

    size_t path_len = kstrlen(path);
    char* path_copy = kmalloc(path_len + 1);
    kmemcpy(path_copy, path, path_len);
    path_copy[path_len] = '\0';

    inode_t current_inode;
    if (!read_inode(fs, EXT2_ROOT_INODE, &current_inode)) {
        kfree(path_copy);
        return false;
    }
    
    char* token  = kstrtok(path_copy, EXT2_PATH_SEPERATOR);
    if (token == NULL) {
        kmemcpy(out_inode, &current_inode, sizeof(inode_t));
        kfree(path_copy);
        return true;
    }
    
    do {
        bool found = false;
        dir_entry_t** entries = NULL;
        size_t count = read_dir_entry_list(fs, &current_inode, &entries);
        
        for (size_t i = 0; i < count; i++) {
            if (entries[i]->inode == 0) {
                continue;
            }

            char* name = kmalloc(entries[i]->name_len + 1);
            kmemcpy(name, entries[i]->name, entries[i]->name_len);
            name[entries[i]->name_len] = '\0';

            if (kstrcmp(name, token) == 0) {
                if (read_inode(fs, entries[i]->inode, &current_inode)) {
                    found = true;
                }

                kfree(name);
                break;
            }

            kfree(name);
        }

        if (entries != NULL) {
            for (size_t i = 0; i < count; i++) {
                if (entries[i] != NULL) {
                    kfree(entries[i]); // each dir_entry
                }
            }
            kfree(entries); // the array itself
        }

        if (!found) {
            kfree(path_copy);

            return false;
        }
    } while ((token = kstrtok(NULL, EXT2_PATH_SEPERATOR)) != NULL);

    kmemcpy(out_inode, &current_inode, sizeof(inode_t));
    kfree(path_copy);

    return true;
}
