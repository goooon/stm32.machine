#ifndef ZLG_PAGE11_HH
#define ZLG_PAGE11_HH
#include "./Page.h"
#include "../api/lcd.h"
class Page11 : public Page
{
public:
	virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
		{
			LOG_I("key pressed:%c",cmd);
			  switch(cmd){
					  case ext::CMD_FanHui:
							lcd::jumpToPage(6);
							break;
					  default:
							lcd::sendKeycode(cmd);
							return cmd;
						break;
				}
				return cmd;
		}
};
#endif