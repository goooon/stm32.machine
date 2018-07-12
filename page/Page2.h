#ifndef ZLG_PAGE2_HH
#define ZLG_PAGE2_HH
#include "./Page.h"
#include "../api/lcd.h"
#include "../api/FPGA.h"
#include "../setting/Setting.h"
extern "C"{
#include "../sys/delay.h"
#include "../sys/usart.h"
}
class Page2 : public Page
{
public:
	  Page2():timeCounter(0){
			  
		}
		virtual void enter(){
			LOG_I("Page2 enter");
			timeCounter = 0;
		}
		virtual void onTimer(){
			  timeCounter++;
			  if(timeCounter >= 20){
					if(Setting::needPresetParameters()){
						lcd::jumpToPage(4);
					}
					else{
						lcd::jumpToPage(3);
					}
					timeCounter = 0;
				}
		}
		virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
		{
			  u8 data;
				LOG_I("key pressed:%c/0x%x",cmd,cmd);
			  switch(cmd){
						 case ext::CMD_Enter:
							lcd::jumpToPage(3);
							return cmd;
						break;
				}
		}
		private:
			int timeCounter;
};
#endif