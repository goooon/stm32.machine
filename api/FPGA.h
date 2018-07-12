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
		   //addr:  0x0     axis_degree_L(r)
		   //       0x1     axis_degree_H(r)
		   //       0x2     pulse_delay_L(r/w)
		   //       0x3     pulse_delay_H(r/w)
		   //       0xF:    fpga_work_mode(r/w), 0:transpant, 1:config fpga, 2:working, 3: reset
			 static void Write(u8 addr,u8 data);
		   static u8   Read(u8 adddr);
		 private:
		};
}
#define FPGA_START_WORKING() ext::Fpga::Write(15,2)
#define FPGA_RESET()  ext::Fpga::Write(15,3);ext::Fpga::Write(15,0)
#define FPGA_CONFIG() ext::Fpga::Write(15,0)
#define FPGA_SET_DELAYED_PULSE(count) ext::Fpga::Write(2,(u8)count);ext::Fpga::Write(3,(u8)(count >> 8))

#endif //__STM32F10x_FPGA_H