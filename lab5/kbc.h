#ifndef KBC_H_
#define KBC_H_

#define DELAY_US 20000

#define TIMEOUT_COUNTER 3

#define STAT_REG 0x64
#define DATA_REG 0x60
#define CMD_REG  0x64

#define LEDS_SWITCH 0xED

#define BIT(n) (0x01<<n)

#define OBF        BIT(0)
#define IBF        BIT(1)
#define AUX        BIT(5)
#define TO_ERR     BIT(6)
#define PAR_ERR    BIT(7)

#define ERROR 0xFE
#define REPEAT 0xFC

#define TIMER0_IRQ 0x00
#define KB_IRQ     0x01
#define MOUSE_IRQ  0x0C

int write_kbc(unsigned long port, unsigned char byte);
int read_kbc(void);

#endif /* KBC_H_ */
