#ifndef ZLG_INPUTPAGE_HH
#define ZLG_INPUTPAGE_HH
#include "./Page.h"
#include "../api/lcd.h"
#include "../setting/Setting.h"
#include "../api/Tool.h"
#define INPUT_NUMB_MAX 99999
#define triIconBitSelectionMask (((u32)1) << (3 - currSelectedIndex))
class InputPage : public Page
{
public:
	#define MAX_INPUT_LEN 14
	  InputPage(){
			  
		}
		virtual void enter(){
			testCount = 0;
			rotateCount = 0;
			currSelectedIndex = 0;
			SelectedIndexMax = 0;
			inputNumb[0] = 0;
		  inputNumb[1] = 0;
		  inputNumb[2] = 0;
		  inputNumb[3] = 0;
			for(int i = 0; i < ARRAY_SIZE(input); i ++){
				//dotNumbToUnicode(i);
				inputlen[i] = 0;
			}
			displayAllValue();
			lcd::sendAddrValue(triIconAddr,triIconBitSelectionMask);
			displayEditorFocus(0);
		}
		void updateRow(u32 i){
			dotNumbToUnicode(i,inputNumb[i]);
		}
		void displayRow(u32 i){
			displayMeasured(i);
			displayFixedDigree(i,inputNumb[i]);
			displayFixedPulse(i,inputNumb[i]);
		}
		void displayTable(){
			for(int i = 0; i < ARRAY_SIZE(input); i ++){
				displayRow(i);
			}
		}
		void displayAllValue(){
			displayMainAxisDegree();
			displayTable();
		}
		//测量距离
		void displayMeasured(int index){
			int uiRowAddrIndex = 3 - index;
			char offset[] = {28,24,20,16,12,8,4,0,0,0,0,0,0,0,0,0};
			lcd::sendAddrValue(measureBassAddr - uiRowAddrIndex * 0x100,134 + offset[inputlen[index]],185 - 16 * uiRowAddrIndex);
			lcd::displayUnicode(inputPulseAddr[uiRowAddrIndex],input[index],inputlen[index] + 1);
		}
		//距离对应的角度
		void displayFixedDigree(int index,u32 numb){
			  int uiRowAddrIndex = 3 - index;
			  short code[20];
			  int len = 0;
			  if(inputlen[index] != 0){
					int pulseToFix = calcDegreePulse(numb);
					len = tool::convertPulseToAngle(pulseToFix,1024,code,ARRAY_SIZE(code));
				}
				else{
					code[0] = 0xFFFF;
					len = 1;
				}
				lcd::displayUnicode(inputDegreeAddr[uiRowAddrIndex],code,len);
		}
		//距离对应的脉冲个数
		void displayFixedPulse(int index,u32 numb){
			  int uiRowAddrIndex = 3 - index;
				short code[20];
				char offset[] = {31,27,23,19,15,11,7,3,0,0,0,0,0};
				int len = 0;
				if(inputlen[index] != 0){
					int pulseToFix = calcDegreePulse(numb);
					len = tool::convertFixed(pulseToFix,0,code,20,false);
					len --;
					code[len++] = 0x8109;
					code[len++] = 0x51b2;
					code[len++] = 0xffff;
				}
				else{
					code[0] = 0xFFFF;
					len = 1;
				}
				lcd::sendAddrValue(pulseBaseAddr - uiRowAddrIndex * 0x100,342 + offset[len - 4],185 - 16 * uiRowAddrIndex);
				lcd::displayUnicode(inputPulseAddr[uiRowAddrIndex] + 0x20,code,len);
		}
		
		void displayMainAxisDegree(){
			//主轴角度
			short code[20];
			u32 c = Setting::getMainAxisAngleInPulse();
			int i = tool::convertPulseToAngle(c,1024,code,ARRAY_SIZE(code));
			lcd::displayUnicode(mainAxixDegreeAddr,code,i);
		}
		//清除内容
		void clearEditorValue(u32 index){
			inputlen[index]= 0;
			inputNumb[index] = 0;
			input[index][0] = 0xFFFF;
			displayMeasured(index);
			displayFixedDigree(index,inputNumb[index]);
			displayFixedPulse(index,inputNumb[index]);
		}
		//显示编辑输入框
		void displayEditorFocus(u32 index){
			editing = true;
			//lcd::displayUnicode(inputPulseAddr[currSelectedIndex],input[currSelectedIndex],inputlen[currSelectedIndex] + 1);
			//displayMeasured(currSelectedIndex);
			lcd::sendAddrValue(focusBoxAddr,(((u32)1) << (3 - index)));
		}
		void hideAllEditorFocus(){
			editing = false;
			lcd::sendAddrValue(focusBoxAddr,0);
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
							if(MAX_INPUT_LEN > inputlen[currSelectedIndex]){
								if(inputlen[currSelectedIndex] == 0 && ext::Numb_Dot == cmd){
									 input[currSelectedIndex][0] = '0';
								   inputlen[currSelectedIndex] = 1;
								}
								if(inputlen[currSelectedIndex] == 1 && input[currSelectedIndex][0] == '0' && ext::Numb_Dot != cmd){
								   inputlen[currSelectedIndex] = 0;
								}
								if(inputlen[currSelectedIndex] >= 1){
									if(input[currSelectedIndex][inputlen[currSelectedIndex] - 1] == '.' && ext::Numb_Dot == cmd)break;
									if(inputlen[currSelectedIndex] >= 2){
										if(input[currSelectedIndex][inputlen[currSelectedIndex] - 2] == '.' && ext::Numb_Dot == cmd)break;
										if(inputlen[currSelectedIndex] >= 3){
											if(input[currSelectedIndex][inputlen[currSelectedIndex] - 3] == '.')break;
										}
									}
								}
								if(cmd != '.'){
									unsigned long temp = inputNumb[currSelectedIndex] * 10 + ((short)cmd - '0');
									if(temp > INPUT_NUMB_MAX)break;
									if(temp == 0 && inputlen[currSelectedIndex] != 2)break;
									inputNumb[currSelectedIndex] = temp;
								}
								input[currSelectedIndex][inputlen[currSelectedIndex]] = (short)cmd;
								inputlen[currSelectedIndex]++;
								input[currSelectedIndex][inputlen[currSelectedIndex]] = 0xFFFF;
								//lcd::displayUnicode(inputPulseAddr[currSelectedIndex],input[currSelectedIndex],inputlen[currSelectedIndex] + 1);
								displayMeasured(currSelectedIndex);
								checkAndFillInputWhenEditing(currSelectedIndex);
						  }
							break;
						case ext::CMD_Backspace:
							editing = true;
							inputNumb[currSelectedIndex] = 0;
						  input[currSelectedIndex][0] = 0xFFFF;
						  inputlen[currSelectedIndex] = 0;
						  displayMeasured(currSelectedIndex);
						  checkAndFillInputWhenEditing(currSelectedIndex);
						  displayEditorFocus(currSelectedIndex);
						/*
							if(inputlen[currSelectedIndex] > 0){
								inputlen[currSelectedIndex] --;
								if(inputlen[currSelectedIndex] != 0){
									input[currSelectedIndex][inputlen[currSelectedIndex]] = 0xFFFF;
									inputNumb[currSelectedIndex] = inputNumb[currSelectedIndex] / 10;									//lcd::displayUnicode(inputPulseAddr[currSelectedIndex],input[currSelectedIndex],inputlen[currSelectedIndex] + 1);
									displayMeasured(currSelectedIndex);
								}
								else{
									input[currSelectedIndex][0] = 0xFFFF;
									inputNumb[currSelectedIndex] = 0;
									displayMeasured(currSelectedIndex);
								}
							}*/
							break;
						case ext::CMD_Input:
							  editing = false;
						    Setting::setMeasureFixPulse(inputNumb[currSelectedIndex],calcDegreePulse(inputNumb[currSelectedIndex]));
							  lcd::jumpToPage(6);
							 return ext::None;
						  break;
					  case ext::CMD_Measure:
							editing = true;
							lcd::sendAddrValue(focusBoxAddr,triIconBitSelectionMask);
						  //lcd::displayUnicode(inputPulseAddr[currSelectedIndex],input[currSelectedIndex],inputlen[currSelectedIndex] + 1);
						  displayMeasured(currSelectedIndex);
							break; 
						case ext::CMD_RepeatMeas:
							if(inputlen[currSelectedIndex] == 0)return;
							if(SelectedIndexMax >= 3)return;
						  else SelectedIndexMax ++;
						  checkAndFillInput(currSelectedIndex);
						  displayFixedDigree(currSelectedIndex,inputNumb[currSelectedIndex]);
							displayFixedPulse(currSelectedIndex,inputNumb[currSelectedIndex]);
						
						  currSelectedIndex = SelectedIndexMax;
						  lcd::sendAddrValue(triIconAddr,triIconBitSelectionMask);
						  displayEditorFocus(currSelectedIndex);
							break;
						case ext::CMD_Up:
							if(editing){
						    hideAllEditorFocus();
								checkAndFillInput(currSelectedIndex);
								displayFixedDigree(currSelectedIndex,inputNumb[currSelectedIndex]);
								displayFixedPulse(currSelectedIndex,inputNumb[currSelectedIndex]);
								editing = false;
							}
						  if(currSelectedIndex == 0){currSelectedIndex = SelectedIndexMax;}
							else currSelectedIndex --;
						  lcd::sendAddrValue(triIconAddr,triIconBitSelectionMask);
							//hideAllEditorFocus();
							//checkAndFillInput(currSelectedIndex);
							//displayFixedDigree(currSelectedIndex,inputNumb[currSelectedIndex]);
							//displayFixedPulse(currSelectedIndex,inputNumb[currSelectedIndex]);
							break;
						case ext::CMD_Dn:
							if(editing){
								hideAllEditorFocus();
								checkAndFillInput(currSelectedIndex);
								displayFixedDigree(currSelectedIndex,inputNumb[currSelectedIndex]);
								displayFixedPulse(currSelectedIndex,inputNumb[currSelectedIndex]);
								editing = false;
							}
						  if(inputlen[currSelectedIndex] == 0)currSelectedIndex --;
						  if(currSelectedIndex >= SelectedIndexMax)currSelectedIndex = 0;
							else currSelectedIndex++;
						  lcd::sendAddrValue(triIconAddr,triIconBitSelectionMask);
							//hideAllEditorFocus();
						  //checkAndFillInput(currSelectedIndex);
							//displayFixedDigree(currSelectedIndex,inputNumb[currSelectedIndex]);
							//displayFixedPulse(currSelectedIndex,inputNumb[currSelectedIndex]);
							break;
						default:
							lcd::sendKeycode(cmd);
						  return cmd;
							break;
				}
				return ext::None;
		}
		void checkAndFillInputWhenEditing(int index){
			 u32 inputLen = inputlen[index];
			 u32 inputNum = inputNumb[currSelectedIndex];
			 short inputMax[MAX_INPUT_LEN + 2];
			 for(int i = 0; i < MAX_INPUT_LEN + 2; ++i){
				  inputMax[i] = input[currSelectedIndex][i];
			 }
			 if( inputLen == 0){
				 inputMax[inputLen] = '0';
				 inputLen = 1;
			 }
			 if(inputLen == 1){
				 if(inputMax[0] != '.'){
						inputMax[1] = '.';
						inputLen = 2;
					}
			 }
			 if(inputLen == 2){
				 if(inputMax[0] == '.'){
						inputMax[2] = '0';
						inputLen = 3;
					  inputNum = inputNum * 10;
				 }
				 else if(inputMax[1] == '.'){
						inputMax[2] = '0';
						inputLen = 3;
					  inputNum = inputNum * 10;
				 }
				 else{
					  inputMax[2] = '.';
						inputLen = 3;
				 }
			 }
			 if(inputLen >= 3){
				  if(inputMax[inputLen - 3] != '.' &&
						 inputMax[inputLen - 2] != '.' &&
					   inputMax[inputLen - 1] != '.'){
						 inputMax[inputLen] = '.';
						 inputLen ++;
					}
					if(inputMax[inputLen - 1] == '.'){
						inputMax[inputLen] = '0';
						inputLen ++;
						inputMax[inputLen] = 0xFFFF;
						inputNum = inputNum * 10;
					}
					if(inputMax[inputLen - 2] == '.'){
						inputMax[inputLen] = '0';
						inputLen ++;
						inputMax[inputLen] = 0xFFFF;
						inputNum = inputNum * 10;
					}
				}
				displayFixedDigree(index,inputNum);
				displayFixedPulse(index,inputNum);
		}
		void checkAndFillInput(int index){
			 if(inputlen[currSelectedIndex] == 0){
				 input[currSelectedIndex][inputlen[currSelectedIndex]] = '0';
				 inputlen[currSelectedIndex] = 1;
			 }
			 if(inputlen[currSelectedIndex] == 1){
				 if(input[currSelectedIndex][0] != '.'){
						input[currSelectedIndex][1] = '.';
						inputlen[currSelectedIndex] = 2;
					}
			 }
			 if(inputlen[currSelectedIndex] == 2){
				 if(input[currSelectedIndex][0] == '.'){
						input[currSelectedIndex][2] = '0';
						inputlen[currSelectedIndex] = 3;
					 inputNumb[currSelectedIndex] = inputNumb[currSelectedIndex] * 10;
				 }
				 else if(input[currSelectedIndex][1] == '.'){
						input[currSelectedIndex][2] = '0';
						inputlen[currSelectedIndex] = 3;
					 inputNumb[currSelectedIndex] = inputNumb[currSelectedIndex] * 10;
				 }
				 else{
					  input[currSelectedIndex][2] = '.';
						inputlen[currSelectedIndex] = 3;
				 }
			 }
			 if(inputlen[currSelectedIndex] >= 3){
				  if(input[currSelectedIndex][inputlen[currSelectedIndex] - 3] != '.' &&
						 input[currSelectedIndex][inputlen[currSelectedIndex] - 2] != '.' &&
					   input[currSelectedIndex][inputlen[currSelectedIndex] - 1] != '.'){
						input[currSelectedIndex][inputlen[currSelectedIndex]] = '.';
						inputlen[currSelectedIndex] ++;
					}
					if(input[currSelectedIndex][inputlen[currSelectedIndex] - 1] == '.'){
						input[currSelectedIndex][inputlen[currSelectedIndex]] = '0';
						inputlen[currSelectedIndex] ++;
						input[currSelectedIndex][inputlen[currSelectedIndex]] = 0xFFFF;
						inputNumb[currSelectedIndex] = inputNumb[currSelectedIndex] * 10;
					}
					if(input[currSelectedIndex][inputlen[currSelectedIndex] - 2] == '.'){
						input[currSelectedIndex][inputlen[currSelectedIndex]] = '0';
						inputlen[currSelectedIndex] ++;
						input[currSelectedIndex][inputlen[currSelectedIndex]] = 0xFFFF;
						inputNumb[currSelectedIndex] = inputNumb[currSelectedIndex] * 10;
					}
				}
				dotNumbToUnicode(index,inputNumb[index]);
				displayMeasured(currSelectedIndex);
		}
		void handleEncoder(char dir){
			 /*if(dir)testCount += 1;
			 else testCount -= 1;
			 for(int i = 0; i < 4; i ++){
				 Setting::setInput(i,testCount);
				 inputNumb[i] = testCount;
				 dotNumbToUnicode(i);
			 }
			 displayAll();*/
		}
		void onTimer(){
			//主轴角度
			displayMainAxisDegree();		
		}
		void dotNumbToUnicode(int index,u32 numbInputted){
			  u32 times = 1000000000;
			  int len = 0;
			  unsigned int numb = numbInputted;
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
		  char currSelectedIndex;
		  char SelectedIndexMax;
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