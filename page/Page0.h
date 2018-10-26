#ifndef ZLG_PAGE0_HH
#define ZLG_PAGE0_HH
#include "./Page.h"
#include "../api/lcd.h"
class Page0 : public Page
{
public:
	Page0():timeCounter(0){}
	virtual void onTimer(){
			  timeCounter++;
			  LOG_I("onTimer");
			  if(timeCounter >= 50){  //5ÃëºóÌø×ªµ½page1
					lcd::jumpToPage(1);
					timeCounter = 0;
				}
		}
private:
		u32 timeCounter;
};
#endif