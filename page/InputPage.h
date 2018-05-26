#ifndef ZLG_INPUTPAGE_HH
#define ZLG_INPUTPAGE_HH
#include "./Page.h"
#include "../api/lcd.h"
#include "../setting/Setting.h"
#include "../api/Tool.h"
#define INPUT_NUMB_MAX 99999
class InputPage : public Page
{
public:
	#define MAX_INPUT_LEN 14
	  InputPage():selection(3){
			  
		}
		virtual void enter(){
			editing = false;
			testCount = 0;
			rotateCount = 0;
			selection = 8;
			selindex = 3;
			for(int i = 0; i < ARRAY_SIZE(input); i ++){
				/*input[i][0] = '0';
				input[i][1] = '.';
				input[i][2] = '0';
				input[i][3] = '0';
				input[i][4] = 0xffff;*/
				dotNumbToUnicode(i);
				//displayMeasured(i);
			}
			lcd::sendAddrValue(focusBoxAddr,0);
			displayMainAxisDegree();
			lcd::sendAddrValue(triIconAddr,selection);
		}
		void displayAll(){
			for(int i = 0; i < ARRAY_SIZE(input); i ++){
				//lcd::displayUnicode(inputPulseAddr[i],input[i],inputlen[i] + 1);
				displayMeasured(i);
				delay_ms(20);
				displayFixedDigree(i);
				delay_ms(20);
				displayFixedPulse(i);
				delay_ms(20);
			}
			displayMainAxisDegree();
		}
		//距离对应的角度
		void displayFixedDigree(int index){
			  short code[20];
			  int pulseToFix = calcDegreePulse(inputNumb[index]);
				int i = tool::convertPulseToAngle(pulseToFix,1024,code,ARRAY_SIZE(code));
				lcd::displayUnicode(inputDegreeAddr[index],code,i);
		}
		//距离对应的脉冲个数
		void displayFixedPulse(int index){
			  short code[20];
			  char offset[] = {31,27,23,19,15,11,7,3,0,0,0,0,0};
				int pulseToFix = calcDegreePulse(inputNumb[index]);
			 	int len = tool::convertFixed(pulseToFix,0,code,20,false);
				len --;
				code[len++] = 0x8109;
				code[len++] = 0x51b2;
				code[len++] = 0xffff;
			  lcd::sendAddrValue(pulseBaseAddr - index * 0x100,342 + offset[len - 4],185 - 16 * index);
				//lcd::sendAddrValue(0xA301,343 + offset[len - 4],184 - 16 * index);
				lcd::displayUnicode(inputPulseAddr[index] + 0x20,code,len);
		}
		
		void displayMainAxisDegree(){
			//主轴角度
			short code[20];
			u32 c = Setting::getMainAxisAngleInPulse();
			int i = tool::convertPulseToAngle(c,1024,code,ARRAY_SIZE(code));
			lcd::displayUnicode(mainAxixDegreeAddr,code,i);
		}
		//测量距离
		void displayMeasured(int index){
			char offset[] = {28,24,20,16,12,8,4,0,0,0,0,0,0,0,0,0};
			lcd::sendAddrValue(measureBassAddr - index * 0x100,134 + offset[inputlen[index]],185 - 16 * index);
			lcd::displayUnicode(inputPulseAddr[index],input[index],inputlen[index] + 1);
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
							if(!editing)break;
							if(MAX_INPUT_LEN > inputlen[selindex]){
								if(inputlen[selindex] == 0 && ext::Numb_Dot == cmd){
									 input[selindex][0] = '0';
								   inputlen[selindex] = 1;
								}
								if(inputlen[selindex] == 1 && input[selindex][0] == '0' && ext::Numb_Dot != cmd){
								   inputlen[selindex] = 0;
								}
								if(inputlen[selindex] >= 1){
									if(input[selindex][inputlen[selindex] - 1] == '.' && ext::Numb_Dot == cmd)break;
									if(inputlen[selindex] >= 2){
										if(input[selindex][inputlen[selindex] - 2] == '.' && ext::Numb_Dot == cmd)break;
										if(inputlen[selindex] >= 3){
											if(input[selindex][inputlen[selindex] - 3] == '.')break;
										}
									}
								}
								if(cmd != '.'){
									unsigned long temp = inputNumb[selindex] * 10 + ((short)cmd - '0');
									if(temp > INPUT_NUMB_MAX)break;
									if(temp == 0 && inputlen[selindex] != 2)break;
									inputNumb[selindex] = temp;
								}
								input[selindex][inputlen[selindex]] = (short)cmd;
								inputlen[selindex]++;
								input[selindex][inputlen[selindex]] = 0xFFFF;
								//lcd::displayUnicode(inputPulseAddr[selindex],input[selindex],inputlen[selindex] + 1);
								displayMeasured(selindex);
						  }
							break;
						case ext::CMD_Backspace:
							if(!editing)break;
							if(inputlen[selindex] > 0){
								inputlen[selindex] --;
								if(inputlen[selindex] != 0){
									input[selindex][inputlen[selindex]] = 0xFFFF;
									inputNumb[selindex] = inputNumb[selindex] / 10;
									//lcd::displayUnicode(inputPulseAddr[selindex],input[selindex],inputlen[selindex] + 1);
									displayMeasured(selindex);
								}
								else{
									input[selindex][0] = 0xFFFF;
									inputNumb[selindex] = 0;
									//lcd::displayUnicode(inputPulseAddr[selindex],input[selindex],inputlen[selindex] + 1);
									displayMeasured(selindex);
								}
							}
							break;
						case ext::CMD_Input:
							  editing = false;
						    Setting::setMeasureFixPulse(inputNumb[selindex],calcDegreePulse(inputNumb[selindex]));
							  lcd::jumpToPage(6);
							 return ext::None;
						  break;
					  case ext::CMD_Measure:
							editing = true;
							lcd::sendAddrValue(focusBoxAddr,selection);
						  //lcd::displayUnicode(inputPulseAddr[selindex],input[selindex],inputlen[selindex] + 1);
						  displayMeasured(selindex);
							break; 
						case ext::CMD_RepeatMeas:
							editing = true;
						  inputlen[selindex]= 0;
							inputNumb[selindex] = 0;
						  input[selindex][0] = 0xFFFF;
						  lcd::sendAddrValue(focusBoxAddr,selection);
						  //lcd::displayUnicode(inputPulseAddr[selindex],input[selindex],inputlen[selindex] + 1);
						  displayMeasured(selindex);
							break;
						case ext::CMD_Up:
							if(editing){
								checkAndFillInput(selindex);
								displayFixedDigree(selindex);
								displayFixedPulse(selindex);
								editing = false;
							}
							selection = selection << 1;
						  selindex++;
						  if(selection == 0x10){selection = 1;selindex = 0;}
						  lcd::sendAddrValue(triIconAddr,selection);
							lcd::sendAddrValue(focusBoxAddr,0);
							break;
						case ext::CMD_Dn:
							if(editing){
								checkAndFillInput(selindex);
								displayFixedDigree(selindex);
								displayFixedPulse(selindex);
								editing = false;
							}
							selection = selection>> 1;
						  selindex--;
						  if(selection == 0){selection = 8;selindex = 3;}
						  lcd::sendAddrValue(triIconAddr,selection);
							lcd::sendAddrValue(focusBoxAddr,0);
							break;
						default:
							lcd::sendKeycode(cmd);
						  return cmd;
							break;
				}
				return ext::None;
		}
		void checkAndFillInput(int index){
			 if(inputlen[selindex] == 0){
				 input[selindex][inputlen[selindex]] = '0';
				 inputlen[selindex] = 1;
			 }
			 if(inputlen[selindex] == 1){
				 if(input[selindex][0] != '.'){
						input[selindex][1] = '.';
						inputlen[selindex] = 2;
					}
			 }
			 if(inputlen[selindex] == 2){
				 if(input[selindex][0] == '.'){
						input[selindex][2] = '0';
						inputlen[selindex] = 3;
					 inputNumb[selindex] = inputNumb[selindex] * 10;
				 }
				 else if(input[selindex][1] == '.'){
						input[selindex][2] = '0';
						inputlen[selindex] = 3;
					 inputNumb[selindex] = inputNumb[selindex] * 10;
				 }
				 else{
					  input[selindex][2] = '.';
						inputlen[selindex] = 3;
				 }
			 }
			 if(inputlen[selindex] >= 3){
				  if(input[selindex][inputlen[selindex] - 3] != '.' &&
						 input[selindex][inputlen[selindex] - 2] != '.' &&
					   input[selindex][inputlen[selindex] - 1] != '.'){
						input[selindex][inputlen[selindex]] = '.';
						inputlen[selindex] ++;
					}
					if(input[selindex][inputlen[selindex] - 1] == '.'){
						input[selindex][inputlen[selindex]] = '0';
						inputlen[selindex] ++;
						input[selindex][inputlen[selindex]] = 0xFFFF;
						inputNumb[selindex] = inputNumb[selindex] * 10;
					}
					if(input[selindex][inputlen[selindex] - 2] == '.'){
						input[selindex][inputlen[selindex]] = '0';
						inputlen[selindex] ++;
						input[selindex][inputlen[selindex]] = 0xFFFF;
						inputNumb[selindex] = inputNumb[selindex] * 10;
					}
				}
				dotNumbToUnicode(index);
				displayMeasured(selindex);
		}
		void handleEncoder(char dir){
			 if(dir)testCount += 1;
			 else testCount -= 1;
			 for(int i = 0; i < 4; i ++){
				 Setting::setInput(i,testCount);
				 inputNumb[i] = testCount;
				 dotNumbToUnicode(i);
			 }
			 displayAll();
		}
		void onTimer(){
			//主轴角度
			displayMainAxisDegree();		
		}
		void dotNumbToUnicode(int index){
			  u32 times = 1000000000;
			  int len = 0;
			  unsigned int numb = inputNumb[index];
			  while(times != 100){
					 if(numb / times != 0){
						  break;
					 }
					 times /= 10;
				}
				while(times != 100){
					input[index][len] = numb / times + '0';
					len ++;
					numb %= times;
					times /= 10;
				}
				input[index][len] = numb / 100 + '0';
				len ++;
				numb %= 100;
				
				input[index][len] = '.';
				len ++;
				
				input[index][len] = numb / 10 + '0';
				len ++;
				input[index][len] = numb % 10 + '0';
				len ++;
				input[index][len] = 0xffff;
				inputlen[index] = len;
		}
		u32 calcDegreePulse(u32 distMM){
			 u32 roundPulse = 1024;
			 u32 toothCount = Setting::getToothCount();
			 u32 distPerTooth = 635;
			 u32 pulseDegree = Setting::getMainAxisAngleInPulse();
			 /*u32 degreeDistMM = toothCount * distPerTooth * pulseDegree / roundPulse;
			 u32 totalMM = distMM + degreeDistMM;
			 u32 fixedMM = totalMM %(toothCount*distPerTooth);
			 u32 fixedForToothMM = fixedMM % distPerTooth;*/
			 float distDeg = (float)distMM /(float)(distPerTooth * toothCount);
			 u32   distPulse = roundPulse * distMM / (distPerTooth * toothCount);
			 u32   totalPulse = pulseDegree + distPulse;
			 u32   fixedForToothPulse = totalPulse % (roundPulse / toothCount);
			 return fixedForToothPulse;
		}
		protected:
			int  testCount;
		  int  rotateCount;
			bool editing;
			char selection;
		  char selindex;
		  int   inputlen[4];
		  short input[4][MAX_INPUT_LEN + 2];
		  unsigned long inputNumb[4];
		  short inputPulseAddr[4];
		  short inputDegreeAddr[4];
		  short triIconAddr;
		  short focusBoxAddr;
		  short mainAxixDegreeAddr;
		  short pulseBaseAddr;
		  short measureBassAddr;
};
#endif