#ifndef __UART_H__
#define __UART_H__

/* UART REGS */
#define UART_RBR_REG 0
#define UART_THR_REG 0
#define UART_IER_REG 1
#define UART_IIR_REG 2
#define UART_FCR_REG 2
#define UART_LCR_REG 3
#define UART_LSR_REG 5
#define UART_DL_LSB 0
#define UART_DL_MSB 1

/* UART COMs */
#define UART_COM1_ADDR 0x3F8
#define UART_COM2_ADDR 0x2F8
#define UART_COM1_IRQ 0x4
#define UART_COM2_IRQ 0x3

/* LCR BITS */
#define LCR_WORD_LENGTH0_BIT 0
#define LCR_WORD_LENGTH1_BIT 1
#define LCR_NO_STOP_BITS_BIT 2
#define LCR_PARITY0_BIT 3
#define LCR_PARITY1_BIT 4
#define LCR_PARITY2_BIT 5
#define LCR_SET_BREAK_ENABLE_BIT 6
#define LCR_DLAB_BIT 7

/* LSR BITS */
#define LSR_RECEIVER_READY_BIT 0
#define LSR_OVERRUN_ERROR_BIT 1
#define LSR_PARITY_ERROR_BIT 2
#define LSR_FRAMING_ERROR_BIT 3
#define LSR_BREAK_INTERRUPT_BIT 4
#define LSR_TRANSMITTER_HOLDING_REGISTER_EMPTY_BIT 5
#define LSR_TRANSMITTER_EMPTY_BIT 6

/* IER BITS */
#define IER_ENABLE_RECEIVED_DATA_INTERRUPT 0
#define IER_ENABLE_TRANSMITTER_EMPTY_INTERRUPT 1
#define IER_ENABLE_RECEIVER_LINE_STATUS_INTERRUPT 2
#define IER_ENABLE_MODEM_STATUS_INTERRUPT 3

/* IIR BITS */
#define IIR_INTERRUPT_STATUS_BIT 0
#define IIR_INTERRUPT_ORIGIN0_BIT 1
#define IIR_INTERRUPT_ORIGIN1_BIT 2
#define IIR_INTERRUPT_ORIGIN2_BIT 3
#define IIR_64_BYTE_FIFO_BIT 5
#define IIR_FIFO_STATUS0_BIT 6
#define IIR_FIFO_STATUS1_BIT 7

/* FCR BITS */
#define FCR_ENABLE_FIFO_BIT 0
#define FCR_CLEAR_RECEIVE_FIFO_BIT 1
#define FCR_CLEAR_TRANSMIT_FIFO_BIT 2
#define FCR_DMA_MODE_SELECT_BIT 3
#define FCR_ENABLE_64_BYTE_FIFO_BIT 5
#define FCR_FIFO_INTERRUPT_TRIGGER_LEVEL0_BIT 6
#define FCR_FIFO_INTERRUPT_TRIGGER_LEVEL1_BIT 7


#define FREQ 115200
#define BitRate(DL) (FREQ)/(DL)
#define DL(bitRate) (FREQ)/(bitRate)

#define UART_LCR_SET_BITS_PER_CHAR(lcr, bits)

#define UART_LCR_GET_BITS_PER_CHAR(lcr) (((lcr) & 0x3) + 5)
#define UART_LCR_GET_STOP_BITS(lcr) (((lcr) >> 2) + 1)
#define UART_LCR_GET_PARITY(lcr) (((lcr) & 0x38) >> 3)

typedef enum {
    UART_NO_PARITY = 0,
    UART_ODD_PARITY = 1,
    UART_EVEN_PARITY = 2,
    UART_ALWAYS_1_PARITY = 5,
    UART_ALWAYS_0_PARITY = 7
} uart_parity_t;

static const char* parity_str[] = { "None", "Odd", "Even", "", "", "Always 1", "", "Always 0" };

typedef struct {
    uart_parity_t parity;
    unsigned long stop;
    unsigned long bits;
    unsigned long bitRate;
    unsigned int receivedDataInterrupt : 1;
    unsigned int transmitterEmptyInterrupt : 1;
    unsigned int receiverLineStatusInterrupt : 1;
} uart_config_t;

typedef union {
    struct w {
        unsigned short value;
    } w;
    struct b {
        unsigned char lsb, msb;
    } b;
} ushort_t;

int uart_read(unsigned long port_addr, unsigned long offset, unsigned long* value);
int uart_write(unsigned long port_addr, unsigned long offset, unsigned long value);
int uart_get_config(unsigned long port, uart_config_t* dest);
int uart_set_config(unsigned long port, uart_config_t src);

int uart_set_dlab(unsigned long port);
int uart_unset_dlab(unsigned long port);
int uart_get_divisor_latch(unsigned long port, ushort_t* dl);
int uart_set_divisor_latch(unsigned long port, ushort_t dl);

#endif
