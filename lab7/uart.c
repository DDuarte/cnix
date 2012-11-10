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

int uart_get_config(unsigned long port, uart_config_t* dest) {
    unsigned long lcr;
    ushort_t dl;

    if (!dest)
        return -1;

    if (uart_read(port, UART_LCR_REG, &lcr) != 0) {
        return 1;
    }

    dest->parity = UART_LCR_GET_PARITY(lcr);
    dest->stop = UART_LCR_GET_STOP_BITS(lcr);
    dest->bits = UART_LCR_GET_BITS_PER_CHAR(lcr);

    if (uart_get_divisor_latch(port, &dl) != 0) {
        return 1;
    }
    dest->bitRate = BitRate(dl.w.value);

    return 0;
}

int uart_set_config(unsigned long port, uart_config_t src) {
    unsigned long lcr;
    ushort_t dl;

    /* read existing configuration */
    if (uart_read(port, UART_LCR_REG, &lcr) != 0) {
        return 1;
    }

    /* bits per char */
    lcr &= ~0x3;
    lcr |= ((src.bits - 5) << LCR_WORD_LENGTH0_BIT);

    /* stop bits */
    lcr &= ~0x4;
    lcr |= ((src.stop - 1) << LCR_NO_STOP_BITS_BIT);

    /* parity */
    lcr &= ~0x38;
    lcr |= ((src.parity) << LCR_PARITY0_BIT);

    /* rate */
    dl.w.value = DL(src.bitRate);
    if (uart_set_divisor_latch(port, dl) != 0) {
        return 1;
    }

    /* overwrite config */
    if (uart_write(port, UART_LCR_REG, lcr) != 0) {
        return 1;
    }

    return 0;
}

int uart_set_dlab(unsigned long port) {
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

int uart_unset_dlab(unsigned long  port) {
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

int uart_get_divisor_latch(unsigned long port, ushort_t* dl) {
    unsigned long temp;

    if (!dl)
        return 1;

    if (uart_set_dlab(port) != 0) {
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

    if (uart_unset_dlab(port) != 0) {
        return -1;
    }

    return 0;
}

int uart_set_divisor_latch(unsigned long port, ushort_t dl) {
    unsigned long temp;

    if (uart_set_dlab(port) != 0) {
        return -1;
    }

    temp = (unsigned long)dl.b.lsb;

    if (uart_write(port, UART_DL_LSB, temp) != 0) {
        return -1;
    }

    temp = (unsigned long)dl.b.msb;

    if (uart_write(port, UART_DL_MSB, temp) != 0) {
        return -1;
    }

    if (uart_unset_dlab(port) != 0) {
        return -1;
    }

    return 0;
}
