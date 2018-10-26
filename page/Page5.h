#ifndef ZLG_PAGE5_HH
#define ZLG_PAGE5_HH
#include "./Page.h"
#include "../api/lcd.h"
#include "../setting/Setting.h"
#include "../api/Tool.h"
#include "./InputPage.h"
#define INPUT_NUMB_MAX 100000

//设置基准值界面
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
			inputPulseAddr[3] = 0x520;
			inputPulseAddr[2] = 0x550;
			inputPulseAddr[1] = 0x580;
			inputPulseAddr[0] = 0x5B0;
			inputDegreeAddr[3] = 0x530;
		  inputDegreeAddr[2] = 0x560;
		  inputDegreeAddr[1] = 0x590;
		  inputDegreeAddr[0] = 0x5C0;
			LOG_I("dist stored is:%d,%d,%d,%d",inputDistUM[3],inputDistUM[2],inputDistUM[1],inputDistUM[0]);
			triIconAddr = 0x5E0;
			mainAxixDegreeAddr = 0x510;
			displayRotateSpeed();
			InputPage::enter();
			displayAllValue();
		}
		void displayRotateSpeed(){
			 //机床转速
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
		void onTimer(){
			displayRotateSpeed();
			InputPage::onTimer();
		}
		virtual void displayFixedPulse(int index,u32 numb){//dist to zero
		    int uiRowAddrIndex = 3 - index;
			s32 defaultInputDist;
			s32 defaultMainAxisInPulse;
			u32 measureDist = numb;
			u32 measurePuls = currMainAxisInPulse[index];
			float combinedFixDist;
			float combinedFixPulse;
			short code[20];
			char offset[] = {31,27,23,19,15,11,7,3,0,0,0,0,0};
			int len = 0;
			if(inputCharsLen[index] != 0){
				
			    Setting::getDefautMeasureFixPulse(defaultInputDist,defaultMainAxisInPulse);
			    //Setting::getMeasureFixPulse(measureDist,measurePuls);

			    combinedFixPulse = Setting::calculateDelayedPulse(defaultMainAxisInPulse,defaultInputDist,measurePuls,measureDist);
			    combinedFixDist = Setting::pulseToDistUM(combinedFixPulse);


				float dist = combinedFixDist;//Setting::calcDistToZero(numb,currMainAxisInPulse[index]);
				if(dist > Setting::getDistUMCountPerTooth() / 2){
					dist -= Setting::getDistUMCountPerTooth();
				}
				len = tool::convertFixed(dist,Setting::getPrecision(),code,20,false);
				len --;
				code[len++] = 'm';
				code[len++] = 'm';
				code[len++] = 0xffff;
			}
			else{
				code[0] = 0xFFFF;
				len = 1;
			}
			lcd::sendAddrValue(pulseBaseAddr - uiRowAddrIndex * 0x100,342 + offset[len - 4],185 - 16 * uiRowAddrIndex);
			lcd::displayUnicode(inputPulseAddr[uiRowAddrIndex] + 0x20,code,len);
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
							InputPage::onKeyPressed(cmd);
						break;
						case ext::CMD_Enter:
							if(!haveValidInput()){
									return ext::None;
							}
							editing = false;
						  hideAllEditorFocus();
						  checkAndFillInput(currSelectedIndex);
						  break;
						case ext::CMD_Input:
							  if(!haveValidInput()){
									return ext::None;
								}
							  editing = false;
								checkAndFillInput(currSelectedIndex);
						    Setting::setMeasureFixPulse(inputDistUM[currSelectedIndex],currMainAxisInPulse[currSelectedIndex]);
							  lcd::jumpToPage(6);
							  return ext::None;
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