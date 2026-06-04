#include "rtc/rtc.h"

#include <stdbool.h>

#include "io/port_io.h"

#define RTC_SECONDS 0x00
#define RTC_MINUTES 0X02
#define RTC_HOURS 0x04
#define RTC_WEEKDAY 0x06
#define RTC_DAY 0x07
#define RTC_MONTH 0x08
#define RTC_YEAR 0x09
#define RTC_CENTURY 0x32 // Note: Not viable on all BIOS

#define RTC_CMOS_REG_PORT 0x70
#define RTC_CMOS_IO_PORT 0x71

#define RTC_BASE_CENTURY 2000
#define RTC_IS_CENTURY_VALID(century) (century >= 19 && century <= 21)

static uint32_t system_epoch;

static bool is_bcd_format = true;
static bool is_24_hour_format;

static inline uint8_t bcd_to_bin(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static uint8_t rtc_get_formatted_hour(uint8_t hour) {
    if (!is_24_hour_format) {
        bool is_pm = hour & 0x80;
        hour &= 0x7F;

        if (is_pm && hour != 12) {
            hour += 12;
        }

        if (!is_pm && hour == 12) {
            hour = 0;
        }
    }

    return hour;
}

static uint8_t rtc_get_cmos_reg(uint8_t reg) {
    outb(RTC_CMOS_REG_PORT, reg);
    uint8_t value = inb(RTC_CMOS_IO_PORT);

    return is_bcd_format ? bcd_to_bin(value) : value;
}

void rtc_init(void) {
    outb(RTC_CMOS_REG_PORT, 0x0B);

    is_bcd_format = !(inb(RTC_CMOS_IO_PORT) & 0x04);
    is_24_hour_format = inb(RTC_CMOS_IO_PORT) & 0x02;

    rtc_update_system_time();
}

uint32_t rtc_get_system_epoch(void) {
    rtc_update_system_time();
    return system_epoch;
}

void rtc_update_system_time(void) {
    datetime_t dt = rtc_get_system_datetime();

    static const int days_in_month[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    uint32_t days = 0;

    // Add days for full years since 1970
    for (int year = 1970; year < dt.year; year++) {
        days += 365;
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            days += 1; // Leap year
        }
    }

    // Add days for full months in current year
    for (int month = 1; month < dt.month; month++) {
        days += days_in_month[month - 1];
        if (month == 2 && ((dt.year % 4 == 0 && dt.year % 100 != 0) || (dt.year % 400 == 0))) {
            days += 1; // Feb in leap year
        }
    }

    // Add days in current month
    days += dt.day - 1;

    // Convert everything to seconds
    system_epoch = dt.second
                 + dt.minute * 60
                 + dt.hour * 3600
                 + days * 86400;
}

datetime_t rtc_get_system_datetime(void) {
    __asm__ volatile("cli");

    // Wait until UIP bit is unset
    outb(RTC_CMOS_REG_PORT, 0x0A);
    while (inb(RTC_CMOS_IO_PORT) & 0x80);

    uint8_t second = rtc_get_cmos_reg(RTC_SECONDS);
    uint8_t minute = rtc_get_cmos_reg(RTC_MINUTES);

    uint8_t hour = rtc_get_formatted_hour(rtc_get_cmos_reg(RTC_HOURS));
    uint8_t weekday = rtc_get_cmos_reg(RTC_WEEKDAY);
    uint8_t day = rtc_get_cmos_reg(RTC_DAY);
    uint8_t month = rtc_get_cmos_reg(RTC_MONTH);
    uint8_t year = rtc_get_cmos_reg(RTC_YEAR);
    uint8_t century = rtc_get_cmos_reg(RTC_CENTURY);
    uint16_t full_year = RTC_IS_CENTURY_VALID(century) ? (century * 100 + year) : (RTC_BASE_CENTURY + year);

    __asm__ volatile("sti");

    datetime_t datetime = {
        .second = second,
        .minute = minute,
        .hour = hour,
        .weekday = weekday,
        .day = day,
        .month = month,
        .year = full_year,
    };

    return datetime;
}