#include "Keyboard.h"
extern "C"{
#include "../sys/delay.h"
#include "../sys/usart.h"
}
using namespace ext;
uint16_t Keyboard::key_prev = 0;
ScanLine scanLines[] = {
				{GPIOE,0x0800,0},
				{GPIOE,0x0400,1},
				{GPIOG,0x0002,2},
				{GPIOG,0x0001,3},
				{GPIOF,0x8000,4},
				{GPIOF,0x4000,5},
				{GPIOF,0x2000,6}
		};

const ExeCommand row0[] = {Numb_Dot,Numb_0,CMD_Enter,None,CMD_Start,CMD_Dn,CMD_Stop};
const ExeCommand row1[] = {Numb_7,Numb_8,Numb_9,CMD_Backspace,None,CMD_Input,None};
const ExeCommand row2[] = {Numb_4,Numb_5,Numb_6,CMD_RepeatMeas,CMD_Left,CMD_Setting,CMD_Right};
const ExeCommand row3[] = {Numb_1,Numb_2,Numb_3,CMD_Measure,CMD_WheelFix,CMD_Up,CMD_KnifeFix};

bool Keyboard::Init(){
					  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOE, DISABLE);
					  GPIO_InitTypeDef GPIO_InitStructure;
						GPIO_InitStructure.GPIO_Pin = INPinsE;
						GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
						GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
						GPIO_Init(GPIOE, &GPIO_InitStructure); 
					
						GPIO_InitStructure.GPIO_Pin = OutPinsE;
						GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
						GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
					  GPIO_Init(GPIOE, &GPIO_InitStructure); 
					
					  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOG, DISABLE);
					  GPIO_InitStructure.GPIO_Pin = OutPinsG;
						GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
						GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
					  GPIO_Init(GPIOG, &GPIO_InitStructure); 
					
					  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOF, DISABLE);
					  GPIO_InitStructure.GPIO_Pin = OutPinsF;
						GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
						GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
					  GPIO_Init(GPIOF, &GPIO_InitStructure); 
						
						RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB, DISABLE);
					  GPIO_InitStructure.GPIO_Pin = INReturnPins;
						GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
						GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
					  GPIO_Init(GPIOB, &GPIO_InitStructure);
					  LOG_I("keyboard Inited()");
					
					  GPIO_SetBits(GPIOE,0x0C00);//PE11,PE10,
						GPIO_SetBits(GPIOG,0x0003);//PG1,PG0
						GPIO_SetBits(GPIOF,0xE000);//PF15,PF14,PF13
					  return true;
				}

				uint16_t Keyboard::SubScanCol(uint16_t row){
					  uint16_t col = 0xFFFF;
					  GPIO_ResetBits(GPIOE,0x0C00);//PE11,PE10,
						GPIO_ResetBits(GPIOG,0x0003);//PG1,PG0
						GPIO_ResetBits(GPIOF,0xE000);//PF15,PF14,PF13
					  for(int i = 0; i < ARRAY_SIZE(scanLines); ++ i){
							 GPIO_SetBits(scanLines[i].GPIOx,scanLines[i].mask);
							 uint16_t read = GPIO_ReadInputData(GPIOE) & 0xF000;
							 if(read == row){
								   col = scanLines[i].col;
								   break;
							 }
						}
						GPIO_SetBits(GPIOE,0x0C00);//PE11,PE10,
						GPIO_SetBits(GPIOG,0x0003);//PG1,PG0
						GPIO_SetBits(GPIOF,0xE000);//PF15,PF14,PF13
					  return col;
				}
				
				const static uint16_t scanedRows[] = {0x1000,0x2000,0x4000,0x8000};
				ExeCommand Keyboard::Scan(){
					 ExeCommand cmd = None;
					 uint16_t keyDown = GPIO_ReadInputData(GPIOB) & 0x8000;
					 if(keyDown == 0){
						   if(key_prev == CMD_FanHui){
								  return None;
							 }
							 delay_ms(10);
							 keyDown = GPIO_ReadInputData(GPIOB) & 0x8000;
							 if(keyDown == 0){
								 LOG_I("Fan Hui Returned");
								 key_prev = CMD_FanHui;
								 return CMD_FanHui;
							 }
					 }
					 else if(key_prev == CMD_FanHui){
						 key_prev = None;
					 }
					 
					 uint16_t row = ext::Keyboard::ScanRow() & 0xF000;
					 if(row != key_prev){
						 if(row == 0x1000 || row == 0x2000 || row == 0x4000 || row == 0x8000){
								uint16_t col = ext::Keyboard::SubScanCol(row);
								LOG_I("key[0x%04x][0x%x]",row,col);
							  if(col == 0xFFFF){
									LOG_E("Error col 0x%x",col);
									return None;
								}
								key_prev = row;
							  if(row == 0x1000)return row0[col];
							  if(row == 0x2000)return row1[col];
							  if(row == 0x4000)return row2[col];
							  if(row == 0x8000)return row3[col];
						 }
						 else if((row & key_prev) == 0){
								LOG_I("key 0x%x up",key_prev);
								key_prev = 0;
						 }
						 else{
							 LOG_W("more than one key is pressed 0x%x",row);
						 }
					 }
					 return cmd;
				}		