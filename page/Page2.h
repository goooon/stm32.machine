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

//启动时，自检通过 选择界面
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
			  if(timeCounter >= 20){ //2S，判断用户是否有点击选项
					if(Setting::needPresetParameters()){ //调用点击跳转
						lcd::jumpToPage(4);//用户点击了相应的选项按钮，跳转到页面4
					}
					else{
						lcd::jumpToPage(3);//用户没有点击按钮，跳转到页面3
					}
					timeCounter = 0;
				}
		}
		//在本界面等待时，如果用户点击键盘enter按键，那么久进入页面3
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