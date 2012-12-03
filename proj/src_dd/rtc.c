#include "rtc.h"
#include "interrupt.h"
#include "priority_list.h"

#include <stdlib.h>
#include <stdio.h>
#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/bitmap.h>

static int _bit;

static int(*_periodicHandler)(void) = NULL;
static int(*_alarmHandler)(void) = NULL;
static int(*_updateHandler)(void) = NULL;
static priority_list_t* _rtc_events;

static unsigned int ticks;

/* Functions */
static int _rtc_subscribe(void);

void rtc_init(void) {
    printf("DEBUG: %s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
    _rtc_events = priority_list_new(NULL, event_copy, event_destroy);
    printf("DEBUG: %s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
    _rtc_subscribe();
    printf("DEBUG: %s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
}

void rtc_terminate(void) {
    int_unsubscribe(_bit);

    priority_list_remove_all(_rtc_events);
    free(_rtc_events);
    _rtc_events = NULL;
}

static void _rtcIntHandler(void) {
    int cause;
    unsigned long regC;
    event_t* event;
    priority_list_node_t* elem;

    sys_outb(RTC_ADDR_REG, RTC_REG_C);
    cause = sys_inb(RTC_DATA_REG, &regC);

    printf("DEBUG: %s, %s\n", __FILE__, __FUNCTION__);

    if(bit_isset(regC, RTC_PF_BIT) && _periodicHandler) {
        ticks++;
        printf("DEBUG: %s, %s, ticks: %d", __FILE__, __FUNCTION__, ticks);
        elem = _rtc_events->root;
        while (elem != NULL) {
            event = (event_t*)(elem->val);
            if (event->due_ticks == ticks) {
                if (!event->callback(event)) {
                    rtc_terminate();
                    return;
                }
                if(!(event->recursive)) {
                    elem = priority_list_remove(_rtc_events, elem);
                    continue;
                } else {
                    event_reset(event, ticks);
                }
                elem = elem->next;
            }
        }
    }
}

static int _rtc_subscribe(void) {
    unsigned long regA, regB;
    printf("DEBUG: %s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
    _bit = int_subscribe(RTC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, _rtcIntHandler);
    printf("DEBUG: %s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
    rtc_read_register(RTC_REG_B, &regB);
    printf("DEBUG: %s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
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

int rtc_add_event(unsigned int dur_ms, int (*callback)(event_t*), unsigned int priority, unsigned int recursive) {
    event_t event = { dur_ms + ticks, dur_ms, recursive, callback };

    if (!priority_list_add_elem(_rtc_events, (void*)(&event), priority))
        return 1;

    return 0;
}
