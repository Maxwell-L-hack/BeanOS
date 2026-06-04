#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include <stdint.h>

#define BLOCK_SECTOR_SIZE 512
#define BLOCK_DRIVERS_MAX_AMOUNT 4

typedef struct block_device_t {
    const char* name;
    int (*read_sectors)(uint32_t lba, uint32_t count, void* buffer, void* driver_data);
    int (*write_sectors)(uint32_t lba, uint32_t count, const void* buffer, void* driver_data);
    uint32_t (*get_block_count)(void* driver_data);
    void* driver_data;
} block_device_t;

void block_register_device(block_device_t* device);

block_device_t* block_get_device(const char* name);

uint8_t block_driver_count(void);

#endif // BLOCK_DEVICE_H