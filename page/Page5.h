#ifndef ZLG_PAGE5_HH
#define ZLG_PAGE5_HH
#include "./Page.h"
#include "../api/lcd.h"
#include "../setting/Setting.h"
#include "../api/Tool.h"
#include "./InputPage.h"
#define INPUT_NUMB_MAX 100000
class Page5 : public InputPage
{
public:
	#define MAX_INPUT_LEN 9
	  Page5(){
			  
		}
		virtual void enter(){
			editing = false;
			pulseBaseAddr = 0x2301;
			measureBassAddr = 0x4301;
			focusBoxAddr = 0x508;
			inputAddr[3] = 0x520;
			inputAddr[2] = 0x550;
			inputAddr[1] = 0x580;
			inputAddr[0] = 0x5B0;
			inputlen[0] = 4;
			inputlen[1] = 4;
			inputlen[2] = 4;
			inputlen[3] = 4;
			triIconAddr = 0x5E0;
			mainAxixDegreeAddr = 0x510;
			displayRotateSpeed();
			InputPage::enter();
		}
		void displayRotateSpeed(){
			 //»ú´²×ªËÙ
			short code[20];
			u32 r = Setting::getRoundPerMin();
			int i = tool::convertFixed(r,0,code,20,false);
			i--;
			code[i++] = 0x8f6c;
			code[i++] = 0x002f;
			code[i++] = 0x5206;
			code[i++] = 0xffff;
			lcd::displayUnicode(0x500,code,i);
		}
		virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
		{
				LOG_I("key pressed:%c/0x%x",cmd,cmd);
			  switch(cmd){
						case ext::Numb_0:
						case ext::Numb_1:
						case ext::Numb_2:
						case ext::Numb_3:
						case ext::Numb_4:
						case ext::Numb_5:
						case ext::Numb_6:
						case ext::Numb_7:
						case ext::Numb_8:
						case ext::Numb_9:
						case ext::Numb_Dot:
						case ext::CMD_Backspace:
						case ext::CMD_Measure:
						case ext::CMD_RepeatMeas:
						case ext::CMD_Input:
							InputPage::onKeyPressed(cmd);
							break;
						case ext::CMD_Up:
							InputPage::onKeyPressed(cmd);
							
							break;
						case ext::CMD_Dn:
							InputPage::onKeyPressed(cmd);
							break;
						case ext::CMD_FanHui:
							lcd::jumpToPage(3);
							break;
						default:
							lcd::sendKeycode(cmd);
						  return cmd;
							break;
				}
				return ext::None;
		}
};
#endif