#ifndef PATA_H
#define PATA_H

#include <stdint.h>
#include <stdbool.h>

#include "block_device/block_device.h"

typedef struct pata_device_t {
    uint16_t io_base;       // Base I/O port (e.g., 0x1F0)
    uint16_t ctrl_base;     // Control port (e.g., 0x3F6)
    bool is_slave;          // 0 = master, 1 = slave
} pata_device_t;

void pata_init(void);

int pata_read_sector(uint32_t lba, uint8_t* buffer, pata_device_t* driver_data);

int pata_write_sector(uint32_t lba, const uint8_t* buffer, pata_device_t* driver_data);

uint32_t pata_get_sector_count(pata_device_t* driver_data);

#endif // PATA_H