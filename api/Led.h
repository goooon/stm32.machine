#ifndef __STM32F10x_LED_H
#define __STM32F10x_LED_H

#include "stm32f10x.h"
#include "Tracer.h"
namespace ext
{
class Led
{
public:
    const static int LED_1 = 0x100; //wheel fix
    const static int LED_2 = 0x200; //processing
    const static int LED_3 = 0x400; //normal
    const static int WheelFixLed = 0x100;  //wheel fix
    const static int ProcessingLed = 0x200; //Processing
    const static int TranspantLed = 0x400;  //TranspantLed
    const static int OutLEDPinsD = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    static bool Init();
    static void SetLed(int LED_Index,bool onOff);
private:
};
}
#endif //__STM32F10x_FPGA_H