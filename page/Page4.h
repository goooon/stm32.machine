#ifndef ZLG_PAGE4_HH
#define ZLG_PAGE4_HH
#include "./Page.h"
#include "../setting/Setting.h"
#include "../api/lcd.h"
class Page4 : public Page
{
public:
    //进入当前页面4
	virtual void enter(){
		index = 0;
		lcd::sendAddrValue(0x410,index == 0 ? 0 : 1); //判断选择按钮之后进入页面，0代表左边按钮，进入基本脉冲界面，1代表右边按钮，进入进刀数据界面
		lcd::sendAddrValue(0x414,index == 1 ? 0 : 1);
	}
	//离开当前页面4
	virtual void leave(){
		index = 0;
		lcd::sendAddrValue(0x410,index == 0 ? 0 : 1);//记录返回页面，0代表左边按钮，1代表右边按钮
		lcd::sendAddrValue(0x414,index == 1 ? 0 : 1);
	}
	virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
	{
			LOG_I("key pressed:%c/0x%x",cmd,cmd);
			switch(cmd){
				  case '<':	  //代表按钮点击了CMD_Left
						index --;
					  if(index == 0xff)index = 1;
						lcd::sendAddrValue(0x410,index == 0 ? 0 : 1);
						lcd::sendAddrValue(0x414,index == 1 ? 0 : 1);
						break;
					case '>':	//代表按钮点击了CMD_Right
						index ++;
					  if(index == 2)index = 0;
						lcd::sendAddrValue(0x410,index == 0 ? 0 : 1);
						lcd::sendAddrValue(0x414,index == 1 ? 0 : 1);
						break;
					case ext::CMD_Input:   //代表按钮点击了CMD_Input
						lcd::jumpToPage(index == 0 ?  7 : 8); //跳转到对应界面
					  Setting::setConfigToothType( index == 0 ? Male : Female);	  //记录公母螺扣
					  return ext::None;
						break;
					case ext::CMD_FanHui: //点击返回按钮，回到页面3
							lcd::jumpToPage(3);
							return ext::None;
			}
			return cmd;
	}
private:
		char index;
};
#endif