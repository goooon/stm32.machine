#ifndef ZLG_INPUTPAGE_HH
#define ZLG_INPUTPAGE_HH
#include "./Page.h"
#include "../api/lcd.h"
#include "../setting/Setting.h"
#include "../api/Tool.h"
#define INPUT_NUMB_MAX 999999
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
			for(int i = 0; i < ARRAY_SIZE(inputChars); i ++){
				inputCharsLen[i] = 0;
				currMainAxisInPulse[i] = 0;
				inputDistUM[i] = 0;
				clearEditorValue(i);
			}
			displayAllValue();
			displayEditorFocus(0);
			lcd::sendAddrValue(triIconAddr,triIconBitSelectionMask);
			onKeyPressed(ext::CMD_RepeatMeas);
		}
		void updateRow(u32 i){
			dotNumbToUnicode(i,inputDistUM[i]);
		}
		void displayRow(u32 i){
			displayMeasured(i);
			displayFixedDigree(i,inputDistUM[i]);
			displayFixedPulse(i,inputDistUM[i]);
		}
		void displayTable(){
			for(int i = 0; i < ARRAY_SIZE(inputChars); i ++){
				displayRow(i);
			}
		}
		void displayAllValue(){
			displayMainAxisDegree();
			displayTable();
		}
		//��������
		void displayMeasured(int index){
			int uiRowAddrIndex = 3 - index;
			char offset[] = {28,24,20,16,12,8,4,0,0,0,0,0,0,0,0,0};
			lcd::sendAddrValue(measureBassAddr - uiRowAddrIndex * 0x100,134 + offset[inputCharsLen[index]],185 - 16 * uiRowAddrIndex);
			lcd::displayUnicode(inputPulseAddr[uiRowAddrIndex],inputChars[index],inputCharsLen[index] + 1);
		}
		//�����Ӧ�ĽǶ�
		void displayFixedDigree(int index,u32 numb){
			  int uiRowAddrIndex = 3 - index;
			  short code[20];
			  int len = 0;
			  if(inputCharsLen[index] != 0){
					//int pulseToFix = calcDegreePulse(numb);
					//len = tool::convertPulseToAngle(pulseToFix,Setting::getPulseCountPerCircle(),code,ARRAY_SIZE(code));
					len = tool::convertPulseToAngle(currMainAxisInPulse[index],Setting::getPulseCountPerCircle(),code,ARRAY_SIZE(code));
				}
				else{
					code[0] = 0xFFFF;
					len = 1;
				}
				lcd::displayUnicode(inputDegreeAddr[uiRowAddrIndex],code,len);
		}
		//�����Ӧ���������
		void displayFixedPulse(int index,u32 numb){
			  int uiRowAddrIndex = 3 - index;
				short code[20];
				char offset[] = {31,27,23,19,15,11,7,3,0,0,0,0,0};
				int len = 0;
				if(inputCharsLen[index] != 0){
					int pulseToFix = Setting::calcDegreePulse(numb);
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
			//����Ƕ�
			short code[20];
			u32 c = Setting::getMainAxisAngleInPulse();
			int i = tool::convertPulseToAngle(c,Setting::getPulseCountPerCircle(),code,ARRAY_SIZE(code));
			lcd::displayUnicode(mainAxixDegreeAddr,code,i);
		}
		//�������
		void clearEditorValue(u32 index){
			inputCharsLen[index]= 0;
			inputDistUM[index] = 0;
			inputChars[index][0] = 0xFFFF;
			displayMeasured(index);
			displayFixedDigree(index,inputDistUM[index]);
			displayFixedPulse(index,inputDistUM[index]);
		}
		//��ʾ�༭�����
		void displayEditorFocus(u32 index){
			editing = true;
			//lcd::displayUnicode(inputPulseAddr[currSelectedIndex],inputChars[currSelectedIndex],inputCharsLen[currSelectedIndex] + 1);
			//displayMeasured(currSelectedIndex);
			lcd::sendAddrValue(focusBoxAddr,(((u32)1) << (3 - index)));
		}
		void hideAllEditorFocus(){
			editing = false;
			lcd::sendAddrValue(focusBoxAddr,0);
		}
		bool haveValidInput(){
			 if(inputCharsLen[currSelectedIndex] != 0)return true;
			 return false;
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
						
						  currMainAxisInPulse[currSelectedIndex] = Setting::getMainAxisAngleInPulse();
						
							if(MAX_INPUT_LEN > inputCharsLen[currSelectedIndex]){
								if(inputCharsLen[currSelectedIndex] == 0){
									inputDistUM[currSelectedIndex] = 0;
									if(ext::Numb_Dot == cmd){
										inputChars[currSelectedIndex][0] = '0';
										inputCharsLen[currSelectedIndex] = 1;
									}
								}
								if(inputCharsLen[currSelectedIndex] == 1 && inputChars[currSelectedIndex][0] == '0'){
									if(ext::Numb_Dot != cmd){
										 inputCharsLen[currSelectedIndex] = 0;
									 }
								}
								if(inputCharsLen[currSelectedIndex] >= 1){
									if(inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex] - 1] == '.' && ext::Numb_Dot == cmd)break;
									if(inputCharsLen[currSelectedIndex] >= 2){
										if(inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex] - 2] == '.' && ext::Numb_Dot == cmd)break;
										if(inputCharsLen[currSelectedIndex] >= 3){
											if(inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex] - 3] == '.' && ext::Numb_Dot == cmd)break;
												if(inputCharsLen[currSelectedIndex] >= 4){
												if(inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex] - 4] == '.')break;
											}
										}
									}
								}
								if(cmd != '.'){
									unsigned long temp = inputDistUM[currSelectedIndex] * 10 + ((short)cmd - '0');
									if(temp > INPUT_NUMB_MAX)break;
									if(temp == 0 && inputCharsLen[currSelectedIndex] > 2)break;
									inputDistUM[currSelectedIndex] = temp;
								}
								inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex]] = (short)cmd;
								inputCharsLen[currSelectedIndex]++;
								inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex]] = 0xFFFF;
								
								displayMeasured(currSelectedIndex);
								checkAndFillInputWhenEditing(currSelectedIndex);
						  }
							break;
						case ext::CMD_Backspace:
							editing = true;
							inputDistUM[currSelectedIndex] = 0;
						  inputChars[currSelectedIndex][0] = 0xFFFF;
						  inputCharsLen[currSelectedIndex] = 0;
						  currMainAxisInPulse[currSelectedIndex] = Setting::getMainAxisAngleInPulse();
						  displayMeasured(currSelectedIndex);
						  checkAndFillInputWhenEditing(currSelectedIndex);
						  displayEditorFocus(currSelectedIndex);
							break;
					  case ext::CMD_Measure:
							editing = true;
							lcd::sendAddrValue(focusBoxAddr,triIconBitSelectionMask);
						  displayMeasured(currSelectedIndex);
							break; 
						case ext::CMD_RepeatMeas:
							if(inputCharsLen[currSelectedIndex] == 0)return ext::None;
							if(SelectedIndexMax >= 3)return ext::None;
						  else SelectedIndexMax ++;
						
						  checkAndFillInput(currSelectedIndex);
						
						  currMainAxisInPulse[currSelectedIndex] = Setting::getMainAxisAngleInPulse();
						
						  displayFixedDigree(currSelectedIndex,inputDistUM[currSelectedIndex]);
							displayFixedPulse(currSelectedIndex,inputDistUM[currSelectedIndex]);
						  
						  currSelectedIndex = SelectedIndexMax;
						  lcd::sendAddrValue(triIconAddr,triIconBitSelectionMask);
						  displayEditorFocus(currSelectedIndex);
							break;
						case ext::CMD_Up:
							if(editing){
						    hideAllEditorFocus();
								checkAndFillInput(currSelectedIndex);
								displayFixedDigree(currSelectedIndex,inputDistUM[currSelectedIndex]);
								displayFixedPulse(currSelectedIndex,inputDistUM[currSelectedIndex]);
								editing = false;
							}
						  if(currSelectedIndex == 0){currSelectedIndex = SelectedIndexMax;}
							else currSelectedIndex --;
						  lcd::sendAddrValue(triIconAddr,triIconBitSelectionMask);
							break;
						case ext::CMD_Dn:
							if(editing){
								hideAllEditorFocus();
								checkAndFillInput(currSelectedIndex);
								displayFixedDigree(currSelectedIndex,inputDistUM[currSelectedIndex]);
								displayFixedPulse(currSelectedIndex,inputDistUM[currSelectedIndex]);
								editing = false;
							}
						  if(inputCharsLen[currSelectedIndex] == 0)currSelectedIndex --;
						  if(currSelectedIndex >= SelectedIndexMax)currSelectedIndex = 0;
							else currSelectedIndex++;
						  lcd::sendAddrValue(triIconAddr,triIconBitSelectionMask);
							break;
						default:
							//lcd::sendKeycode(cmd);
						  return cmd;
							break;
				}
				return ext::None;
		}
		void checkAndFillInputWhenEditing(int index){
			 u32 inputLen = inputCharsLen[index];
			 u32 inputNum = inputDistUM[currSelectedIndex];
			 short inputMax[MAX_INPUT_LEN + 3];
			 for(int i = 0; i < MAX_INPUT_LEN + 3; ++i){
				  inputMax[i] = inputChars[currSelectedIndex][i];
			 }
			 //expand length to 4 characters
			 if(inputLen == 0){//   -> 0
				  inputLen = 1;
				  inputMax[inputLen] = '0';
			 }
			 if(inputLen == 1){
				 inputLen = 2;
				 if(inputMax[0] != '.'){// x -> x.
						inputMax[1] = '.';
				 }
				 else{
					  inputMax[1] = '0'; // . -> .0
				 }
			 }
			 if(inputLen == 2){
				 inputLen = 3;
				 if(inputMax[0] == '.'){ // .a -> .a0
						inputMax[2] = '0';
					  inputNum = inputNum * 10;
				 }
				 else if(inputMax[1] == '.'){// x. -> x.0
						inputMax[2] = '0';
					  inputNum = inputNum * 10;
				 }
				 else{
					  inputMax[2] = '.'; //yx -> yx.
				 }
			 }
			 if(inputLen == 3){
				 inputLen = 4;
				 if(inputMax[0] == '.'){// .ab -> .abc
						inputMax[3] = '0';
					  inputNum = inputNum * 10;
				 }
				 else if(inputMax[1] == '.'){// x.a -> x.a0
						inputMax[3] = '0';
					  inputNum = inputNum * 10;
				 }
				 else if(inputMax[2] == '.'){// yx. -> yx.0
					  inputMax[3] = '0';
					  inputNum = inputNum * 10;
				 }
				 else{											 //zyx -> zyx.
					  inputMax[3] = '.';
				 }
			 }
			 if(inputLen >= 4){
				  if(inputMax[inputLen - 4] != '.' && //not    *.abc
						 inputMax[inputLen - 3] != '.' && //not   *x.ab
						 inputMax[inputLen - 2] != '.' && //not  *yx.a
					   inputMax[inputLen - 1] != '.'){  //not *zyx.
						 inputMax[inputLen] = '.';        //*wzyx -> *wzyx.
						 inputLen ++;
					}
					if(inputMax[inputLen - 1] == '.'){// *. -> *.0
						inputMax[inputLen] = '0';
						inputLen ++;
						inputMax[inputLen] = 0xFFFF;
						inputNum = inputNum * 10;
					}
					if(inputMax[inputLen - 2] == '.'){// *.a -> *.a0
						inputMax[inputLen] = '0';
						inputLen ++;
						inputMax[inputLen] = 0xFFFF;
						inputNum = inputNum * 10;
					}
					if(inputMax[inputLen - 3] == '.'){// *.ab -> *.ab0
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
			 //expand length to 4 characters
			 if(inputCharsLen[currSelectedIndex] == 0){//   -> 0
				  inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex]] = '0';
				  inputCharsLen[currSelectedIndex] = 1;
			 }
			 if(inputCharsLen[currSelectedIndex] == 1){
				 inputCharsLen[currSelectedIndex] = 2;
				 if(inputChars[currSelectedIndex][0] != '.'){// x -> x.
						inputChars[currSelectedIndex][1] = '.';
				 }
				 else{																				// . -> .0
					 inputChars[currSelectedIndex][1] = '0';
				 }
			 }
			 if(inputCharsLen[currSelectedIndex] == 2){
				 inputCharsLen[currSelectedIndex] = 3;
				 if(inputChars[currSelectedIndex][0] == '.'|| // .a -> .a0
					  inputChars[currSelectedIndex][1] == '.'){  // x. -> x.0
						inputChars[currSelectedIndex][2] = '0';
					  inputDistUM[currSelectedIndex] = inputDistUM[currSelectedIndex] * 10;
				 }
				 else{																						// yx -> yx.
					  inputChars[currSelectedIndex][2] = '.';
				 }
			 }
			 if(inputCharsLen[currSelectedIndex] == 3){
				 inputCharsLen[currSelectedIndex] = 4;
				 if(inputChars[currSelectedIndex][0] == '.' || // .ab -> .ab0
					  inputChars[currSelectedIndex][1] == '.' || // x.a -> x.a0
				    inputChars[currSelectedIndex][2] == '.' ){ // yx. -> yx.0
						inputChars[currSelectedIndex][3] == '0';
					  inputDistUM[currSelectedIndex] = inputDistUM[currSelectedIndex] * 10;
				 }
				 else{																						// wyx -> wyx.
					  inputChars[currSelectedIndex][2] = '.';
				 }
			 }
			 if(inputCharsLen[currSelectedIndex] >= 3){
				  if(inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex] - 4] != '.' &&  //.abc
						 inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex] - 3] != '.' &&  //x.ab
						 inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex] - 2] != '.' &&  //yx.a
					   inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex] - 1] != '.'){   //wyx.
						 inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex]] = '.';
						 inputCharsLen[currSelectedIndex] ++;
					}
					if(inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex] - 1] == '.'){ // *. -> *.0
						inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex]] = '0';
						inputCharsLen[currSelectedIndex] ++;
						inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex]] = 0xFFFF;
						inputDistUM[currSelectedIndex] = inputDistUM[currSelectedIndex] * 10;
					}
					if(inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex] - 2] == '.'){ // *.a -> *.a0
						inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex]] = '0';
						inputCharsLen[currSelectedIndex] ++;
						inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex]] = 0xFFFF;
						inputDistUM[currSelectedIndex] = inputDistUM[currSelectedIndex] * 10;
					}
					if(inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex] - 3] == '.'){ // *.ab -> *.ab0
						inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex]] = '0';
						inputCharsLen[currSelectedIndex] ++;
						inputChars[currSelectedIndex][inputCharsLen[currSelectedIndex]] = 0xFFFF;
						inputDistUM[currSelectedIndex] = inputDistUM[currSelectedIndex] * 10;
					}
				}
				dotNumbToUnicode(index,inputDistUM[index]);
				displayMeasured(currSelectedIndex);
		}
		void handleEncoder(char dir){
			 /*if(dir)testCount += 1;
			 else testCount -= 1;
			 for(int i = 0; i < 4; i ++){
				 Setting::setInput(i,testCount);
				 inputDistUM[i] = testCount;
				 dotNumbToUnicode(i);
			 }
			 displayAll();*/
		}
		void onTimer(){
			//����Ƕ�
			displayMainAxisDegree();		
		}
		void dotNumbToUnicode(int index,u32 numbInputted){
			  u32 times = 1000000000;
			  int len = 0;
			  unsigned int numb = numbInputted;
			  while(times != Setting::getPrecision()){
					 if(numb / times != 0){
						  break;
					 }
					 times /= 10;
				}
				while(times != Setting::getPrecision()){
					inputChars[index][len] = numb / times + '0';
					len ++;
					numb %= times;
					times /= 10;
				}
				inputChars[index][len] = numb / Setting::getPrecision() + '0';
				len ++;
				numb %= Setting::getPrecision();
				
				inputChars[index][len] = '.';
				len ++;
				
				inputChars[index][len] = numb / 100 + '0';
				len ++;
				inputChars[index][len] = (numb % 100) / 10 + '0';
				len ++;
				inputChars[index][len] = numb % 10 + '0';
				len ++;
				inputChars[index][len] = 0xffff;
				inputCharsLen[index] = len;
		}
protected://for data
	short inputCharsLen[4];
	wchar inputChars[4][MAX_INPUT_LEN + 2];
	s32   inputDistUM[4];

	char currSelectedIndex;
	char SelectedIndexMax;
	s32  currMainAxisInPulse[4];
protected://for UI
	int  testCount;
	int  rotateCount;
	bool editing;

	short inputPulseAddr[4];
	short inputDegreeAddr[4];
	short triIconAddr;
	short focusBoxAddr;
	short mainAxixDegreeAddr;
	short pulseBaseAddr;
	short measureBassAddr;
};
#endif