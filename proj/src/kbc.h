#ifndef KBC_H_
#define KBC_H_

#include "utilities.h"

/** @defgroup kbc kbc
 * @{
 * Constants and functions for writing and reading from keyboard driver
 */

#define DELAY_US 20000 ///< Waiting time in ticks when reading/writing to keyboard

#define TIMEOUT_COUNTER 3 ///< Number of times until giving up when reading/writing to keyboard

#define STAT_REG 0x64 ///< Status register
#define DATA_REG 0x60 ///< Data register
#define CMD_REG  0x64 ///< Command register

#define LEDS_SWITCH 0xED ///< Keyboard leds

#define OBF        BIT(0)
#define IBF        BIT(1)
#define AUX        BIT(5) ///< Auxiliar data
#define TO_ERR     BIT(6)
#define PAR_ERR    BIT(7) ///< Paritiy error

#define ERROR 0xFE ///< Common keyboard error
#define REPEAT 0xFC ///< Repeat error
#define ACK 0xFA ///< Acknowledge (no error)

#define TIMER0_IRQ 0x00 ///< Timer IRQ
#define KB_IRQ     0x01 ///< Keyboard IRQ
#define MOUSE_IRQ  0x0C ///< Mouse IRQ

/**
 * @brief Writes byte to keyboard
 *
 * @param port Keyboard port
 * @param byte Byte to write
 * @return 0 if operation is successful, non-0 otherwise
 */
int write_kbc(unsigned long port, unsigned char byte);

/**
 * @brief Reads byte to keyboard
 *
 * @return Less than 0 if error, byte read otherwise
 */
int read_kbc(void);

/**@}*/

#endif /* KBC_H_ */
