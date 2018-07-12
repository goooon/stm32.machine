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
	//callback=prescaler* period/FLK
	void Timer2_Init(unsigned int period,unsigned int prescaler,void (*callback)(void*),void* userData);
	void Timer3_Init(unsigned int period,unsigned int prescaler,void (*callback)(void*),void* userData);
	void Timer4_Init(unsigned int period,unsigned int prescaler,void (*callback)(void*),void* userData);
	void Timer5_Init(unsigned int period,unsigned int prescaler,void (*callback)(void*),void* userData);
	void Timer6_Init(unsigned int period,unsigned int prescaler,void (*callback)(void*),void* userData);
	void Timer7_Init(unsigned int period,unsigned int prescaler,void (*callback)(void*),void* userData);
	void Timer7_Stop();
#ifdef __cplusplus
}
#endif
#endif