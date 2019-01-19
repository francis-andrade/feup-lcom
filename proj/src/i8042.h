
#ifndef __I8042_H
#define __I8042_H

/** @defgroup i8042 i8042

 * @{

 *

 * Constants for programming the i8042 controller.

 */
/**
 * @name KBC
 */
/*@{*/
#define KBC_IRQ	1


#define KBC_OUT_BUF 0x60
#define KBC_IN_BUF 0x60
#define KBC_STAT_REG 0x64
#define KBC_CMD_REG 0x64

#define CKBD_ACK 0xfa
#define CKBD_RESEND 0xfe
#define CKBD_ERROR 0xfc


#define KBC_OBF BIT(0)
#define KBC_IBF BIT(1)
#define KBC_PAR_ERROR BIT(7)
#define KBC_TO_ERROR BIT(6)
#define KBC_WRITE_TO_MOUSE 0xD4

#define	KBD_LEDS 0xed

#define KBD_LED(n) (1<<n)
/*@}*/

/**
 * @name MOUSE
 */
/*@{*/
#define MOUSE_IRQ 12
#define MOUSE_RESET 0xff
#define MOUSE_DISABLE 0xf5
#define MOUSE_ENABLE 0xf4
#define MOUSE_SET_STREAM 0xea
#define MOUSE_DISABLE_STREAM_MODE 0XF5
#define MOUSE_STATUS_REQUEST 0xE9
/*@}*/
/** @} end of i8042 */
#endif /* __I8042_H */
