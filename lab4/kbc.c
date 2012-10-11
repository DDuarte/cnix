#include "kbc.h"

#include <minix/syslib.h>
#include <minix/drivers.h>

int write_kbc(unsigned long port, unsigned char byte) {
    unsigned long stat, counter = 0;
    
    while (counter < 1000) {
        sys_inb(STAT_REG, &stat);
        
        if((stat & IBF) == 0){
            sys_outb(port, byte);
            return 0;
        }
        tickdelay(micros_to_ticks(DELAY_US));
        counter++;
    }
    
    return -1;
}

int read_kbc() {
    unsigned long stat, data, counter = 0;
    
    while (counter < 1000) {
        sys_inb(STAT_REG, &stat);
        
        if ((stat & OBF) != 0) {
            sys_inb(DATA_REG, &data);
            if ((stat & (PAR_ERR | TO_ERR)) == 0)
                return data;
            else if (data == ERROR)
                return data;
            else if (data == REPEAT)
                return data;
            else
                return -1;
        }
        tickdelay(micros_to_ticks(DELAY_US));
        counter++;
    }
    
    return -1;
}