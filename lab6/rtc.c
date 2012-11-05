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
