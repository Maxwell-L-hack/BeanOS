#include "libc_kernel/stdlib.h"

#include <stdbool.h>

int katoi(const char* s) {
    const int INT_MAX = 2147483647, INT_MIN = -2147483647;
    int sign = 1, res = 0, idx = 0;

    while (s[idx] == ' ') {
        idx++;
    }

    if (s[idx] == '-' || s[idx] == '+') {
      	if(s[idx++] == '-')
          sign = -1;
    }

    while (s[idx] >= '0' && s[idx] <= '9') {
        if (res > INT_MAX / 10 || (res == INT_MAX / 10 && s[idx] - '0' > 7)) {
            return sign == 1 ? INT_MAX : INT_MIN;
        }
      
        res = 10 * res + (s[idx++] - '0');
    }

    return res * sign;
}

char* kitoa(int value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    int tmp_val;

    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        
        return str;
    }

    bool is_negative = false;
    if (value < 0 && base == 10) {
        is_negative = true;
        value = -value;
    }

    while (value != 0) {
        tmp_val = value % base;
        *ptr++ = (tmp_val < 10) ? (tmp_val + '0') : (tmp_val - 10 + 'a');
        value /= base;
    }

    if (is_negative)
        *ptr++ = '-';

    *ptr = '\0';

    // Reverse string
    ptr--;
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }

    return str;
}

char* kutoa(uint32_t value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp;

    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return str;
    }

    while (value != 0) {
        uint32_t digit = value % base;
        *ptr++ = (digit < 10) ? (digit + '0') : (digit - 10 + 'a');
        value /= base;
    }

    *ptr = '\0';

    // Reverse the string
    ptr--;
    while (ptr1 < ptr) {
        tmp = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp;
    }

    return str;
}
