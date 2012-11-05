#include "rtc.h"
#include "interrupt.h"
#include "debug.h"

#include <minix/drivers.h>

int test_conf(void) {
    int res = 0;
    unsigned long regB;
    
    sys_outb(RTC_ADDR_REG, RTC_REG_B);
    res = sys_inb(RTC_DATA_REG, &regB);
    if (res != 0) {
        printf("test_conf: sys_inb failed.\n");
        return res;
    }
    
    printfd("DEBUG: RTC_REG_B : 0x%02X\n", regB);
    
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
	/* To be completed */
}

int test_int(unsigned long duration) { 
	/* To be completed */
}

