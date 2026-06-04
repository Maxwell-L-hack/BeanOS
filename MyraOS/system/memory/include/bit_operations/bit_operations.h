#ifndef BIT_OPERATIONS_H
#define BIT_OPERATIONS_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void set_bit(uint8_t* bitmap, const size_t bit);

void clear_bit(uint8_t* bitmap, const size_t bit);

bool test_bit(uint8_t* bitmap, const size_t bit);

#endif // BIT_OPERATIONS_H