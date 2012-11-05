#include "rtc.h"
#include "interrupt.h"
#include "debug.h"

#include <minix/bitmap.h>
#include <minix/drivers.h>

#define DEBUG 1

int test_conf(void) {
    int res = 0;
    unsigned long regA, regB, regC, regD;
    
    sys_outb(RTC_ADDR_REG, RTC_REG_B);
    res = sys_inb(RTC_DATA_REG, &regB);
    if (res != 0) {
        printf("test_conf: sys_inb failed.\n");
        return res;
    }
    
    /*printf("DEBUG: RTC_REG_B : 0x%02X\n", regB);*/
    
    printf("SET:%d PIE:%d AIE:%d UIE:%d SQWE:%d DM:%d MODE:%s DSE:%d\n",
        !!(bit_isset(regB, RTC_SET_BIT)),
        !!(bit_isset(regB, RTC_PIE_BIT)),
        !!(bit_isset(regB, RTC_AIE_BIT)),
        !!(bit_isset(regB, RTC_UIE_BIT)),
        !!(bit_isset(regB, RTC_SQWE_BIT)),
        !!(bit_isset(regB, RTC_DM_BIT)),
        !!(bit_isset(regB, RTC_MODE_BIT)) ? "24" : "12",
        !!(bit_isset(regB, RTC_DSE_BIT)));
        
    sys_outb(RTC_ADDR_REG, RTC_REG_A);
    res = sys_inb(RTC_DATA_REG, &regA);
    if (res != 0) {
        printf("test_conf: sys_inb failed.\n");
        return res;
    }
    
    /*printf("DEBUG: RTC_REG_A : 0x%02X\n", regA);*/
    
    printf("UIP:%d\n", !!(bit_isset(regA, RTC_UIP_BIT)));
    
    sys_outb(RTC_ADDR_REG, RTC_REG_C);
    res = sys_inb(RTC_DATA_REG, &regC);
    if (res != 0) {
        printf("test_conf: sys_inb failed.\n");
        return res;
    }
    
    /*printf("DEBUG: RTC_REG_C : 0x%02X\n", regC);*/
    
    printf("UF:%d AF:%d PF:%d IRQF:%d\n", 
        !!(bit_isset(regC, RTC_UF_BIT)),
        !!(bit_isset(regC, RTC_AF_BIT)),
        !!(bit_isset(regC, RTC_PF_BIT)),
        !!(bit_isset(regC, RTC_IRQF_BIT)));
    
    sys_outb(RTC_ADDR_REG, RTC_REG_D);
    res = sys_inb(RTC_DATA_REG, &regD);
    if (res != 0) {
        printf("test_conf: sys_inb failed.\n");
        return res;
    }
    
    /*printf("DEBUG: RTC_REG_D : 0x%02X\n", regD);*/
    
    printf("VRT:%d\n", !!bit_isset(regD, RTC_VRT_BIT));
    
    return res;
}

int test_date(void) {
	int res = 0;
    unsigned long regA = 0;
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

    printf("%s %s %u %u:%u:%u UTC 20%u\n", month_s[bcd_to_decimal(month) - 1], week_day_s[day_of_week - 1], bcd_to_decimal(day_of_month),
        bcd_to_decimal(hours), bcd_to_decimal(minutes), bcd_to_decimal(seconds), bcd_to_decimal(year));
    printf("Alarm: %u:%u:%u\n", bcd_to_decimal(hours_alarm), bcd_to_decimal(minutes_alarm), bcd_to_decimal(seconds_alarm));

    return res;
}

static unsigned long _counter = 0;
static unsigned long _duration;

int Handler() {
    
    _counter = periodicHandler();
    
    if (_counter >= _duration)
        int_stop_handler();
    return _counter;
}

int test_int(unsigned long duration) {     
    unsigned long regA, regB;
    int_init();
    
    rtc_read_register(RTC_REG_C, NULL);
    
        
    
    
    
    int_subscribe(RTC_IRQ, IRQ_REENABLE,Handler);
    
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
    
    rtc_subscribe_periodic(Handler);
	printf("test_int: Starting time interval with duration %u ms\n", duration);
    _duration = duration;
    
    int_start_handler();
    
    printf("test_int: Elapsed time interval of duration %u ms\n", duration);
    
    return 0;
}

