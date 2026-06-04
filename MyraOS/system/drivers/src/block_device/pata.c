#include "block_device/pata.h"

#include <stddef.h>

#include "io/port_io.h"

#define ATA_PRIMARY_IO       0x1F0
#define ATA_PRIMARY_CTRL     0x3F6

#define ATA_REG_DATA(device)        ((device)->io_base + 0)
#define ATA_REG_SECCOUNT(device)    ((device)->io_base + 2)
#define ATA_REG_LBA_LOW(device)     ((device)->io_base + 3)
#define ATA_REG_LBA_MID(device)     ((device)->io_base + 4)
#define ATA_REG_LBA_HIGH(device)    ((device)->io_base + 5)
#define ATA_REG_DRIVE_HEAD(device)  ((device)->io_base + 6)
#define ATA_REG_STATUS(device)      ((device)->io_base + 7)
#define ATA_REG_COMMAND(device)     (ATA_REG_STATUS(device))

#define ATA_REG_ALT_STATUS   (ATA_PRIMARY_CTRL)
#define ATA_REG_DEVICE_CTRL  (ATA_PRIMARY_CTRL)

#define ATA_CMD_IDENTIFY     0xEC

#define ATA_CMD_READ_SECTORS   0x20
#define ATA_CMD_WRITE_SECTORS  0x30
#define ATA_CMD_FLUSH_CACHE    0xE7

static int pata_read_multiple_sectors(uint32_t lba, uint32_t count, void* buffer, void* driver_data) {
    for (size_t i = 0; i < count; i++) {
        int finish_code = pata_read_sector(lba + i, (uint8_t*)buffer + BLOCK_SECTOR_SIZE * i, (pata_device_t*)driver_data);

        if (finish_code < 0) {
            return finish_code;
        }
    }

    return 0;
}

static int pata_write_multiple_sectors(uint32_t lba, uint32_t count, const void* buffer, void* driver_data) {
    for (size_t i = 0; i < count; i++) {
        int finish_code = pata_write_sector(lba + i, (uint8_t*)buffer + BLOCK_SECTOR_SIZE * i, (pata_device_t*)driver_data);

        if (finish_code < 0) {
            return finish_code;
        }
    }

    return 0;
}

static uint32_t pata_get_sector_count_wrapper(void* driver_data) {
    return pata_get_sector_count((pata_device_t*)driver_data);
}

static pata_device_t hd0 = {
    .io_base = 0x1F0,
    .ctrl_base = 0x3F6,
    .is_slave = false,
};

static block_device_t pata_block_device = {
    .name = "hd0",
    .read_sectors = pata_read_multiple_sectors,
    .write_sectors = pata_write_multiple_sectors,
    .get_block_count = pata_get_sector_count_wrapper,
    .driver_data = &hd0
};

static void ata_wait_busy(pata_device_t* device) {
    while (inb(ATA_REG_STATUS(device)) & 0x80); // Wait for BSY to clear
}

static void ata_wait_drq(pata_device_t* device) {
    while (!(inb(ATA_REG_STATUS(device)) & 0x08)); // Wait for DRQ set
}

static void ata_io_delay(void) {
    inb(ATA_REG_ALT_STATUS);
    inb(ATA_REG_ALT_STATUS);
    inb(ATA_REG_ALT_STATUS);
    inb(ATA_REG_ALT_STATUS);
}

static inline bool ata_check_error(pata_device_t* device) {
    return inb(ATA_REG_STATUS(device)) & 0x01;
}

static uint32_t cached_sector_count = 0;

void pata_init(void) {
    block_register_device(&pata_block_device);
}

int pata_read_sector(uint32_t lba, uint8_t* buffer, pata_device_t* device) {
    if (lba >= pata_get_sector_count(device)) {
        return -1;
    }

    ata_wait_busy(device);

    outb(ATA_REG_DRIVE_HEAD(device), 0xE0 | (device->is_slave ? 0x10 : 0x00) | ((lba >> 24) & 0x0F));
    outb(ATA_REG_SECCOUNT(device), 1);

    outb(ATA_REG_LBA_LOW(device), lba & 0xFF);
    outb(ATA_REG_LBA_MID(device), (lba >> 8) & 0xFF);
    outb(ATA_REG_LBA_HIGH(device), (lba >> 16) & 0xFF);

    outb(ATA_REG_COMMAND(device), ATA_CMD_READ_SECTORS);
    if (ata_check_error(device)) {
        return -1;
    }

    ata_io_delay();
    ata_wait_drq(device);

    for (size_t i = 0; i < BLOCK_SECTOR_SIZE / 2; i++) {
        uint16_t data = inw(ATA_REG_DATA(device));

        buffer[i * 2] = data & 0xFF;
        buffer[i * 2 + 1] = (data >> 8) & 0xFF;
    }

    return 0;
}

int pata_write_sector(uint32_t lba, const uint8_t* buffer, pata_device_t* device) {
    if (lba >= pata_get_sector_count(device)) {
        return -1;
    }

    ata_wait_busy(device);

    outb(ATA_REG_DRIVE_HEAD(device), 0xE0 | (device->is_slave ? 0x10 : 0x00) | ((lba >> 24) & 0x0F));
    outb(ATA_REG_SECCOUNT(device), 1);

    outb(ATA_REG_LBA_LOW(device), lba & 0xFF);
    outb(ATA_REG_LBA_MID(device), (lba >> 8) & 0xFF);
    outb(ATA_REG_LBA_HIGH(device), (lba >> 16) & 0xFF);

    outb(ATA_REG_COMMAND(device), ATA_CMD_WRITE_SECTORS);
    if (ata_check_error(device)) {
        return -1;
    }

    ata_io_delay();
    ata_wait_drq(device);

    for (int i = 0; i < BLOCK_SECTOR_SIZE / 2; i++) {
        uint16_t data = buffer[i * 2] | (buffer[i * 2 + 1] << 8);
        outw(ATA_REG_DATA(device), data);
    }

    outb(ATA_REG_COMMAND(device), ATA_CMD_FLUSH_CACHE);

    return 0;
}

uint32_t pata_get_sector_count(pata_device_t* device) {
    if (cached_sector_count) {
        return cached_sector_count;
    }

    ata_wait_busy(device);

    outb(ATA_REG_DRIVE_HEAD(device), device->is_slave ? 0xB0 : 0xA0);
    outb(ATA_REG_COMMAND(device), ATA_CMD_IDENTIFY);

    if (inb(ATA_REG_STATUS(device)) == 0) {
        return 0; // No drive
    }

    if (inb(ATA_REG_LBA_MID(device)) != 0 || inb(ATA_REG_LBA_HIGH(device)) != 0) {
        return 0; // Not ATA device
    }

    if (ata_check_error(device)) {
        return 0;
    }

    ata_wait_drq(device);

    uint16_t buffer[BLOCK_SECTOR_SIZE / 2];
    for (int i = 0; i < BLOCK_SECTOR_SIZE / 2; i++) {
        buffer[i] = inw(ATA_REG_DATA(device));
    }

    // Words 60-61 contain total LBA28 sectors
    cached_sector_count = ((uint32_t)buffer[61] << 16) | buffer[60];
    return cached_sector_count;
}