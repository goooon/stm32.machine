#ifndef ZLG_PAGE6_HH
#define ZLG_PAGE6_HH
#include "./Page.h"
#include "../api/lcd.h"
#include "../setting/Setting.h"
#include "../api/Tool.h"
#include "../api/FPGA.h"
class Page6 : public Page
{
public:
		virtual void enter(){
			 u32 roundPerMin = Setting::getRoundPerMin();
			 u32 angle = Setting::getMainAxisAngleInPulse();
			 //state = 0;
			 display(roundPerMin,angle);
			 //ext::Led::SetLed(ext::Led::ProcessingLed,false);
			 updateFpgaData();
		}
		virtual void leave(){
			ext::Led::SetLed(ext::Led::ProcessingLed,false);
			ext::Led::SetLed(ext::Led::WheelFixLed,false);
			ext::Led::SetLed(ext::Led::TranspantLed,true);
		}
		void display(u32 r,u32 curAngle){
			  short code[20];
			  s32 defaultInputDist;
			  s32 defaultMainAxisInPulse;
			  u32 measureDist,measurePuls;
			  s32 whellDist,whellPulse;
			  s32 totalDist;
			  s32 combinedFixDist;
			  s32 combinedFixPulse;
			  Setting::getWhellFixPulse(whellDist,whellPulse);
			  //JiChuangZhuanSu
			  int i = tool::convertFixed(r,0,code,20,false);
			  i--;
			  code[i++] = 0x8f6c;
			  code[i++] = 0x002f;
			  code[i++] = 0x5206;
			  code[i++] = 0xffff;
			  lcd::displayUnicode(0x610,code,i);
			  //ZhuZhouJiaoDu
			  i = tool::convertPulseToAngle(curAngle,Setting::getPulseCountPerCircle(),code,ARRAY_SIZE(code));
			  lcd::displayUnicode(0x620,code,i);
			  //GongMuKou
			  if(Setting::getConfigToothType() == 1){//male
					code[0] = 0x516c;//
					
				}
				else if(Setting::getConfigToothType() == 2){//female
					code[0] = 0x6bcd;//
				}
				else{
					LOG_E("unknown tooth type %d",Setting::getToothCount());
					code[0] = 0x516c;//
				}
				code[1] = 0x6263;
				code[2] = 0x94bb;
				code[3] = 0x6746;
				code[4] = 0xffff;
				lcd::displayUnicode(0x6B0,code,4);
				//YaShu
			  if(Setting::getToothCount() == 4){
					 code[0] = 0x0034;//'4'
				}
				else if(Setting::getToothCount() == 6){
					 code[0] = 0x0036;//'6'
				}
				else if(Setting::getToothCount() == 8){
					 code[0] = 0x0038;//'8'
				}
				else{
					//use default
					LOG_E("unknown tooth count %d",Setting::getToothCount());
					code[0] = 0x0034;//'4'
				}
			  code[1] = 0x7259;
				code[2] = 0x87ba;
				code[3] = 0x8ddd;
				code[4] = 0x94bb;
				code[5] = 0x6746;
				code[6] = 0xffff;
				lcd::displayUnicode(0x6D0,code,7);
			  //CeLiangXiuZheng
				defaultInputDist = Setting::getBaseConfigInput(Setting::getDefaultBaseConfigInputIndex(),defaultMainAxisInPulse);
			  Setting::getMeasureFixPulse(measureDist,measurePuls);

				combinedFixPulse = Setting::calculateDelayedPulse(defaultMainAxisInPulse,defaultInputDist,measurePuls,measureDist);
			  combinedFixDist = Setting::pulseToDistUM(combinedFixPulse);
				i = tool::convertFixed(combinedFixDist,Setting::getPrecision(),code,20);
			  i--;
			  code[i++] = 'm';
			  code[i++] = 'm';
			  code[i++] = 0xffff;
			  lcd::displayUnicode(0x630,code,i);
				
				//combinedFixPulse = Setting::distUMToPulse(Fix);//距离差换算脉冲
			  i = tool::convertFixed(combinedFixPulse,0,code,20);
			  i--;
			  code[i++] = 0x8109;
			  code[i++] = 0x51b2;
			  code[i++] = 0xffff;
			  lcd::displayUnicode(0x640,code,i);
				
			  //ShouLunXiuZheng
			  i = tool::convertFixed(whellDist,Setting::getPrecision(),code,20);
			  i--;
			  code[i++] = 'm';
			  code[i++] = 'm';
			  code[i++] = 0xffff;
			  lcd::displayUnicode(0x650,code,i);
				i = tool::convertFixed(whellPulse,0,code,20);
			  i--;
			  code[i++] = 0x8109;
			  code[i++] = 0x51b2;
			  code[i++] = 0xffff;
			  lcd::displayUnicode(0x660,code,i);
				
			  //HeJiXiuZheng
				totalDist = combinedFixDist + whellDist;
				if(totalDist < 0)totalDist += Setting::getDistUMCountPerTooth();
			  i = tool::convertFixed(totalDist,Setting::getPrecision(),code,20); 
			  i--;
			  code[i++] = 'm';
			  code[i++] = 'm';
			  code[i++] = 0xffff;
			  lcd::displayUnicode(0x690,code,i);
				total = combinedFixPulse + whellPulse;
				total %= Setting::getPulseCountPerCircle();
				total += Setting::getPulseCountPerCircle();
				total %= Setting::getPulseCountPerCircle();
					
				i = tool::convertFixed(total,0,code,20);
			  i--;
			  code[i++] = 0x8109;
			  code[i++] = 0x51b2;
			  code[i++] = 0xffff;
			  lcd::displayUnicode(0x6a0,code,i);
				displayState();
		}
		void displayState(){
			short code[20];
			int i = 0;
			if(state == 0){//
					code[0] = 0x672a;
					code[1] = 0x5f00;
					code[2] = 0x59cb;
					code[3] = 0x52a0;
					code[4] = 0x5de5;
					code[5] = 0xffff;
					i = 6;
				}
				else if(state == 1){//
					code[0] = 0x6b63;
					code[1] = 0x5728;
					code[2] = 0x52a0;
					code[3] = 0x5de5;
					code[4] = 0x002e;
					code[5] = 0x002e;
					code[6] = 0x002e;
					code[7] = 0xffff;
					i = 8;
				}
				else{//
					code[0] = 0x52a0;
					code[1] = 0x5de5;
					code[2] = 0x7ed3;
					code[3] = 0x675f;
					code[4] = 0xffff;
					i = 5;
				}
				lcd::displayUnicode(0x6E0,code,i);
		}
		void updateFpgaData(){
			state = 1;
			displayState();
			FPGA_SET_DELAYED_PULSE(total);
			FPGA_START_WORKING();
			ext::Led::SetLed(ext::Led::ProcessingLed,true);
			ext::Led::SetLed(ext::Led::TranspantLed,false);
			LOG_I("start working 0x%x",total);
		}
		virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
		{
			LOG_I("key pressed:%c/0x%x",cmd,cmd);
			u8 data;
			switch(cmd){
				  case ext::CMD_FanHui:
						if(state == 1){
							//return ext::None;
						}
						FPGA_RESET();
						Setting::setWhellFixPulse(0,0);
						lcd::jumpToPage(5);
						return ext::None;
						break;
					case ext::CMD_KnifeFix:
						if(state == 1){
							return ext::None;
						}
						FPGA_RESET();
						lcd::jumpToPage(11);
						return ext::None;
						break;
					case ext::CMD_WheelFix:
						if(state == 1){
							//return ext::None;
						}
						FPGA_RESET();
						lcd::jumpToPage(12);
						return ext::None;
						break;
					case ext::CMD_Start:
						/*state = 1;
					  displayState();
					  ext::Led::SetLed(ext::Led::ProcessingLed,true);
						ext::Led::SetLed(ext::Led::TranspantLed,false);
						FPGA_CONFIG();
						FPGA_SET_DELAYED_PULSE(total);
					  FPGA_START_WORKING();
					  LOG_I("start working 0x%x",total);*/
					  updateFpgaData();
						return ext::None;
						break;
					case ext::CMD_Stop:
						/*state = 2;
					  ext::Led::SetLed(ext::Led::ProcessingLed,false);
					  ext::Led::SetLed(ext::Led::TranspantLed,true);
						displayState();
						FPGA_SET_DELAYED_PULSE(0);
					  FPGA_RESET();*/
					  Setting::setWhellFixPulse(0,0);
					  ext::Led::SetLed(ext::Led::WheelFixLed,false);
						lcd::jumpToPage(5);
						FPGA_RESET();
					  LOG_I("reset fpga 0x%x",total);
						return ext::None;
					// below is to debut fpga
					// case ext::Numb_0:
					// 		data = ext::Fpga::Read(0);
					// 	  LOG_I("read 0x%x",data);
					// 	  break;
					// 	 case ext::Numb_1:
					// 		data = ext::Fpga::Read(1);
					// 	  LOG_I("read 0x%x",data);
					// 	  break;
					// 	 case ext::Numb_2:
					// 	  ext::Fpga::Write(2,total);
					// 	  LOG_I("addr 2 write:0x%x",(u8)total);
					// 	  break;
					// 	 case ext::Numb_3:
					// 		ext::Fpga::Write(3,total>>8);
					// 	  LOG_I("addr 3 write:0x%x",total>>8);
					// 	  break;
					// 	 case ext::Numb_4:
					// 		data = ext::Fpga::Read(2);
					// 	  LOG_I("read from addr 2: 0x%x",data);
					// 	  break;
					// 	 case ext::Numb_5:
					// 		data = ext::Fpga::Read(3);
					// 	  LOG_I("read from addr 3: 0x%x",data);
					// 	  break;
					// 	 case ext::Numb_6:
					// 		ext::Fpga::Write(15,0);
					// 	  LOG_I("addr 15 write:0");
					// 	  break;
					// 	 case ext::Numb_7:
					// 		ext::Fpga::Write(15,1);
					// 	  LOG_I("addr 15 write:1");
					// 	  break;
					// 	 case ext::Numb_8:
					// 		ext::Fpga::Write(15,2);
					// 	  LOG_I("addr 15 write:2");
					// 	  break;
					// 	 case ext::Numb_9:
					// 		data = ext::Fpga::Read(15);
					// 	  LOG_I("read from addr 0xf: 0x%x",data);
					// 	  break;
					// 	 case ext::Numb_Dot:
					// 		ext::Fpga::Write(2,total);
					// 	  ext::Fpga::Write(3,total>>8);
					// 	break;
				}
			return cmd;
		}
		void handleEncoder(char dir){
			 //if(dir)count += 1;
			 //else count -= 1;
			 //display(count,count);
		}
		void onTimer(){
			 u32 roundPerMin = Setting::getRoundPerMin();
			 u32 angle = Setting::getMainAxisAngleInPulse();
			 display(roundPerMin,angle);
			 //LOG_I("Timer2 Trigger: axis pulse %d",angle);
		}
	private:
	  s32 total;
	  u8  state;//0: not started, 1: started, 2: stopped
};
#endif