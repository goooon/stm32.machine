#ifndef ZLG_PAGE7_HH
#define ZLG_PAGE7_HH
#include "./Page.h"
#include "../api/lcd.h"
#include "../setting/Setting.h"

//设置公母螺扣钻杆类型
class Page7 : public Page
{
public:
	virtual void enter(){
		index = 0;
		lcd::sendAddrValue(0x710,index == 0 ? 0 : 1);
		lcd::sendAddrValue(0x714,index == 1 ? 0 : 1);
		displayInfor(false);
	}
	virtual void leave(){
		index = 0;
		lcd::sendAddrValue(0x710,index == 0 ? 0 : 1);
		lcd::sendAddrValue(0x714,index == 1 ? 0 : 1);
	}
	void displayInfor(bool set){
		short code[20];
		int i;
		if(set){
			code[0] = 0x8bf7;
			code[1] = 0x8bbe;
			code[2] = 0x7f6e;
			code[3] = 0x57fa;
			code[4] = 0x51c6;
			code[5] = 0x53c2;
			code[6] = 0x6570;
			code[7] = 0xffff;
			i = 8;
		}
		else{
			code[0] = 0xffff;
			i = 1;
		}
		lcd::displayUnicode(0x710,code,i);
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
						Setting::setConfigToothType(index == 0 ? Male : Female);
						lcd::jumpToPage(9);
					  return ext::None;
						break;
					case ext::CMD_FanHui:
							if(!Setting::needPresetParameters()){
								lcd::jumpToPage(4);
							}
							else{
								displayInfor(true);
							}
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