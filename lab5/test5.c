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
static unsigned int init = 0;

static void mouseCallback() {

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
    
    if (num_interrupts == 300) int_stop_handler();
    
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

int test_packet() {
    int mouseInterrupt;
    unsigned long res;
    
    int_init();
    
    write_kbc(CMD_REG, 0xD4) ; /* Write Byte to Mouse */    
    write_kbc(DATA_REG, 0xF5); /* Enable Sending Data Packets */
    
    if (sys_inb(DATA_REG, &res) != 0) {
        printf("read_kbc: sys_inb (2) failed.\n");
    }
    printf("DEBUG: acknowledge1: 0x%X\n", res);
    
    write_kbc(CMD_REG, 0xD4) ; /* Write Byte to Mouse */    
    write_kbc(DATA_REG, 0xF4); /* Enable Sending Data Packets */
    
    if (sys_inb(DATA_REG, &res) != 0) {
        printf("read_kbc: sys_inb (2) failed.\n");
    }
    printf("DEBUG: acknowledge2: 0x%X\n", res);
    
    printf("DEBUG: Subscribing interruption\n");
    mouseInterrupt = int_subscribe(12, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouseCallback);
    printf("DEBUG: mouseInterrupt: %d\n", mouseInterrupt);
    
    printf("DEBUG: STARTING_HANDLER.\n");
    
    res = int_start_handler();
    
    int_unsubscribe(mouseInterrupt);
    
    return res;
}

int test_asynch(unsigned short duration) {
    /* To be completed ... */
}
int test_config(void) {
    /* To be completed ... */
}
