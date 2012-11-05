#include "rtc.h"
#include "interrupt.h"
#include "debug.h"

#include <minix/bitmap.h>
#include <minix/drivers.h>

#define DEBUG 1

int test_conf(void) {
    int res = 0;
    unsigned long regB;
    
    sys_outb(RTC_ADDR_REG, RTC_REG_B);
    res = sys_inb(RTC_DATA_REG, &regB);
    if (res != 0) {
        printf("test_conf: sys_inb failed.\n");
        return res;
    }
    
    printf("DEBUG: RTC_REG_B : 0x%02X\n", regB);
    
    printf("SET:%d PIE:%d AIE:%d UIE:%d SQWE:%d DM:%d MODE:%s DSE:%d\n",
        !!(bit_isset(regB, RTC_SET_BIT)),
        !!(bit_isset(regB, RTC_PIE_BIT)),
        !!(bit_isset(regB, RTC_AIE_BIT)),
        !!(bit_isset(regB, RTC_UIE_BIT)),
        !!(bit_isset(regB, RTC_SQWE_BIT)),
        !!(bit_isset(regB, RTC_DM_BIT)),
        !!(bit_isset(regB, RTC_MODE_BIT)) ? "24" : "12",
        !!(bit_isset(regB, RTC_DSE_BIT)));
    
    return res;
}

int test_date(void) {
	int res = 0;
    unsigned long regA = 0;
    unsigned long seconds, minutes, hours, day_of_month, month, year, day_of_week;
    
    do {
        
    sys_outb(RTC_ADDR_REG, RTC_REG_A);
    res = sys_inb(RTC_DATA_REG, &regA);
    if (res != 0) {
        printf("test_date: sys_inb failed.\n");
        return res;
    }
    
    printf("DEBUG: RTC_REG_A : 0x%X\n", regA);
    
    } while (bit_isset(regA, RTC_UIP_BIT));
    
    sys_outb(RTC_ADDR_REG, RTC_SECONDS);
    res = sys_inb(RTC_DATA_REG, &seconds);
    if (res != 0) {
        printf("test_date: sys_inb failed.\n");
        return res;
    }
    
    printf("DEBUG: RTC_SECONDS : 0x%X\n", seconds);
    
    sys_outb(RTC_ADDR_REG, RTC_MINUTES);
    res = sys_inb(RTC_DATA_REG, &minutes);
    if (res != 0) {
        printf("test_date: sys_inb failed.\n");
        return res;
    }
    
    printf("DEBUG: RTC_MINUTES : 0x%X\n", minutes);
    
    sys_outb(RTC_ADDR_REG, RTC_HOURS);
    res = sys_inb(RTC_DATA_REG, &hours);
    if (res != 0) {
        printf("test_date: sys_inb failed.\n");
        return res;
    }
    
    printf("DEBUG: RTC_HOURS : 0x%X\n", hours);
    
    sys_outb(RTC_ADDR_REG, RTC_DAY_OF_MONTH);
    res = sys_inb(RTC_DATA_REG, &day_of_month);
    if (res != 0) {
        printf("test_date: sys_inb failed.\n");
        return res;
    }
    
    printf("DEBUG: RTC_DAY_OF_MONTH : 0x%X\n", day_of_month);
    
    sys_outb(RTC_ADDR_REG, RTC_MONTH);
    res = sys_inb(RTC_DATA_REG, &month);
    if (res != 0) {
        printf("test_date: sys_inb failed.\n");
        return res;
    }
    
    printf("DEBUG: RTC_MONTH : 0x%X\n", month);
    
    sys_outb(RTC_ADDR_REG, RTC_YEAR);
    res = sys_inb(RTC_DATA_REG, &year);
    if (res != 0) {
        printf("test_date: sys_inb failed.\n");
        return res;
    }
    
    printf("DEBUG: RTC_YEAR : 0x%X\n", year);
    
    sys_outb(RTC_ADDR_REG, RTC_DAY_OF_WEEK);
    res = sys_inb(RTC_DATA_REG, &day_of_week);
    if (res != 0) {
        printf("test_date: sys_inb failed.\n");
        return res;
    }
    
    printf("DEBUG: RTC_DAY_OF_WEEK : 0x%X\n", day_of_week);
    
    
    return res;
}

int test_int(unsigned long duration) { 
	/* To be completed */
}

