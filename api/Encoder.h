#ifndef __STM32F10x_ENCODER_H
#define __STM32F10x_ENCODER_H
#include "../sys/delay.h"
#include "../sys/usart.h"
u8 ENCODER_Init(void (*onExitInterupt)(char,char));

#endif