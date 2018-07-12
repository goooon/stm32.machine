#ifndef __STM32F10x_KEYBOARD_H
#define __STM32F10x_KEYBOARD_H

#include "stm32f10x.h"
#include "Tracer.h"
namespace ext
{
	  typedef struct 
		{
				GPIO_TypeDef* GPIOx;
				uint16_t mask;
				uint8_t  col;
		}ScanLine;
		
		typedef enum{
					 None = ' ',
					 Numb_Dot = '.',
					 Numb_0 = '0',
					 Numb_1 = '1',
					 Numb_2 = '2',
					 Numb_3 = '3',
					 Numb_4 = '4',
					 Numb_5 = '5',
					 Numb_6 = '6',
					 Numb_7 = '7',
					 Numb_8 = '8',
					 Numb_9 = '9',
					 CMD_Enter = 'E',
					 CMD_Backspace='B',
					 CMD_RepeatMeas = 'R',
					 CMD_Measure='M',
					 CMD_Start = '{',
					 CMD_Stop = '}',
					 CMD_Up = 'A',
					 CMD_Dn = 'V',
					 CMD_Left = '<',
					 CMD_Right = '>',
					 CMD_Input='I',//输入
					 CMD_Setting='S',//设置
					 CMD_WheelFix='W',//手轮修正
					 CMD_KnifeFix='K',//进刀修正
					 CMD_FanHui = 'F'
				}ExeCommand;
				
	 class Keyboard
	 {
		 public:
			const static int INPinsE = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
		   	const static int OutPinsE =GPIO_Pin_10 | GPIO_Pin_11;
		   	const static int OutPinsG =GPIO_Pin_0 | GPIO_Pin_1;
		   	const static int OutPinsF =GPIO_Pin_13 | GPIO_Pin_14 |GPIO_Pin_15;
		   	const static int INReturnPins = GPIO_Pin_15;
		   	static bool Init();
			static uint16_t ScanRow(){
					  return  GPIO_ReadInputData(GPIOE);
				}
			static uint16_t key_prev;
			static uint16_t SubScanCol(uint16_t row);
			static ExeCommand Scan();					
			};
		}
#endif