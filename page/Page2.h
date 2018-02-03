#ifndef ZLG_PAGE2_HH
#define ZLG_PAGE2_HH
#include "./Page.h"
#include "../api/lcd.h"
#include "../api/FPGA.h"
extern "C"{
#include "../sys/delay.h"
#include "../sys/usart.h"
}
class Page2 : public Page
{
public:
	  Page2(){
			  
		}
		virtual void enter(){
			LOG_I("Page2 enter");
			timeCounter = 0;
		}
		virtual void onTimer(){
			  timeCounter++;
			  LOG_I("onTimer");
			  if(timeCounter >= 1000){
					lcd::jumpToPage(3);
				}
		}
		virtual ext::ExeCommand onKeyPressed(ext::ExeCommand cmd)
		{
			  u8 data;
				LOG_I("key pressed:%c/0x%x",cmd,cmd);
			  switch(cmd){
						 case ext::CMD_Enter:
							lcd::jumpToPage(3);
							return cmd;
						 case ext::Numb_0:
							data = ext::Fpga::Read(0);
						  LOG_I("read 0x%x",data);
						  break;
						 case ext::Numb_1:
							data = ext::Fpga::Read(1);
						  LOG_I("read 0x%x",data);
						  break;
						 case ext::Numb_2:
						  ext::Fpga::Write(2,0xB8);
						  LOG_I("addr 2 write:0xB8");
						  break;
						 case ext::Numb_3:
							ext::Fpga::Write(3,0x0B);
						  LOG_I("addr 3 write:0x0B");
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
						break;
				}
		}
		private:
			int timeCounter;
};
#endif