
#ifndef __I8042_H
#define __I8042_H



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

#define	KBD_LEDS 0xed

#define KBD_LED(n) (1<<n)


#endif /* __I8042_H */
