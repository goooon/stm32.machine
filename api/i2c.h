#ifndef _I2C_h
#define _I2C_H
#include "stm32f10x.h"
#include "../sys/delay.h"
#define At24c02_address 0xa0

void I2C1_init(void);
u8 At24c02_write(u8 page,u8 address,u8* buff,u8 count);
u8 At24c02_read(u8 page,u8 address,u8* buff,u8 count);
u8 At24c02_w8(u8 page,u8 address,u8* buff,u8 count);
#endif
