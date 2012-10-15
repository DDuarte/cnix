#include "kbc.h"

#include <minix/syslib.h>
#include <minix/drivers.h>

int write_kbc(unsigned long port, unsigned char byte) {
    unsigned long stat, counter = 0;
    
    while (counter < TIMEOUT_COUNTER) {
        if (sys_inb(STAT_REG, &stat) != 0) {
            printf("write_kbc: sys_inb failed.\n");
            return -1;
        }
        
        if(!(stat & IBF)) {
            if (sys_outb(port, byte) != 0) {
                printf("write_kbc: sys_outb failed.\n");
                return -1;
            }

            return 0;
        }

        if (tickdelay(micros_to_ticks(DELAY_US)) != 0) {
            printf("write_kbc: tickdelay failed.\n");
            return -1;
        }
        counter++;
    }
    
    return -1;
}

int read_kbc(void) {
    unsigned long stat, data, counter = 0;
    
    while (counter < TIMEOUT_COUNTER) {
        if (sys_inb(STAT_REG, &stat) != 0) {
            printf("read_kbc: sys_inb (1) failed.\n");
            return -1;
        }

        if ((stat & OBF) != 0) {
            if (sys_inb(DATA_REG, &data) != 0) {
                printf("read_kbc: sys_inb (2) failed.\n");
                return -1;
            }

            if ((stat & (PAR_ERR | TO_ERR)) == 0)
                return data;
            else if (data == ERROR)
                return data;
            else if (data == REPEAT)
                return data;
            else
                return -1;
        }

        if (tickdelay(micros_to_ticks(DELAY_US)) != 0) {
            printf("read_kbc: tickdelay failed.\n");
            return -1;
        }
        counter++;
    }
    
    return -1;
}
