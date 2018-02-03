#ifndef ZLG_PAGE2_HH
#define ZLG_PAGE2_HH
#include "./Page.h"
#include "../api/lcd.h"
extern "C"{
#include "../sys/delay.h"
#include "../sys/usart.h"
}
class Page2 : public Page
{
public:
	  Page2(){
			  
		}
		virtual void enter(){
			LOG_I("Page2 enter");
			timeCounter = 0;
		}
		virtual void onTimer(){
			  timeCounter++;
			  LOG_I("onTimer");
			  if(timeCounter >= 1000){
					lcd::jumpToPage(3);
				}
		}
		virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
		{
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