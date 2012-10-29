#include "interrupt.h"
#include "kbc.h"
#include "mouse.h"

#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/bitmap.h>

#include <stdio.h>
#include <stdarg.h>

#define DEBUG

#ifdef DEBUG
#define printfd printf
#else
#define printfd // 
#endif

static unsigned char packet[3] = { 0, 0, 0 };
static short counter = 0;
static int num_interrupts = 0;

static short timerDuration = 0;
static long timerCounter = 0;

typedef enum { MSI, MSR, MSD } mouse_state_m_t;

static mouse_state_m_t mouseState = MSI;

static void _mouseCallback(void) {

    unsigned long datatemp;
    if (mouse_read(&datatemp) != 0) {
        printf("read_kbc: mouse_read failed.\n");
    }
    packet[counter] = datatemp;

    if (counter == 0 && !bit_isset(packet[0], 3)) {
        return;
    }

    counter = (counter + 1) % 3;
    num_interrupts++;

    if (counter != 0)
        return;

    mouse_state.ldown = !!bit_isset(packet[0], 0); // LB
    mouse_state.mdown = !!bit_isset(packet[0], 2); // MB
    mouse_state.rdown = !!bit_isset(packet[0], 1); // RB
    mouse_state.ovfx =  !!bit_isset(packet[0], 6); // XOV
    mouse_state.ovfy =  !!bit_isset(packet[0], 7); // YOV
    mouse_state.diffx = !bit_isset(packet[0], 4) ? packet[1] : (char)(packet[1]);  // X
    mouse_state.diffy = !bit_isset(packet[0], 5) ? packet[2] : (char)(packet[2]); // Y


    switch (mouseState) {
    case MSI:
        if (mouse_state.ldown)
            mouseState = MSR;
        break;
    case MSR:
        if (!mouse_state.ldown || !mouse_state.rdown || mouse_state.mdown)
            mouseState = MSI;
        else
            mouseState = MSD;
        break;
    }


    printf("B1=0x%02X B2=0x%02X B3=0x%02X LB=%d MB=%d RB=%d XOV=%d YOV=%d X=%04d Y=%04d\n",
        packet[0], // B1
        packet[1], // B2
        packet[2], // B3
        mouse_state.ldown, // LB
        mouse_state.mdown, // MB
        mouse_state.rdown, // RB
        mouse_state.ovfx, // XOV
        mouse_state.ovfy, // YOV
        mouse_state.diffx, // X
        mouse_state.diffy); // Y

}

static void mouseCallback(void) {
    _mouseCallback();
    if (mouseState == MSD)
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
    unsigned long res, res1;

    int_init();

    if (mouse_enable_stream_mode() != 0) {
        printf("test_packet: mouse_enable_stream_mode failed.\n");
        return 1;
    }

    printfd("DEBUG: Subscribing interruption\n");

    mouseInterrupt = int_subscribe(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouseCallback);
    printfd("DEBUG: mouseInterrupt: %d\n", mouseInterrupt);

    printfd("DEBUG: STARTING_HANDLER.\n");

    res = int_start_handler();

    int_unsubscribe(mouseInterrupt);

    mouse_read(&res1); /* clear out buffer */
    
    return res;
}

int test_asynch(unsigned short duration) {
    int mouseInterrupt, timer0Interrupt;
    unsigned long res, res1;

    timerDuration = duration;

    int_init();

    if (mouse_write(ENABLE_SEND_PACKETS) != 0) {
        printf("test_packet: mouse_write failed.\n");
        return 1;
    }

    if (mouse_read(&res) != 0) {
        printf("test_asynch: mouse_read failed.\n");
        return 1;
    }

    printfd("DEBUG: acknowledge: 0x%X\n", res);

    printfd("DEBUG: Subscribing mouse interruption\n");
    mouseInterrupt = int_subscribe(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouseAsynchCallback);
    printfd("DEBUG: mouseInterrupt: %d\n", mouseInterrupt);

    printfd("DEBUG: Subscribing Timer0 interruption\n");
    timer0Interrupt = int_subscribe(TIMER0_IRQ, IRQ_REENABLE, &timer0Callback);
    printfd("DEBUG: timer0Interrupt: %d\n", timer0Interrupt);

    printfd("DEBUG: STARTING_HANDLER.\n");

    res = int_start_handler();

    int_unsubscribe(mouseInterrupt);
    int_unsubscribe(timer0Interrupt);

    mouse_read(&res1); /* clear out buffer */
    
    return res;
}

int test_config(void) {
    unsigned long res, res1;
    unsigned char byte[3];

    if (mouse_write(GET_MOUSE_STATUS) != 0) {
        printf("test_packet: mouse_write failed.\n");
        return 1;
    }

    if (mouse_read(&res) != 0) {
        printf("test_config: mouse_read failed.\n");
        return 1;
    }

    while (1) {
        mouse_read(&res);
        if (!bit_isset(res, 7) && !bit_isset(res, 3))
            break;
    }

    byte[0] = res;

    while (1) {
        mouse_read(&res);
        if (res <= 3)
            break;
    }

    byte[1] = res;

    tickdelay(micros_to_ticks(5000)); // waiting for the mouse

    if (mouse_read(&res) != 0) {
        printf("test_config: mouse_read (2) failed.\n");
        return 1;
    }

    byte[2] = res;

    mouse_status.mode = bit_isset(byte[0], 6);
    mouse_status.enable = !!bit_isset(byte[0], 5);
    mouse_status.scaling = bit_isset(byte[0], 4);
    mouse_status.ldown = !!bit_isset(byte[0], 3);
    mouse_status.mdown = !!bit_isset(byte[0], 2);
    mouse_status.rdown = !!bit_isset(byte[0], 1);
    mouse_status.resolution = 0x1 << byte[1];
    mouse_status.rate = byte[2];

    printf("Mode: %s\nEnable: %d\nScaling: %s\nLB: %d\nMB: %d\nRB: %d\nResolution: %d count/mm\nSample Rate: %d\n",
        mouse_status.mode ? "Remote" : "Stream",
        mouse_status.enable,
        mouse_status.scaling ? "1:1" : "2:1",
        mouse_status.ldown,
        mouse_status.mdown,
        mouse_status.rdown,
        mouse_status.resolution,
        mouse_status.rate);
  
    mouse_read(&res1); /* clear out buffer */
    
    return 0;
}
