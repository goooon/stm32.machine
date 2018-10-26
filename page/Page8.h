#ifndef ZLG_PAGE8_HH
#define ZLG_PAGE8_HH
#include "./Page.h"
#include "../api/lcd.h"

//∂‘µ∂ΩÁ√Ê
class Page8 : public Page
{
public:
	virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
		{
			LOG_I("cmd %c[0x%x/%d]",cmd,cmd,cmd);
			switch(cmd){
				case ext::CMD_FanHui:
					lcd::jumpToPage(4);
				  return ext::None;
				default:
					return cmd;
				
			}
			return cmd;
		}
};
#endif