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

static void _mouseCallback(void) {

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
        printf("B1=0x%02X B2=0x%02X B3=0x%02X LB=%d MB=%d RB=%d XOV=%d YOV=%d X=%04d Y=%04d\n", packet[0], packet[1], packet[2],
          bit_isset(packet[0], 0),
          bit_isset(packet[0], 2),
          bit_isset(packet[0], 1),
          bit_isset(packet[0], 6),
          bit_isset(packet[0], 7),
          !bit_isset(packet[0], 4) ? packet[1] : - (packet[1] + 1),
          !bit_isset(packet[0], 5) ? packet[2] : - (packet[2] + 1));
    }

}

static void mouseCallback(void) {
    _mouseCallback();
    if (num_interrupts == 300)
        int_stop_handler();
}

static void mouseAsynchCallback(void) {
    _mouseCallback();
}

static void timer0Callback(void) {
    timerCounter++;
    if (timerCounter / 60 == timerDuration)
        int_stop_handler();
}

int test_packet(void) {
    int mouseInterrupt;
    unsigned long res;

    int_init();

    if (write_kbc(CMD_REG, 0xD4) != 0) {/* Write Byte to Mouse */
        printf("test_packet: write_kbc CMD failed.\n");
        return 1;
    }
    if (write_kbc(DATA_REG, 0xF4) != 0) {/* Enable Sending Data Packets */
        printf("test_packet: write_kbc DATA failed.\n");
        return 1;
    }

    if (sys_inb(DATA_REG, &res) != 0) {
        printf("read_kbc: sys_inb failed.\n");
        return 1;
    }

    printf("DEBUG: acknowledge: 0x%X\n", res);
    printf("DEBUG: Subscribing interruption\n");

    mouseInterrupt = int_subscribe(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouseCallback);
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

    if (write_kbc(CMD_REG, 0xD4) != 0) {/* Write Byte to Mouse */
        printf("test_asynch: write_kbc CMD failed.\n");
        return 1;
    }

    if (write_kbc(DATA_REG, 0xF4) != 0) {/* Enable Sending Data Packets */
        printf("Test_asynch: write_kbc DATA failed.\n");
        return 1;
    }

    if (sys_inb(DATA_REG, &res) != 0) {
        printf("test_asynch: sys_inb failed.\n");
        return 1;
    }

    printf("DEBUG: acknowledge: 0x%X\n", res);

    printf("DEBUG: Subscribing mouse interruption\n");
    mouseInterrupt = int_subscribe(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouseAsynchCallback);
    printf("DEBUG: mouseInterrupt: %d\n", mouseInterrupt);

    printf("DEBUG: Subscribing Timer0 interruption\n");
    timer0Interrupt = int_subscribe(TIMER0_IRQ, IRQ_REENABLE, &timer0Callback);
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

    if (write_kbc(CMD_REG, 0xD4) != 0) {/* Write Byte to Mouse */
        printf("test_config: write_kbc CMD failed.\n");
        return 1;
    }

    if (write_kbc(DATA_REG, 0xE9)) {/* Enable Sending Data Packets */
        printf("test_config: write_kbc DATA failed.\n");
        return 1;
    }

    if (sys_inb(DATA_REG, &res) != 0) {
        printf("test_config: sys_inb failed.\n");
        return 1;
    }

    while (1) {
        sys_inb(DATA_REG, &res);
        if (!bit_isset(res, 7) && !bit_isset(res, 3))
            break;
    }

    byte[0] = res;

    while (1) {
        sys_inb(DATA_REG, &res);
        if (res <= 3)
            break;
    }

    byte[1] = res;

    tickdelay(micros_to_ticks(50000));

    if (sys_inb(DATA_REG, &res) != 0) {
        printf("test_config: sys_inb (2) failed.\n");
        return 1;
    }

    byte[2] = res;

    printf("Mode: %s\nEnable: %d\nScaling: %s\nLB: %d\nMB: %d\nRB: %d\nResolution: %d count/mm\nSample Rate: %d\n",
            bit_isset(byte[0], 6) ? "Remote" : "Stream",
            bit_isset(byte[0], 5),
            bit_isset(byte[0], 4) ? "1:1" : "2:1",
            bit_isset(byte[0], 3),
            bit_isset(byte[0], 2),
            bit_isset(byte[0], 1),
            0x1 << byte[1],
            byte[2]);

    return 0;
}
