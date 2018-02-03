#ifndef ZLG_PAGE6_HH
#define ZLG_PAGE6_HH
#include "./Page.h"
#include "../api/lcd.h"
#include "../setting/Setting.h"
#include "../api/Tool.h"
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
			  Setting::getMeasureFixPulse(measureDist,measurePuls);
			  Setting::getWhellFixPulse(whellDist,whellPulse);
			  //����ת��
			  int i = tool::convertFixed(r,0,code,20,false);
			  i--;
			  code[i++] = 0x8f6c;
			  code[i++] = 0x002f;
			  code[i++] = 0x5206;
			  code[i++] = 0xffff;
			  lcd::displayUnicode(0x610,code,i);
			  //����Ƕ�
			  i = tool::convertPulseToAngle(count,100,code,ARRAY_SIZE(code));
			  lcd::displayUnicode(0x620,code,i);
			  //��������
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
			  //��������
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
			  //�ϼ���������
			  i = tool::convertFixed(measureDist + whellDist,100,code,20); 
			  i--;
			  code[i++] = 'm';
			  code[i++] = 'm';
			  code[i++] = 0xffff;
			  lcd::displayUnicode(0x690,code,i);
				i = tool::convertFixed(measurePuls + whellPulse,0,code,20);
			  i--;
			  code[i++] = 0x8109;
			  code[i++] = 0x51b2;
			  code[i++] = 0xffff;
			  lcd::displayUnicode(0x6a0,code,i);
				
		}
		virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
		{
			LOG_I("key pressed:%c/0x%x",cmd,cmd);
			switch(cmd){
				  case ext::CMD_FanHui:
						lcd::jumpToPage(3);
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
						return ext::None;
						break;
					case ext::CMD_Stop:
						return ext::None;
						break;
				}
			return cmd;
		}
		void handleEncoder(char dir){
			 if(dir)count += 1;
			 else count -= 1;
			 display(count,count);
		}
	private:
		u32 count;
};
#endif