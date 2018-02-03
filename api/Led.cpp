#include "Led.h"
using namespace ext;
bool Led::Init(){
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB, DISABLE);

		GPIO_InitStructure.GPIO_Pin = OutLEDPinsB;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		GPIO_ResetBits(GPIOB,0x7000);//PB12 ~ PB14
		return true;
}

void Led::SetLed(int LED_Index,bool onOff){
	   if(onOff){
			 GPIO_SetBits(GPIOB,LED_Index);
		 }
		 else{
			 GPIO_ResetBits(GPIOB,LED_Index);
		 }
}
				