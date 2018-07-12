#include <stm32f10x.h>
#include <stm32f10x.h>
#include "Fpga.h"
using namespace ext;

bool Fpga::Init(){
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
			RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOE, DISABLE);
			
			GPIO_InitTypeDef GPIO_InitStructure;

			//Addr
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
			RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOF, DISABLE);
			GPIO_InitStructure.GPIO_Pin = OutAddrPinsF;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
			GPIO_Init(GPIOF, &GPIO_InitStructure);

			//Data Out
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
			RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOE, DISABLE);
			GPIO_InitStructure.GPIO_Pin = InDataPinsE;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
			GPIO_Init(GPIOE, &GPIO_InitStructure); 

			//RD/WR#,EN
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
			RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, DISABLE);
			GPIO_InitStructure.GPIO_Pin = OutRDWR_ENPinsC;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
			GPIO_Init(GPIOC, &GPIO_InitStructure); 
			LOG_I("Fpga Inited()");
		
			GPIO_ResetBits(GPIOE,0x00FF);//PE0 ~ PE7
			GPIO_ResetBits(GPIOF,0x000F);//PF0,PF1,PF2,PF3
			GPIO_ResetBits(GPIOC,0x6000);//PC14,PC13
			return true;
}

void setOuputMode(){
	    GPIO_InitTypeDef GPIO_InitStructure;
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
			RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOE, DISABLE);
			GPIO_InitStructure.GPIO_Pin = ext::Fpga::InDataPinsE;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
			GPIO_Init(GPIOE, &GPIO_InitStructure); 
}

void setInputMode(){
	    GPIO_InitTypeDef GPIO_InitStructure;
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
			RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOE, DISABLE);
			GPIO_InitStructure.GPIO_Pin = ext::Fpga::InDataPinsE;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
			GPIO_Init(GPIOE, &GPIO_InitStructure); 
}

#define SetAddr(addr)        GPIO_ResetBits(GPIOF,0x000F);GPIO_SetBits(GPIOF,addr)
#define SetData(data)        GPIO_ResetBits(GPIOE,0x00FF);GPIO_SetBits(GPIOE,data);
#define GetData()            (GPIO_ReadInputData(GPIOE) & 0xFF)

#define SetReadEnable()      GPIO_ResetBits(GPIOC,0x6000);GPIO_SetBits(GPIOC,0x2000);GPIO_SetBits(GPIOC,0x6000);setInputMode()

#define SetWriteEnable()     GPIO_ResetBits(GPIOC,0x6000);GPIO_ResetBits(GPIOC,0x2000);GPIO_SetBits(GPIOC,0x4000);setOuputMode()

#define SetIODisable()       GPIO_ResetBits(GPIOC,0x6000);

void Fpga::Write(u8 addr,u8 data){
	SetIODisable();
	SetAddr(addr);
	SetData(data);
	SetWriteEnable();
	SetIODisable();
	return;
}
u8  Fpga::Read(u8 addr){
	u8 ret = 0;
	SetIODisable();
	SetAddr(addr);
	SetReadEnable();
	ret = GetData();
	SetIODisable();
	return ret;
}
				