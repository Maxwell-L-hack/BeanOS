#ifndef EXT2_H
#define EXT2_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "block_device/block_device.h"

#define EXT2_DIRECT_BLOCKS 12
#define EXT2_NAME_LEN 255

#define EXT2_SUPERBLOCK_SIZE 1024
#define EXT2_SUPERBLOCK_OFFSET 1024
#define EXT2_SUPERBLOCK_SIGNATURE 0xEF53

typedef struct superblock_t {
    uint32_t total_inodes;
    uint32_t total_blocks;
    uint32_t superuser_blocks;
    uint32_t free_blocks;
    uint32_t free_inodes;
    uint32_t superblock_index;
    uint32_t log_2_block_size;
    uint32_t log_2_fragment_size;
    uint32_t blocks_per_group;
    uint32_t fragments_per_group;
    uint32_t inodes_per_group;
    uint32_t last_mount_time;
    uint32_t last_written_time;

    uint16_t mounts_since_last_check;
    uint16_t mounts_allowed_before_check;
    uint16_t signature;
    uint16_t fs_state;
    uint16_t error_detected_action;
    uint16_t minor_version;
    
    uint32_t last_check;
    uint32_t interval;
    uint32_t os_id;
    uint32_t rev_level;
    
    uint16_t uid;
    uint16_t gid;

    // Extended features
    uint32_t first_inode;
    
    uint16_t inode_size;
    uint16_t superblock_group;
    
    uint32_t optional_feature;
    uint32_t required_feature;
    uint32_t readonly_feature;

    char fs_id[16];
    char vol_name[16];
    char last_mount_path[64];

    uint32_t compression_method;

    uint8_t file_pre_alloc_blocks;
    uint8_t dir_pre_alloc_blocks;
    
    uint16_t unused;

    char journal_id[16];

    uint32_t journal_inode;
    uint32_t journal_device;
    uint32_t orphan_head;
} __attribute__((packed)) superblock_t;

typedef enum inode_mode_t {
    EXT2_S_IFSOCK = 0xC000,  // socket
    EXT2_S_IFLNK = 0xA000,   // symbolic link
    EXT2_S_IFREG = 0x8000,   // regular file
    EXT2_S_IFBLK = 0x6000,   // block device
    EXT2_S_IFDIR = 0x4000,   // directory
    EXT2_S_IFCHR = 0x2000,   // char device
    EXT2_S_IFIFO = 0x1000,   // FIFO
} inode_mode_t;

typedef struct inode_t {
    uint16_t mode;
    uint16_t uid;

    uint32_t size;
    uint32_t atime;
    uint32_t ctime;
    uint32_t mtime;
    uint32_t dtime;

    uint16_t gid;
    uint16_t hard_links;
    
    uint32_t disk_sectors;
    uint32_t flags;
    uint32_t os_specific_value_1;
    uint32_t direct_blocks[EXT2_DIRECT_BLOCKS];
    uint32_t single_indirect_block;
    uint32_t double_indirect_block;
    uint32_t triple_indirect_block;
    uint32_t generation_number;
    uint32_t file_acl;
    union {
        uint32_t dir_acl;
        uint32_t file_size;
    };
    uint32_t block_fragment_addr;

    char os_specific_value_2[12];
} __attribute__((packed)) inode_t;

typedef struct dir_entry_t {
    uint32_t inode;
    uint16_t rec_len;
    uint8_t name_len;
    uint8_t type;
    char name[];
} __attribute__((packed)) dir_entry_t;

typedef struct block_group_desc_t {
    uint32_t block_bitmap;
    uint32_t inode_bitmap;
    uint32_t inode_table;

    uint16_t free_blocks_count;
    uint16_t free_inodes_count;
    uint16_t used_dirs_count;
    uint16_t pad;

    uint8_t reserved[12];
} __attribute__((packed)) block_group_desc_t;

typedef struct ext2_fs_t {
    superblock_t* superblock;
    block_group_desc_t* group_desc; 

    uint32_t block_size;
    uint32_t total_groups;
    uint32_t inodes_per_group;
    uint32_t blocks_per_group;
    uint32_t first_data_block;
    uint32_t* inode_table_start;

    block_device_t* device;
} ext2_fs_t;

// TODO: add VFS
extern ext2_fs_t* root_fs;

// Mount
bool ext2_mount(ext2_fs_t* fs, block_device_t* device);

// Files
uint8_t* ext2_read_file(ext2_fs_t* fs, const char* path, size_t* out_size, bool* succeeded);

size_t ext2_get_file_size(ext2_fs_t* fs, char* path);

bool ext2_file_exists(ext2_fs_t* fs, char* path);

bool ext2_is_file(ext2_fs_t* fs, char* path);

// Folders
size_t ext2_list_dir(ext2_fs_t* fs, char* path, dir_entry_t*** out_entries);

bool ext2_is_dir(ext2_fs_t* fs, char* path);

#endif // EXT2_H
