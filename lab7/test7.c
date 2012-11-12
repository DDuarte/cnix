#include "test7.h"
#include "uart.h"
#include <stdio.h>
#include <minix/bitmap.h>
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
    static uart_parity_t parities[] = { UART_NO_PARITY, UART_EVEN_PARITY, UART_ODD_PARITY };
    uart_config_t conf;

    /* uart_get_config(base_addr, &conf); */

    conf.bitRate = rate;
    conf.bits = bits;
    conf.parity = parities[parity+1];
    conf.stop = stop;

    if (uart_set_config(base_addr, conf) != 0) {
        return -1;
    }

    return 0;
}

void wait_to_be_ready(unsigned short base_addr) {
    unsigned long lsr;
    do {
        uart_read(base_addr, UART_LSR_REG, &lsr);
        tickdelay(20);
    } while(!bit_isset(lsr, LSR_TRANSMITTER_HOLDING_REGISTER_EMPTY_BIT));
}

int test_poll(unsigned short base_addr, unsigned char tx, unsigned long bits, unsigned long stop, long parity, unsigned long rate, int stringc, char *strings[]) {
    static uart_parity_t parities[] = { UART_NO_PARITY, UART_EVEN_PARITY, UART_ODD_PARITY };
    uart_config_t prevConfig, newConfig;
    unsigned long lsr, chr, previer, prevfcr;
    unsigned int i, j, strLength;

    uart_get_config(base_addr, &prevConfig);
    newConfig = prevConfig;

    newConfig.bits = bits;
    newConfig.stop = stop;
    newConfig.bitRate = rate;
    newConfig.parity = parities[parity-1];


    uart_read(base_addr, UART_IER_REG, &previer);
    uart_write(base_addr, UART_IER_REG, 0);

    uart_read(base_addr, UART_FCR_REG, &prevfcr);
    uart_write(base_addr, UART_FCR_REG, prevfcr & ~FCR_ENABLE_FIFO_BIT);

    uart_set_config(base_addr, newConfig);


    if (tx == 0) { /* a receber */

        do {
            wait_to_be_ready(base_addr);
            uart_read(base_addr, UART_RBR_REG, &chr);
            printf("%c", (char)chr);
        } while((char)chr != '.');
        printf("\n");

    } else {
        for (i = 0; i < stringc; i++) {
            strLength = strlen(strings[i]);
            for (j = 0; j < strLength; j++) {
                wait_to_be_ready(base_addr);
                uart_write(base_addr, UART_RBR_REG, (unsigned long)strings[i][j]);
            }
            wait_to_be_ready(base_addr);
            uart_write(base_addr, UART_RBR_REG, (unsigned long)' ');
        }
        wait_to_be_ready(base_addr);
        uart_write(base_addr, UART_RBR_REG, (unsigned long)'.');
    }

    uart_write(base_addr, UART_IER_REG, previer);
    uart_write(base_addr, UART_FCR_REG, prevfcr);
    uart_set_config(base_addr, prevConfig);

    return 0;
}

int test_fifo(unsigned short base_addr, unsigned char tx, unsigned long bits, unsigned long stop, long parity, unsigned long rate, int stringc, char *strings[]) {
    printf("DEBUG: NOT IMPLEMENTED.\n");
    return 1;
}
