/*char KeyCode[] = 
21:{0xA5,0x5A,0x03,0x80,0x4F,0x21};   //A55A03804F21
31:{0xA5,0x5A,0x03,0x80,0x4F,0x31};   //A55A03804F30
32:{0xA5,0x5A,0x03,0x80,0x4F,0x32};   //A55A03804F31

/*jump to page*/
//{0xA5,0x5A,0x04,0x80,0x03,0x00,0x02};   //A55A0480030002
/*read current page*/
//{0xA5,0x5A,0x03,0x81,0x03,0x02}	//A55A03810302
//图标变量 A55A0582xxxxyyyy
//变量存储地址
//0510://A55A0A82051056494557313233   //view123
//0510://A55A0B820510CEE4BABAD6D0CFD4
//A55A07820510316DC4E3   //
//GBK:// A55A09820510316DC4E3BAC3
//UNICODE://A55A098205140031006D4F60597D
//SELECT://A55A05820508000F

//A55A07822241016f0000
#ifndef ZLG_PAGE_HH
#define ZLG_PAGE_HH
#include "../api/KeyBoard.h"
#include "../api/lcd.h"
class Page
{
public:
	  Page():shouldHandle(false){}
	  void handleEnter(){
			enter();
			shouldHandle = true;
		}
    void handleLeave(){
			shouldHandle = false;
      leave();
    }
    void handleTimer(){ if(shouldHandle)return onTimer();}
    ext::ExeCommand handleKeyPressed(ext::ExeCommand cmd){
			if(!shouldHandle){
				return ext::None;
			}
			return onKeyPressed(cmd);
		}
protected:
	  virtual void enter(){}
		virtual void leave(){}
		virtual void onTimer(){}
		virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd){
			LOG_W("unhandled comd %d",cmd);
			LOG_I("key pressed:%c/0x%x",cmd,cmd);
			switch(cmd){
				  case '<':
						break;
				}
			return cmd;
		};
private:
	  bool shouldHandle;
};
#endif