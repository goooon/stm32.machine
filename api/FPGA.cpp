#include <stm32f10x.h>
#include <stm32f10x.h>
#include "Fpga.h"
using namespace ext;

bool Fpga::Init(){
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);//配置时钟	使能PE口的时钟源
			RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOE, DISABLE);//配置时钟 关闭PE口的时钟源
			
			GPIO_InitTypeDef GPIO_InitStructure; //声明配置GPIO口的结构体

			//Addr配置地址位
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);	 //??????????这里为什么要设置RCC_APB2  APB2是什么东西
			RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOF, DISABLE);
			GPIO_InitStructure.GPIO_Pin = OutAddrPinsF;//开启那几位：OutAddrPinsF = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;//设置速度
			GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIOF的IO口

			//Data Out配置数据位
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
			RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOE, DISABLE);
			GPIO_InitStructure.GPIO_Pin = InDataPinsE;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
			GPIO_Init(GPIOE, &GPIO_InitStructure); 

			//RD/WR#,EN配置读写和使能位
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
			RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, DISABLE);
			GPIO_InitStructure.GPIO_Pin = OutRDWR_ENPinsC;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
			GPIO_Init(GPIOC, &GPIO_InitStructure); 
			LOG_I("Fpga Inited()");
		
			//电平设置为底
			GPIO_ResetBits(GPIOE,0x00FF);//PE0 ~ PE7
			GPIO_ResetBits(GPIOF,0x000F);//PF0,PF1,PF2,PF3
			GPIO_ResetBits(GPIOC,0x6000);//PC14,PC13
			return true;
}

void setOuputMode(){//STM32向FPGA输出的配置
	    GPIO_InitTypeDef GPIO_InitStructure;
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOE, DISABLE);
		GPIO_InitStructure.GPIO_Pin = ext::Fpga::InDataPinsE;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOE, &GPIO_InitStructure); 
}

void setInputMode(){//STM32输入的配置
	    GPIO_InitTypeDef GPIO_InitStructure;
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOE, DISABLE);
		GPIO_InitStructure.GPIO_Pin = ext::Fpga::InDataPinsE;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOE, &GPIO_InitStructure); 
}

#define SetAddr(addr)        GPIO_ResetBits(GPIOF,0x000F);GPIO_SetBits(GPIOF,addr)
#define SetData(data)        GPIO_ResetBits(GPIOE,0x00FF);GPIO_SetBits(GPIOE,data);
#define GetData()            (GPIO_ReadInputData(GPIOE) & 0xFF)//获取GPIOE口的低八位数据

#define SetReadEnable()      GPIO_ResetBits(GPIOC,0x6000);GPIO_SetBits(GPIOC,0x2000);GPIO_SetBits(GPIOC,0x6000);setInputMode()

#define SetWriteEnable()     GPIO_ResetBits(GPIOC,0x6000);GPIO_ResetBits(GPIOC,0x2000);GPIO_SetBits(GPIOC,0x4000);setOuputMode()

#define SetIODisable()       GPIO_ResetBits(GPIOC,0x6000);

void Fpga::Write(u8 addr,u8 data){ //向FPGA写数据
	SetIODisable();
	SetAddr(addr);
	SetData(data);
	SetWriteEnable();
	SetIODisable();
	return;
}
u8  Fpga::Read(u8 addr){//向FPGA读取数据
	u8 ret = 0;
	SetIODisable();
	SetAddr(addr);
	SetReadEnable();
	ret = GetData();
	SetIODisable();
	return ret;
}
				