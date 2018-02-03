#ifndef __STM32F10x_TIMER_H
#define __STM32F10x_TIMER_H
#include "stm32f10x.h"
#include "../base/type.h"
#ifdef __cplusplus
extern "C" {
#endif
	unsigned int GetSystemTicks();
	void Timer1_Init();
	void Timer1_DelayUS(uint us);
	void Timer1_DelayMS(uint ms);
	
	void Timer2_Init(unsigned int period,unsigned int prescaler,void (*callback)(void));
	void Timer3_Init(unsigned int period,unsigned int prescaler,void (*callback)(void));
	void Timer4_Init(unsigned int period,unsigned int prescaler,void (*callback)(void));
	void Timer5_Init(unsigned int period,unsigned int prescaler,void (*callback)(void));
	void Timer6_Init(unsigned int period,unsigned int prescaler,void (*callback)(void));
	void Timer7_Init(unsigned int period,unsigned int prescaler,void (*callback)(void));
#ifdef __cplusplus
}
#endif
#endif