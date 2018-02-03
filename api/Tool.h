#ifndef __STM32F10x_TOOL_H
#define __STM32F10x_TOOL_H
#include "../sys/delay.h"
#include "../sys/usart.h"	
namespace tool
{
	  int convertFixed(s32 numb,s32 digit,short unicode[],uint size,bool showPlus = true,short ending = 0xffff);
	  int convertPulseToAngle(u32 pulse,u32 circulePulses,short unicode[],uint size);
	  
}

#endif