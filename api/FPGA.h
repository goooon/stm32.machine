#ifndef __STM32F10x_FPGA_H
#define __STM32F10x_FPGA_H

#include "stm32f10x.h"
#include "Tracer.h"
namespace ext
{
	 class Fpga
	 {
		 public:
			 const static int OutAddrPinsF = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
		   const static int OutRDWR_ENPinsC = GPIO_Pin_13 | GPIO_Pin_14;
			 const static int OutDataPinsE = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
		   const static int InDataPinsE = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
		   static bool Init();
			 static void Write(u8 addr,u8 data);
		   static u8   Read(u8 adddr);
		 private:
		};
}
#endif //__STM32F10x_FPGA_H