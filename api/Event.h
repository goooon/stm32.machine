#ifndef __STM32F10x_EVENT_H
#define __STM32F10x_EVENT_H

#include "../base/type.h"

typedef enum {
	NoEvent,
	Timer1,
	Timer2,
	Timer3,
	UART2,
	UART3,
	PAGE,
	KEYCODE,
	EXITR
}EventType;

#ifdef __cplusplus
extern "C" {
bool PostEvent(EventType type,u32 hex,u32 dec,void* param);
bool ReadEvent(EventType* type,u32* hex,u32* dec,void** param);
};
#endif
#endif