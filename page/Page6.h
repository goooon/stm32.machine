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
			 display(roundPerMin,angle);
		}
		virtual void leave(){
			
		}
		void display(u32 r,u32 a){
			  short code[20];
			  u32 measureDist,measurePuls;
			  s32 whellDist,whellPulse;
			  
			  Setting::getWhellFixPulse(whellDist,whellPulse);
			  //机床转速
			  int i = tool::convertFixed(r,0,code,20,false);
			  i--;
			  code[i++] = 0x8f6c;
			  code[i++] = 0x002f;
			  code[i++] = 0x5206;
			  code[i++] = 0xffff;
			  lcd::displayUnicode(0x610,code,i);
			  //主轴角度
			  i = tool::convertPulseToAngle(a,1024,code,ARRAY_SIZE(code));
			  lcd::displayUnicode(0x620,code,i);
			  //测量修正
			  Setting::getMeasureFixPulse(measureDist,measurePuls);
			  i = tool::convertFixed(measureDist,100,code,20);
			  i--;
			  code[i++] = 'm';
			  code[i++] = 'm';
			  code[i++] = 0xffff;
			  lcd::displayUnicode(0x630,code,i);
				
			  i = tool::convertFixed(measurePuls,0,code,20);
			  i--;
			  code[i++] = 0x8109;
			  code[i++] = 0x51b2;
			  code[i++] = 0xffff;
			  lcd::displayUnicode(0x640,code,i);
			  //手轮修正
			  i = tool::convertFixed(whellDist,100,code,20);
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
			  //合计修正距离
			  i = tool::convertFixed(measureDist + whellDist,100,code,20); 
			  i--;
			  code[i++] = 'm';
			  code[i++] = 'm';
			  code[i++] = 0xffff;
			  lcd::displayUnicode(0x690,code,i);
				total = measurePuls + whellPulse;
				total %= 1024;
				if(total < 0)total += 1024;
				i = tool::convertFixed(total,0,code,20);
			  i--;
			  code[i++] = 0x8109;
			  code[i++] = 0x51b2;
			  code[i++] = 0xffff;
			  lcd::displayUnicode(0x6a0,code,i);
				
		}
		virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
		{
			LOG_I("key pressed:%c/0x%x",cmd,cmd);
			u8 data;
			switch(cmd){
				  case ext::CMD_FanHui:
						lcd::jumpToPage(5);
						FPGA_CONFIG();
						return ext::None;
						break;
					case ext::CMD_KnifeFix:
						lcd::jumpToPage(11);
						return ext::None;
						break;
					case ext::CMD_WheelFix:
						lcd::jumpToPage(12);
						return ext::None;
						break;
					case ext::CMD_Start:
						FPGA_CONFIG();
						FPGA_SET_DELAYED_PULSE(total);
					  FPGA_START_WORKING();
					  LOG_I("start working 0x%x",total);
						return ext::None;
						break;
					case ext::CMD_Stop:
						FPGA_SET_DELAYED_PULSE(0);
					  FPGA_RESET();
					  LOG_I("reset fpga 0x%x",total);
						return ext::None;
					//
					case ext::Numb_0:
							data = ext::Fpga::Read(0);
						  LOG_I("read 0x%x",data);
						  break;
						 case ext::Numb_1:
							data = ext::Fpga::Read(1);
						  LOG_I("read 0x%x",data);
						  break;
						 case ext::Numb_2:
						  ext::Fpga::Write(2,total);
						  LOG_I("addr 2 write:0x%x",(u8)total);
						  break;
						 case ext::Numb_3:
							ext::Fpga::Write(3,total>>8);
						  LOG_I("addr 3 write:0x%x",total>>8);
						  break;
						 case ext::Numb_4:
							data = ext::Fpga::Read(2);
						  LOG_I("read from addr 2: 0x%x",data);
						  break;
						 case ext::Numb_5:
							data = ext::Fpga::Read(3);
						  LOG_I("read from addr 3: 0x%x",data);
						  break;
						 case ext::Numb_6:
							ext::Fpga::Write(15,0);
						  LOG_I("addr 15 write:0");
						  break;
						 case ext::Numb_7:
							ext::Fpga::Write(15,1);
						  LOG_I("addr 15 write:1");
						  break;
						 case ext::Numb_8:
							ext::Fpga::Write(15,2);
						  LOG_I("addr 15 write:2");
						  break;
						 case ext::Numb_9:
							data = ext::Fpga::Read(15);
						  LOG_I("read from addr 0xf: 0x%x",data);
						  break;
						 case ext::Numb_Dot:
							ext::Fpga::Write(2,total);
						  ext::Fpga::Write(3,total>>8);
						break;
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
};
#endif