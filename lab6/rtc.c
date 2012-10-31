#include "rtc.h"
#include "interrupt.h"

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