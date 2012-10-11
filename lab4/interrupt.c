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

static interrupt int_data[32] = { { -1, NULL }, { -1, NULL },{ -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL },{ -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL },{ -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL },{ -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL }, { -1, NULL } };

static unsigned short executing = 0;

int int_subscribe(int irq_line, int policy, void (*callback)()/* = NULL*/) {
    int r, bit = 0;
    
    while(int_data[bit].hook_id != -1 && bit < 32) { bit++; }
    
    if (bit == 32) {
        printf("int_subscribe: Can't subscribe more than 32 interruptions\n");
        return -1;
    }
    
    r = sys_irqsetpolicy(irq_line, policy, &(int_data[bit].hook_id));
    if (r != 0) {
        if (r == EINVAL) {
            printf("int_unsubscribe: sys_irqdisable failed with: Invalid request, IRQ line, hook id, or process number.\n");
            return -1;
        } else if (r == EPERM) {
            printf("int_unsubscribe: sys_irqdisable failed with: Only owner of hook can toggle interrupts or release the hook.\n");
            return -1;
        } else if (r == ENOSPC) {
            printf("int_unsubscribe: sys_irqdisable failed with:  No free IRQ hook could be found.\n");
            return -1;
        }
    
        /*printf("int_subscribe: sys_irqsetpolicy failed with: %d\n", r);*/
        int_data[bit].hook_id = -1;
        int_data[bit].callback = NULL;
        return -1;
    }
    
    r = sys_irqenable(&(int_data[bit].hook_id));
    if (r != 0) {
        printf("int_subscribe: sys_irqenable failed with: %d\n", r);
        int_data[bit].hook_id = -1;
        int_data[bit].callback = NULL;
        return -1;
    }
    
    int_data[bit].callback = callback;
    
    return bit;        
}

int int_unsubscribe(bit)
{
    int r;
    
    r = sys_irqdisable(&(int_data[bit].hook_id));
    if (r == EINVAL) {
        printf("int_unsubscribe: sys_irqdisable failed with: Invalid request, IRQ line, hook id, or process number.\n");
        return -1;
    } else if (r == EPERM) {
        printf("int_unsubscribe: sys_irqdisable failed with: Only owner of hook can toggle interrupts or release the hook.\n");
        return -1;
    } else if (r == ENOSPC) {
        printf("int_unsubscribe: sys_irqdisable failed with:  No free IRQ hook could be found.\n");
        return -1;
    }
    
    r = sys_irqrmpolicy(&(int_data[bit].hook_id));
    if (r != 0) {
        printf("nt_unsubscribe: sys_irqrmpolicy failed with : %d\n", r);
        return -1;
    }
    
    int_data[bit].hook_id = -1;
    int_data[bit].callback = NULL;
    
    return 0;
}

static int int_handle() {
    
    message msg;
    int ipc_status;
    int r = 0;
    
    while (r < 32 && int_data[r].hook_id == -1) { r++; }
    
    if (r == 32) {
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
                    for (r = 0; r < 32; r++) {
                        if (int_data[r].hook_id != -1 && bit_isset(msg.NOTIFY_ARG, r) && int_data[r].callback != NULL) {
                            int_data[r].callback();
                        }
                    }
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






