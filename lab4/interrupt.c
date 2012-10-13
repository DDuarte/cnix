#include "interrupt.h"

#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/com.h>
#include <minix/bitmap.h>
#include <stdio.h>

typedef struct
{
    int hook_id;
    void (*callback)();
} interrupt;

#define NUM_OF_INTERRUPTS 32

static interrupt int_data[NUM_OF_INTERRUPTS] = { { -1, NULL }, { -1, NULL },{ -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL },{ -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL },{ -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL },{ -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL } };

static int executing = 0;

int int_subscribe(int irq_line, int policy, void (*callback)()) {
    int r, bit = 0;

    for (r = 0; r < NUM_OF_INTERRUPTS; ++r)
        printf("%d, %d", int_data[r].hook_id, int_data[r].callback);
    
    while (int_data[bit].hook_id != -1 && bit < NUM_OF_INTERRUPTS) { bit++; }
    
    if (bit == NUM_OF_INTERRUPTS) {
        printf("int_subscribe: Can't subscribe more than 32 interruptions\n");
        return -1;
    }

    int_data[bit].hook_id = bit;

    r = sys_irqsetpolicy(irq_line, policy, &int_data[bit].hook_id);
    if (r != 0) {
        if (r == EINVAL)
            printf("int_subscribe: sys_irqsetpolicy failed with: Invalid request, IRQ line, hook id, or process number.\n");
        else if (r == EPERM)
            printf("int_subscribe: sys_irqsetpolicy failed with: Only owner of hook can toggle interrupts or release the hook.\n");
        else if (r == ENOSPC)
            printf("int_subscribe: sys_irqsetpolicy failed with: No free IRQ hook could be found.\n");
        else
            printf("int_subscribe: sys_irqsetpolicy failed with error %d.\n", r);

        int_data[bit].hook_id = -1;
        int_data[bit].callback = NULL;
        return -1;
    }

    r = sys_irqenable(&int_data[bit].hook_id);
    if (r != 0) {
        printf("int_subscribe: sys_irqenable failed with: %d\n", r);
        int_data[bit].hook_id = -1;
        int_data[bit].callback = NULL;
        return -1;
    }
    
    int_data[bit].callback = callback;
    
    return bit;        
}

int int_unsubscribe(int bit)
{
    int r;
    
    r = sys_irqdisable(&int_data[bit].hook_id);
    if (r != 0) {
        if (r == EINVAL)
            printf("int_unsubscribe: sys_irqdisable failed with: Invalid request, IRQ line, hook id, or process number.\n");
        else if (r == EPERM)
            printf("int_unsubscribe: sys_irqdisable failed with: Only owner of hook can toggle interrupts or release the hook.\n");
        else if (r == ENOSPC)
            printf("int_unsubscribe: sys_irqdisable failed with: No free IRQ hook could be found.\n");
        else
            printf("int_unsubscribe: sys_irqdisable failed with error %d.\n", r);
    }
    else
    {
        r = sys_irqrmpolicy(&int_data[bit].hook_id);
        if (r != 0)
            printf("int_unsubscribe: sys_irqrmpolicy failed with : %d\n", r);
    }

    int_data[bit].hook_id = -1;
    int_data[bit].callback = NULL;

    return 0; /* never fails (but prints errors if interrupt was not enabled) */
}

static int int_handle() {
    
    message msg;
    int ipc_status;
    int r = 0, i;
    
    while (r < NUM_OF_INTERRUPTS && int_data[r].hook_id == -1) { r++; }
    
    if (r == NUM_OF_INTERRUPTS) {
        printf("int_handle: no interrupts where subscribed\n");
        return -1;
    }
    
    while(executing != 0) {
        r = driver_receive(ANY, &msg, &ipc_status);
        if (r != 0) {
            printf("int_handle: driver_receive failed with: %d\n", r);
            continue;
        }
        
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    for (i = 0; i < NUM_OF_INTERRUPTS; i++)
                        if (int_data[i].hook_id != -1 && int_data[r].callback != NULL)
                            if (bit_isset(msg.NOTIFY_ARG, i))
                                int_data[i].callback();
                    break;
                default:
                    break;
            }
        }
    }
    
    return 0;
}

int int_start_handler() {
    executing = 1;
    return int_handle();
}

void int_stop_handler() {
    executing = 0;
}
