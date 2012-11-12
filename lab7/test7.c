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
    printf("DEBUG: NOT IMPLEMENTED.\n");
    return 1;
}

int test_fifo(unsigned short base_addr, unsigned char tx, unsigned long bits, unsigned long stop, long parity, unsigned long rate, int stringc, char *strings[]) {
    printf("DEBUG: NOT IMPLEMENTED.\n");
    return 1;
}
