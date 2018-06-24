#ifndef ZLG_PAGE12_HH
#define ZLG_PAGE12_HH
#include "./Page.h"
#include "../api/lcd.h"
#include "../api/Tool.h"
#include "../setting/Setting.h"
//手轮调整
void callback(void){
}
class Page12 : public Page
{
public:
	void updateIcon(){
		lcd::sendAddrValue(0x1210,index == 1 ? 0 : 1);
		lcd::sendAddrValue(0x1220,index == 10 ? 0 : 1);
		lcd::sendAddrValue(0x1230,index == 100 ? 0 : 1);
	}
	virtual void enter(){
		s32 dist;
		Setting::getWhellFixPulse(dist,count);
		index = 1;
		updateIcon();
		display();
		//Timer7_Init(2000,36000,callback,0);
	}
	virtual void leave(){
	}
	virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
		{
			LOG_I("key pressed:%c[0x%x,%d]",cmd,cmd,cmd);
			  switch(cmd){
					  case '1':
							index = 1;
							updateIcon();
							break;
						case '2':
							index = 10;
						  updateIcon();
						  break;
						case '3':
							index = 100;
						  updateIcon();
							break;
					  case ext::CMD_FanHui:
						case ext::CMD_Enter:
						case ext::CMD_Input:
							Setting::setWhellFixPulse(count * 625 / 1024,count);
							lcd::jumpToPage(6);
							break;
					  default:
							lcd::sendKeycode(cmd);
							return cmd;
						break;
				}
				return cmd;
		}
		void display(){
			 short code[20];
			 /*unsigned long numb = count;
			 if(count < 0){code[0] = 0x2d; i = 1; numb = -count;}
			 else if(count > 0){code[0] = 0x2b; i = 1;}
			 else i = 0;
			 
			 unsigned long times = 10000;
			 if(numb != 0){
				 while(numb / times == 0){
						times /= 10;
				 }
				 while(times != 0){
					 code[i] = numb / times + '0';
					 numb %= times;
					 times /= 10;
					 LOG_I("n %d,t %d",numb,times);
					 i++;
				 }
			 }
			 else{
				  code[i] = '0';
				  i = 1;
			 }
			 code[i] = 0xffff;
			 i++;
			 LOG_I("i is %d",i);*/
			 int i = tool::convertFixed(count,0,code,20);
			 LOG_I("i is %d",i);
			 if(i == 6){
				 lcd::sendAddrValue(0x2241,0x0178,0x006a);
			 }
			 else if(i == 5){
				 lcd::sendAddrValue(0x2241,0x017a,0x006a);
			 }
			 else if(i == 4){
				 lcd::sendAddrValue(0x2241,0x0180,0x006a);
			 }
			 else if(i == 3){
				 lcd::sendAddrValue(0x2241,0x0183,0x006a);
			 }
			 else if(i == 2){
				 lcd::sendAddrValue(0x2241,0x018a,0x006a);
			 }
			 lcd::displayUnicode(0x1240,code,i);
			 //Z轴调整距离
			 int len = tool::convertFixed(count * 625 / 1024,100,code,20);
			 lcd::displayUnicode(0x1280,code,len);
		}
		void handleEncoder(char dir){
			 if(dir)count += index;
			 else count -= index;
			 if(count > 512)count = 512;
			 if(count < -512)count = -512;
			 //lcd::sendAddrValue(0x1240,count);
			 //short code[] = {0x002b,0x0031,0x002d,0x0032,0x0033,0x0034,0x4f60,0x597d};
			 display();
		}
	private:
		s32 count;
	  char index;
};
#endif