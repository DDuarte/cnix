#include "interrupt.h"
#include "kbc.h"

#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/bitmap.h>

#include <stdio.h>

static unsigned char packet[3] = { 0, 0, 0 };
static short counter = 0;
static int num_interrupts = 0;

static short timerDuration = 0;
static long timerCounter = 0;

static void _mouseCallback() {

    unsigned long datatemp;
    if (sys_inb(DATA_REG, &datatemp) != 0) {
        printf("read_kbc: sys_inb (2) failed.\n");
    }
    packet[counter] = datatemp;
    
    if (counter == 0 && !bit_isset(packet[0], 3)) {
        /*printf("DEBUG: bit(3) packet[0] = %d\n", !!bit_isset(packet[counter], 3));*/
        return;
    }
    
    counter = (counter + 1) % 3;
    num_interrupts++;
    
    if (counter == 0) {
        printf("B1=0x%X B2=0x%X B3=0x%X LB=%d MB=%d RB=%d XOV=%d YOV=%d X=%d Y=%d\n", packet[0], packet[1], packet[2], 
        !!bit_isset(packet[0], 0),
        !!bit_isset(packet[0], 2),
        !!bit_isset(packet[0], 1),
        !!bit_isset(packet[0], 6),
        !!bit_isset(packet[0], 7),
        (!bit_isset(packet[0], 4) ? packet[1] : - packet[1]),
        (!bit_isset(packet[0], 5) ? packet[2] : - packet[2]));
    }
}

static void mouseCallback(void) {
    _mouseCallback();
    if (num_interrupts == 300) int_stop_handler();
}

static void mouseAsynchCallback(void) {
    _mouseCallback();
}

static void timer0Callback(void) {
    timerCounter++;
    if (timerCounter / 60 == timerDuration)
        int_stop_handler();
}

int test_packet() {
    int mouseInterrupt;
    unsigned long res;
    
    int_init();
    

    
    write_kbc(CMD_REG, 0xD4) ; /* Write Byte to Mouse */    
    write_kbc(DATA_REG, 0xF4); /* Enable Sending Data Packets */
    
    if (sys_inb(DATA_REG, &res) != 0) {
        printf("read_kbc: sys_inb (2) failed.\n");
    }
    printf("DEBUG: acknowledge: 0x%X\n", res);
   
    
    printf("DEBUG: Subscribing interruption\n");
    mouseInterrupt = int_subscribe(12, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouseCallback);
    printf("DEBUG: mouseInterrupt: %d\n", mouseInterrupt);
    
    printf("DEBUG: STARTING_HANDLER.\n");
    
    res = int_start_handler();
    
    int_unsubscribe(mouseInterrupt);
    
    return res;
}

int test_asynch(unsigned short duration) {
    int mouseInterrupt, timer0Interrupt;
    unsigned long res;
    
    timerDuration = duration;
    
    int_init();

    write_kbc(CMD_REG, 0xD4) ; /* Write Byte to Mouse */    
    write_kbc(DATA_REG, 0xF4); /* Enable Sending Data Packets */
    
    if (sys_inb(DATA_REG, &res) != 0) {
        printf("read_kbc: sys_inb (2) failed.\n");
    }
    printf("DEBUG: acknowledge: 0x%X\n", res);
   
    
    printf("DEBUG: Subscribing mouse interruption\n");
    mouseInterrupt = int_subscribe(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouseAsynchCallback);
    printf("DEBUG: mouseInterrupt: %d\n", mouseInterrupt);
    
    printf("DEBUG: Subscribing Timer0 interruption\n");
    mouseInterrupt = int_subscribe(TIMER0_IRQ, IRQ_REENABLE, &timer0Callback);
    printf("DEBUG: timer0Interrupt: %d\n", timer0Interrupt);
    
    printf("DEBUG: STARTING_HANDLER.\n");
    
    
    
    res = int_start_handler();
    
    int_unsubscribe(mouseInterrupt);
    int_unsubscribe(timer0Interrupt);
    
    return res;
}
int test_config(void) {
    unsigned long res;
    unsigned char byte[3];

    write_kbc(CMD_REG, 0xD4) ; /* Write Byte to Mouse */    
    write_kbc(DATA_REG, 0xE9); /* Enable Sending Data Packets */
    
    if (sys_inb(DATA_REG, &res) != 0) {
        printf("read_kbc: sys_inb (2) failed.\n");
    }
    
    
    while (1) {
        sys_inb(DATA_REG, &res);
        if (!bit_isset(res, 7) && !bit_isset(res, 3))
            break;
    }
    byte[0] = res;
    
    while (1) {
        sys_inb(DATA_REG, &res);
        if (res == 0x00 || res == 0x01 || res == 0x02 || res == 0x03)
            break;
    }
    byte[1] = res;
   
    tickdelay(micros_to_ticks(50000));
    
    sys_inb(DATA_REG, &res);
    byte[2] = res;
    
    printf("Mode: %s\nEnable: %d\nScaling: %s\nLB: %d\nMB: %d\nRB: %d\nResolution: %d count/mm\nSample Rate: %d\n",
            (!!bit_isset(byte[0], 6) ? "Remote" : "Stream"),
            (!!bit_isset(byte[0], 5)),
            (!!bit_isset(byte[0], 4) ? "1:1" : "2:1"),
            (!!bit_isset(byte[0], 3)),
            (!!bit_isset(byte[0], 2)),
            (!!bit_isset(byte[0], 1)),
            0x1 << byte[1],
            byte[2]);
            
    return 0;
}

