#ifndef ZLG_PAGE4_HH
#define ZLG_PAGE4_HH
#include "./Page.h"
#include "../setting/Setting.h"
#include "../api/lcd.h"
class Page4 : public Page
{
public:
	virtual void enter(){
		index = 0;
		lcd::sendAddrValue(0x410,index == 0 ? 0 : 1);
		lcd::sendAddrValue(0x414,index == 1 ? 0 : 1);
	}
	virtual void leave(){
		index = 0;
		lcd::sendAddrValue(0x410,index == 0 ? 0 : 1);
		lcd::sendAddrValue(0x414,index == 1 ? 0 : 1);
	}
	virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
	{
			LOG_I("key pressed:%c/0x%x",cmd,cmd);
			switch(cmd){
				  case '<':
						index --;
					  if(index == 0xff)index = 1;
						lcd::sendAddrValue(0x410,index == 0 ? 0 : 1);
						lcd::sendAddrValue(0x414,index == 1 ? 0 : 1);
						break;
					case '>':
						index ++;
					  if(index == 2)index = 0;
						lcd::sendAddrValue(0x410,index == 0 ? 0 : 1);
						lcd::sendAddrValue(0x414,index == 1 ? 0 : 1);
						break;
					case ext::CMD_Input:
						lcd::jumpToPage(index == 0 ?  7 : 8);
					  Setting::setConfigToothType( index == 0 ? Male : Female);
					  return ext::None;
						break;
					case ext::CMD_FanHui:
							lcd::jumpToPage(3);
							return ext::None;
			}
			return cmd;
	}
private:
		char index;
};
#endif