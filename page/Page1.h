#ifndef ZLG_PAGE1_HH
#define ZLG_PAGE1_HH
#include "./Page.h"
#include "../api/lcd.h"
class Page1 : public Page
{
public:
	Page1():timeCounter(0){}
	virtual void onTimer(){
			  timeCounter++;
			  LOG_I("onTimer");
			  if(timeCounter >= 10){
					lcd::jumpToPage(2);
					timeCounter = 0;
				}
		}
private:
		u32 timeCounter;
};
#endif