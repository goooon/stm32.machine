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
			currSelectedIndex = 0;
		  triIconAddr = 0x10D0;
		  mainAxixDegreeAddr = 0x1000;
			InputPage::enter();
	}
	virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
	{
		  LOG_I("key pressed:%c",cmd);
			switch(cmd){
				  case ext::CMD_FanHui:
							lcd::jumpToPage(9);
							return ext::None;
					case ext::CMD_Input:
						  if(!haveValidInput()){
									return ext::None;
							}
							editing = false;
							checkAndFillInput(currSelectedIndex);
							Setting::setMeasureFixPulse(inputDistUM[currSelectedIndex],Setting::calcDegreePulse(inputDistUM[currSelectedIndex]));
					    Setting::setBaseConfigInput(0,inputDistUM[0],currMainAxisInPulse[0]);
					    Setting::setBaseConfigInput(1,inputDistUM[1],currMainAxisInPulse[1]);
					    Setting::setBaseConfigInput(2,inputDistUM[2],currMainAxisInPulse[2]);
					    Setting::setBaseConfigInput(3,inputDistUM[3],currMainAxisInPulse[3]);
					    Setting::setDefaultBaseConfigInputIndex(currSelectedIndex);
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