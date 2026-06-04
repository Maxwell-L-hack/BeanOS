#include "block_device/block_device.h"

#include <stddef.h>

#include "assert/assert.h"
#include "libc_kernel/string.h"
#include "log/log.h"

static block_device_t* block_drivers[BLOCK_DRIVERS_MAX_AMOUNT] = { 0 };
static uint8_t block_driver_amount = 0;

void block_register_device(block_device_t* device) {
    if (block_driver_amount == BLOCK_DRIVERS_MAX_AMOUNT) {
        klog_info("Too many block drivers registered!\n");
        return;
    }

    if (!device || !device->name) {
        klog_info("Device and name required!\n");
        return;
    }

    block_drivers[block_driver_amount] = device;
    block_driver_amount++;
}

block_device_t *block_get_device(const char *name) {
    for (size_t i = 0; i < BLOCK_DRIVERS_MAX_AMOUNT; i++) {
        if (block_drivers[i] && kstrcmp(block_drivers[i]->name, name) == 0) {
            return block_drivers[i];
        }
    }

    return NULL;
}

uint8_t block_driver_count(void) {
    return block_driver_amount;
}