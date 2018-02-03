#ifndef ZLG_PAGE3_HH
#define ZLG_PAGE3_HH
#include "./Page.h"
#include "../api/lcd.h"
class Page3 : public Page
{
public:
	  Page3(){
			  
		}
		virtual void enter(){
			LOG_I("Page3 enter");
		}
		virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
		{
				LOG_I("key pressed:%c",cmd);
			  switch(cmd){
					  case ext::CMD_Input:
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