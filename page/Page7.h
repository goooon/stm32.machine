#ifndef ZLG_PAGE7_HH
#define ZLG_PAGE7_HH
#include "./Page.h"
#include "../api/lcd.h"
#include "../setting/Setting.h"
class Page7 : public Page
{
public:
	virtual void enter(){
		index = 0;
		lcd::sendAddrValue(0x710,index == 0 ? 0 : 1);
		lcd::sendAddrValue(0x714,index == 1 ? 0 : 1);
	}
	virtual void leave(){
		index = 0;
		lcd::sendAddrValue(0x710,index == 0 ? 0 : 1);
		lcd::sendAddrValue(0x714,index == 1 ? 0 : 1);
	}
	virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
	{
			LOG_I("key pressed:%c/0x%x",cmd,cmd);
			switch(cmd){
				  case '<':
						index --;
					  if(index == 0xff)index = 1;
						lcd::sendAddrValue(0x710,index == 0 ? 0 : 1);
						lcd::sendAddrValue(0x714,index == 1 ? 0 : 1);
					  return ext::None;
						break;
					case '>':
						index ++;
					  if(index == 2)index = 0;
						lcd::sendAddrValue(0x710,index == 0 ? 0 : 1);
						lcd::sendAddrValue(0x714,index == 1 ? 0 : 1);
					  return ext::None;
						break;
					case ext::CMD_Input:
						Setting::setIsMale(index == 0 ? true : false);
						lcd::jumpToPage(9);
					  return ext::None;
						break;
					case ext::CMD_FanHui:
							lcd::jumpToPage(4);
						break;
					default:
						lcd::sendKeycode(cmd);
					  return cmd;
			}
			return cmd;
	}
private:
	char index;
};
#endif