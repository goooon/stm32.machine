#include "Led.h"
using namespace ext;
bool Led::Init(){
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOD, DISABLE);

		GPIO_InitStructure.GPIO_Pin = OutLEDPinsD;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
	
		GPIO_ResetBits(GPIOD,0x700);//PB12 ~ PB14
		return true;
}

void Led::SetLed(int LED_Index,bool onOff){
	   if(onOff){
			 GPIO_SetBits(GPIOD,LED_Index);
		 }
		 else{
			 GPIO_ResetBits(GPIOD,LED_Index);
		 }
}
				