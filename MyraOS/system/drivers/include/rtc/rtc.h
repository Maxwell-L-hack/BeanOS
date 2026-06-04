#ifndef RTC_H
#define RTC_H

#include <stdint.h>

typedef struct datetime_t {
    uint16_t year;
    uint8_t month, day;
    uint8_t hour, minute, second;
    uint8_t weekday; // 0–6 (Sunday–Saturday)
} datetime_t;

// init
void rtc_init(void);

// epoch
uint32_t rtc_get_system_epoch(void);

void rtc_update_system_time(void);

// datetime
datetime_t rtc_get_system_datetime(void);

#endif // RTC_H