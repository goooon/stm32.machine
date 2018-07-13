#ifndef ZLG_PAGE3_HH
#define ZLG_PAGE3_HH
#include "./Page.h"
#include "../api/lcd.h"
#include "../setting/Setting.h"
#include "../api/Led.h"
class Page3 : public Page
{
public:
	  Page3(){
			  
		}
		virtual void enter(){
			LOG_I("Page3 enter");
			dispToothDist();
			dispToothType();
			dispToothCoverType();
			u32 toothCount = Setting::getToothCount();
			ext::Led::SetLed(ext::Led::TranspantLed,true);
		}
		void dispToothDist(){
			short code[20];
			int i = 0;
			code[i++] = 0x0036;
			code[i++] = 0x002e;
			code[i++] = 0x0033;
			code[i++] = 0x0035;
			code[i++] = 0x006d;
			code[i++] = 0x006d;
			code[i++] = 0xffff;
			lcd::displayUnicode(0x310,code,i);
		}
		void dispToothType(){
			 short code[20];
			 int i = 0;
			 ToothType type = Setting::getConfigToothType();
			 //\u516c\u6263\u87ba\u7eb9
			 code[i++] = 0x516c;
			 code[i++] = 0x6263;
			 code[i++] = 0x87ba;
			 code[i++] = 0x7eb9;
			 code[i++] = 0xffff;
			 if(type == Male){
				  code[0] = 0x516c;
				  lcd::displayUnicode(0x340,code,i);
			 }
			 else if(type == Female){
				  code[0] = 0x6bcd;
				  lcd::displayUnicode(0x340,code,i);
			 }
			 else{
			 }
		}
		void dispToothCoverType(){
			short code[20];
			 int i = 0;
			 ToothType type = Setting::getConfigToothType();
		   //ÕýË¿ÂÝÎÆ¿Û
			 //\u6b63\u4e1d\u87ba\u7eb9\u6263
			 code[i++] = 0x6b63;
			 code[i++] = 0x4e1d;
			 code[i++] = 0x87ba;
			 code[i++] = 0x7eb9;
			 code[i++] = 0x6263;
			 code[i++] = 0xffff;

			 lcd::displayUnicode(0x370,code,i);

		}
		virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
		{
				LOG_I("key pressed:%c",cmd);
			  switch(cmd){
					  case ext::CMD_Start:
							lcd::jumpToPage(5);
							break;
						case ext::CMD_Setting:
							lcd::jumpToPage(4);
							break;
					  default:
							lcd::sendKeycode(cmd);
							return cmd;
						break;
				}
				return cmd;
		}
		private:
};
#endif