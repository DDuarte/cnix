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

static void _rtcIntHandler(void) {
    int cause;
    unsigned long regC;
    
    //sys_outb(RTC_ADDR_REG, RTC_REG_C);
    //cause = sys_inb(RTC_DATA_REG, &regC);
    
    if(bit_isset(regC, RTC_UF_BIT) && _updateHandler)
        _updateHandler();
        
    if(bit_isset(regC, RTC_AF_BIT) && _alarmHandler)
        _alarmHandler();
        
    if(bit_isset(regC, RTC_PF_BIT) && _periodicHandler)
        _periodicHandler();
}

int rtc_subscribe_periodic(void(*callback)()) {
    unsigned long regA, regB;
    
    
    
    _bit = int_subscribe(RTC_IRQ, IRQ_REENABLE,callback);
    
    rtc_read_register(RTC_REG_B, &regB);
    
    bit_set(regB, RTC_PIE_BIT);
    bit_unset(regB, RTC_AIE_BIT);
    bit_unset(regB, RTC_UIE_BIT);
    
    rtc_write_register(RTC_REG_B, regB);
    
    rtc_read_register(RTC_REG_A, &regA);
    
    bit_unset(regA, RTC_RS0_BIT);
    bit_set(regA, RTC_RS1_BIT);
    bit_set(regA, RTC_RS2_BIT);
    bit_unset(regA, RTC_RS3_BIT);
    
    rtc_write_register(RTC_REG_A, regA);
    
    return _bit;       
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