#include "rtc.h"
#include "interrupt.h"

#include <stdlib.h>
#include <stdio.h>
#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/bitmap.h>

static void(*_periodicHandler)(void);
static void(*_alarmHandler)(void);
static void(*_updateHandler)(void);

static void _rtcIntHandler(void) {
    int cause;
    unsigned long regC;
    
    sys_outb(RTC_ADDR_REG, RTC_REG_C);
    cause = sys_inb(RTC_DATA_REG, &regC);
    
    if(bit_isset(regC, RTC_UF_BIT) && _updateHandler)
        _updateHandler();
        
    if(bit_isset(regC, RTC_AF_BIT) && _alarmHandler)
        _alarmHandler();
        
    if(bit_isset(regC, RTC_PF_BIT) && _periodicHandler)
        _periodicHandler();
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

    res = sys_outb(RTC_ADDR_REG, reg);
    if (res != 0)
    {
        printf("rtc_read_register: sys_outb failed.\n");
        return res;
    }

    res = sys_inb(RTC_DATA_REG, value);
    if (res != 0) {
        printf("rtc_read_register: sys_inb failed.\n");
        return res;
    }

    return 0;
}
