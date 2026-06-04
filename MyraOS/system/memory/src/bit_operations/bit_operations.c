#include "bit_operations/bit_operations.h"

#include "constants/mem_constants.h"

void set_bit(uint8_t* bitmap, const size_t bit) {
    size_t bitmap_index = bit / BYTE_SIZE;
    size_t bit_index = bit % BYTE_SIZE;

    uint8_t mask = 1 << bit_index;

    bitmap[bitmap_index] |= mask;
}

void clear_bit(uint8_t* bitmap, const size_t bit) {
    size_t bitmap_index = bit / BYTE_SIZE;
    size_t bit_index = bit % BYTE_SIZE;

    uint8_t mask = ~(1 << bit_index);

    bitmap[bitmap_index] &= mask;
}

bool test_bit(uint8_t* bitmap, const size_t bit) {
    size_t bitmap_index = bit / BYTE_SIZE;
    size_t bit_index = bit % BYTE_SIZE;

    return (bitmap[bitmap_index] >> bit_index) & 1;
}
