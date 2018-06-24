#ifndef __STM32F10x_LCD_H
#define __STM32F10x_LCD_H
extern "C"{
#include "../sys/delay.h"
#include "../sys/usart.h"
}	
//http://www.viewtech.cn/
namespace lcd
{
	  void reset();
		void readCurrPage();
	  void jumpToPageDirect(short page);
		void jumpToPage(short page);
		void parseCodes(char* codes,int len);
	  void sendKeycode(char code);
		void sendAddrValue(short addr,short value);
	  void sendAddrValue(short addr,short value1,short value2);
		void sendAddrValueLong(short addr,unsigned long value);
		void displayUnicode(short addr,short* buff,short len);
	  void displayUnicodeQueue(short addr,short* buff,short len);
}
#endif