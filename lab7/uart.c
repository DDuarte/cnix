#include "uart.h"

#include <stdio.h>

#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/bitmap.h>

int uart_read(unsigned long port_addr, unsigned long offset, unsigned long* value) {
    int res;
    unsigned long temp;

    res = sys_inb(port_addr + offset, &temp);

    if (value != NULL)
        *value = temp;

    return res;
}

int uart_write(unsigned long port_addr, unsigned long offset, unsigned long value) {
    int res;

    res = sys_outb(port_addr + offset, value);

    return res;
}

int set_dlad(unsigned long port) {
    unsigned long lcr;

    if (uart_read(port, UART_LCR_REG, &lcr) != 0) {
        return 1;
    }

    bit_set(lcr, LCR_DLAB_BIT);

    if (uart_write(port, UART_LCR_REG, lcr) != 0) {
        return 1;
    }

    return 0;
}

int unset_dlad(unsigned long  port) {
    unsigned long lcr;

    if (uart_read(port, UART_LCR_REG, &lcr) != 0) {
        return 1;
    }

    bit_unset(lcr, LCR_DLAB_BIT);

    if (uart_write(port, UART_LCR_REG, lcr) != 0) {
        return 1;
    }

    return 0;
}

int getDL(unsigned long port, ushort_t* dl) {
    unsigned long temp;

    if (!dl)
        return 1;

    if (set_dlad(port) != 0) {
        return -1;
    }

    if (uart_read(port, UART_DL_LSB, &temp) != 0) {
        return -1;
    }

    dl->b.lsb = (unsigned char)temp;

    if (uart_read(port, UART_DL_MSB, &temp) != 0) {
        return -1;
    }

    dl->b.msb = (unsigned char)temp;

    if (unset_dlad(port) != 0) {
        return -1;
    }

    return 0;
}

int getConfig(unsigned long port, uart_config_t* dest) {
    unsigned long lcr;
    ushort_t dl;

    if (!dest)
        return -1;

    if (uart_read(port, UART_LCR_REG, &lcr) != 0) {
        return 1;
    }

    dest->parity = Parity(lcr);
    dest->stop = NumStopBits(lcr);
    dest->bits = NumBitsPerChar(lcr);
    getDL(port, &dl);
    dest->bitRate = BitRate(dl.w.value);

    return 0;
}
