#ifndef __STM32F10x_LED_H
#define __STM32F10x_LED_H

#include "stm32f10x.h"
#include "Tracer.h"
namespace ext
{
	 class Led
	 {
		 public:
		   const static int LED_1 = 0x1000;
		   const static int LED_2 = 0x2000;
		   const static int LED_3 = 0x4000;
			 const static int OutLEDPinsB = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
		   static bool Init();
			 static void SetLed(int LED_Index,bool onOff);
		 private:
		};
}
#endif //__STM32F10x_FPGA_H