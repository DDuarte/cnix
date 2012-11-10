#include "test7.h"
#include "uart.h"

int test_conf(unsigned short base_addr) {
    uart_config_t conf;

    if (getConfig(base_addr, &conf) != 0) {
        return -1;
    }

    printf("Parity: %s\nNumber of bits per character: %d\nNumber of stop bits: %d\nBit Rate: %d\n",
            parity_str[conf.parity],
            conf.bits,
            conf.stop,
            conf.bitRate);

    return 0;
}

int test_set(unsigned short base_addr, unsigned long bits, unsigned long stop,
	     long parity, unsigned long rate) {
    printf("DEBUG: NOT IMPLEMENTED.\n");
}

int test_poll(/* details to be provided */) {
    printf("DEBUG: NOT IMPLEMENTED.\n");
}

int test_int(/* details to be provided */) {
    printf("DEBUG: NOT IMPLEMENTED.\n");
}

int test_fifo(/* details to be provided */) {
    printf("DEBUG: NOT IMPLEMENTED.\n");
}
