#include "rtc.h"
#include "interrupt.h"

#include <stdlib.h>
#include <stdio.h>
#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/bitmap.h>

static int _bit;

static int(*_periodicHandler)(void) = NULL;
static int(*_alarmHandler)(void) = NULL;
static int(*_updateHandler)(void) = NULL;

char* rtc_get_date(void) {
    char* res;
    unsigned long seconds, minutes, hours, day_of_month, month, year, day_of_week,
                  seconds_alarm, minutes_alarm, hours_alarm;

    rtc_wait_valid();

    rtc_read_register(RTC_SECONDS, &seconds);
    rtc_read_register(RTC_MINUTES, &minutes);
    rtc_read_register(RTC_HOURS, &hours);
    rtc_read_register(RTC_DAY_OF_MONTH, &day_of_month);
    rtc_read_register(RTC_MONTH, &month);
    rtc_read_register(RTC_YEAR, &year);
    rtc_read_register(RTC_DAY_OF_WEEK, &day_of_week);
    rtc_read_register(RTC_SECONDS_ALARM, &seconds_alarm);
    rtc_read_register(RTC_MINUTES_ALARM, &minutes_alarm);
    rtc_read_register(RTC_HOURS_ALARM, &hours_alarm);

    res = (char*)malloc(sizeof(char)*50);

    /* format similar to unix date command */
    sprintf(res, "%s %s %02u 20%02u %02u:%02u:%02u\n", month_s[bcd_to_decimal(month) - 1], week_day_s[day_of_week - 1], bcd_to_decimal(day_of_month), bcd_to_decimal(year),
        bcd_to_decimal(hours), bcd_to_decimal(minutes), bcd_to_decimal(seconds));

    return res;
}

unsigned long bcd_to_decimal(unsigned long bcd) {

    unsigned long n;
    char buffer[10];
    char *endptr;

    sprintf(buffer, "%x", bcd);

    n = strtoul(buffer, NULL, 10);

    return n;
}


void rtc_wait_valid() {

    unsigned long regA;
    int res;

    do {
        sys_outb(RTC_ADDR_REG, RTC_REG_A);
        res = sys_inb(RTC_DATA_REG, &regA);

        if (res != 0) {
            printf("rtc_wait_valid: sys_inb failed.\n");
            return;
        }

    } while (bit_isset(regA, RTC_UIP_BIT));
}

int rtc_read_register(unsigned long reg, unsigned long* value) {

    int res;
    unsigned long val_temp;
    res = sys_outb(RTC_ADDR_REG, reg);
    if (res != 0)
    {
        printf("rtc_read_register: sys_outb failed.\n");
        return res;
    }

    res = sys_inb(RTC_DATA_REG, &val_temp);
    if (res != 0) {
        printf("rtc_read_register: sys_inb failed.\n");
        return res;
    }

    if (value != NULL)
        *value = val_temp;

    return 0;
}

int rtc_write_register(unsigned long reg, unsigned long value) {

    int res;

    res = sys_outb(RTC_ADDR_REG, reg);
    if (res != 0)
    {
        printf("rtc_read_register: sys_outb failed.\n");
        return res;
    }

    res = sys_outb(RTC_DATA_REG, value);
    if (res != 0) {
        printf("rtc_read_register: sys_inb failed.\n");
        return res;
    }

    return 0;
}
