#ifndef ZLG_PAGE10_HH
#define ZLG_PAGE10_HH
#include "./Page.h"
#include "../api/lcd.h"
#include "../setting/Setting.h"
#include "InputPage.h"
class Page10 : public InputPage
{
public:
	virtual void enter(){
		  editing = false;
		  pulseBaseAddr = 0x3301;
		  measureBassAddr = 0x8301;
		  focusBoxAddr = 0x10E0;
			inputPulseAddr[3] = 0x1010;
			inputPulseAddr[2] = 0x1040;
			inputPulseAddr[1] = 0x1070;
			inputPulseAddr[0] = 0x10a0;
		  inputDegreeAddr[3] = 0x1020;
		  inputDegreeAddr[2] = 0x1050;
		  inputDegreeAddr[1] = 0x1080;
		  inputDegreeAddr[0] = 0x10b0;
			inputlen[0] = 4;
			inputlen[1] = 4;
			inputlen[2] = 4;
			inputlen[3] = 4;
		  inputNumb[0] = Setting::getConfigInput(0);
		  inputNumb[1] = Setting::getConfigInput(1);
		  inputNumb[2] = Setting::getConfigInput(2);
		  inputNumb[3] = Setting::getConfigInput(3);
			selection = 0;
			selindex = 0;
		  triIconAddr = 0x10D0;
		  mainAxixDegreeAddr = 0x1000;
			InputPage::enter();
			displayAll();
			u32 angle = Setting::getMainAxisAngleInPulse();
			display(angle);
	}
	/*void displayItem(int item){
		int i = 0;
		short code[20];
		i = tool::convertFixed(dist[item],100,code,20);
		i--;
		code[i++] = 'm';
		code[i++] = 'm';
		code[i++] = 0xffff;
		lcd::displayUnicode(addr[item],code,i);
		
		i = tool::convertPulseToAngle(dist[item],PULSE_PER_AXIS_ROUND,code,ARRAY_SIZE(code));
		lcd::displayUnicode(addr[item] + 0x10,code,i);
		
		i = tool::convertFixed(dist[item],0,code,ARRAY_SIZE(code));
		i--;
		code[i++] = 0x8109;
		code[i++] = 0x51b2;
		code[i++] = 0xffff;
		lcd::displayUnicode(addr[item] + 0x20,code,i);
	}*/
	
	void display(u32 angle){
		int i;
		short code[20];
		//Ö÷Öá½Ç¶È
		i = tool::convertPulseToAngle(angle,PULSE_PER_AXIS_ROUND,code,ARRAY_SIZE(code));
		lcd::displayUnicode(0x1000,code,i);
	}
	virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
	{
		LOG_I("key pressed:%c",cmd);
			switch(cmd){
				  case ext::CMD_FanHui:
							lcd::jumpToPage(9);
							return ext::None;
					case ext::CMD_Input:
							editing = false;
						  Setting::setMeasureFixPulse(inputNumb[selindex],calcDegreePulse(inputNumb[selindex]));
					    Setting::setConfigInput(0,inputNumb[0]);
					    Setting::setConfigInput(1,inputNumb[1]);
					    Setting::setConfigInput(2,inputNumb[2]);
					    Setting::setConfigInput(3,inputNumb[3]);
							Setting::saveToFlash();
							lcd::jumpToPage(3);
							return ext::None;
					default:
						InputPage::onKeyPressed(cmd);
						return cmd;
						break;
			}
			return cmd;
	}
};
#endif