#include "test7.h"
#include "uart.h"
#include <stdio.h>

int test_conf(unsigned short base_addr) {
    uart_config_t conf;

    if (uart_get_config(base_addr, &conf) != 0) {
        return -1;
    }

    printf("Parity: %s\nNumber of bits per character: %lu\nNumber of stop bits: %lu\nBit Rate: %lu bps\n",
            parity_str[conf.parity],
            conf.bits,
            conf.stop,
            conf.bitRate);

    printf("Received Data Interrupt enabled: %u\nTransmitter Transmitter Empty Interrupt enabled: %u\nReceiver Line Status Interrupt enabled: %u\n",
        conf.receivedDataInterrupt,
        conf.transmitterEmptyInterrupt,
        conf.receiverLineStatusInterrupt);

    return 0;
}

int test_set(unsigned short base_addr, unsigned long bits, unsigned long stop,
	     long parity, unsigned long rate) {
    uart_config_t conf;

    /* uart_get_config(base_addr, &conf); */

    conf.bitRate = rate;
    conf.bits = bits;
    conf.parity = parity;
    conf.stop = stop;

    if (uart_set_config(base_addr, conf) != 0) {
        return -1;
    }

    return 0;
}

int test_poll(unsigned short base_addr, unsigned char tx, unsigned long bits, unsigned long stop, long parity, unsigned long rate, int stringc, char *strings[]) {
    static uart_parity_t parities[] = { UART_NO_PARITY, UART_EVEN_PARITY, UART_ODD_PARITY };    
    uart_config_t prevConfig, newConfig;
    unsigned long lsr, chr, previer;
    unsigned int i, j, strLength;
    
    uart_get_config(base_addr, &prevConfing);
    newConfig = prevConfig;
    
    newConfig.bits = bits;
    newconfig.stop = stop;
    newConfig.bitrate = rate;
    newConfig.parity = parities[parity-1];
    
    
    uart_read(base_addr, UART_IER_REG, &previer);
    uart_write(base_addr, UART_IER_REG, 0);
    
    uart_set_config(base_addr, newConfig);
    
    if (tx == 0) { 
        do {
            uart_read(base_addr, UART_LSR_REG, &lsr);
            
            if (bit_isset(lsr, LSR_RECEIVER_READY_BIT) {
                uart_read(base_addr, UART_RBR_REG, &chr);
                printf("%c", (char)chr);
            }
        } while((char)chr != '.');
    } else {
        for (i = 0; i < stringc; i++) {
            strLength = strlen(strings[i]);
            for (j = 0; j < strLength; j++) {
                uart_write(base_addr, UART_RBR_REG, (unsigned long)strings[i][j];
            }
            uart_write(base_addr, UART_RBR_REG, (unsigned long)' ');
        }
    }
    
    uart_write(base_addr, UART_IER_REG, previer);
    uart_set_config(base_addr, prevConfig);
    
    return 0;
}

int test_fifo(unsigned short base_addr, unsigned char tx, unsigned long bits, unsigned long stop, long parity, unsigned long rate, int stringc, char *strings[]) {
    printf("DEBUG: NOT IMPLEMENTED.\n");
    return 1;
}
