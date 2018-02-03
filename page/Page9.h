#ifndef ZLG_PAGE9_HH
#define ZLG_PAGE9_HH
#include "./Page.h"
#include "../api/lcd.h"
#include "../setting/Setting.h"
class Page9 : public Page
{
public:
	public:
	virtual void enter(){
		index = 0;
		lcd::sendAddrValue(0x910,index == 0 ? 0 : 1);
		lcd::sendAddrValue(0x920,index == 1 ? 0 : 1);
		lcd::sendAddrValue(0x930,index == 2 ? 0 : 1);
	}
	virtual void leave(){
		index = 0;
		lcd::sendAddrValue(0x910,index == 0 ? 0 : 1);
		lcd::sendAddrValue(0x920,index == 1 ? 0 : 1);
		lcd::sendAddrValue(0x930,index == 2 ? 0 : 1);
	}
	virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
	{
			LOG_I("key pressed:%c/0x%x",cmd,cmd);
			switch(cmd){
				  case '<':
						index --;
					  if(index == 0xFF)index = 2;
						lcd::sendAddrValue(0x910,index == 0 ? 0 : 1);
						lcd::sendAddrValue(0x920,index == 1 ? 0 : 1);
						lcd::sendAddrValue(0x930,index == 2 ? 0 : 1);
					  return ext::None;
						break;
					case '>':
						index ++;
					  if(index == 3)index = 0;
						lcd::sendAddrValue(0x910,index == 0 ? 0 : 1);
						lcd::sendAddrValue(0x920,index == 1 ? 0 : 1);
						lcd::sendAddrValue(0x930,index == 2 ? 0 : 1);
					  return ext::None;
						break;
					case ext::CMD_Input:
						Setting::setToothCount(index * 2 + 4);
						lcd::jumpToPage(10);
					  return ext::None;
						break;
					case ext::CMD_FanHui:
						lcd::jumpToPage(7);
						return ext::None;
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