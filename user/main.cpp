extern "C"{
#include "../sys/delay.h"
#include "../sys/usart.h"
#include "../api/Encoder.h"
}	

#include "../api/Tracer.h"
#include "../api/Timer.h"
#include "../api/Event.h"
#include "../api/Keyboard.h"
#include "../api/Led.h"
#include "../api/Fpga.h"
#include "../page/page0.h"
#include "../page/page1.h"
#include "../page/page2.h"
#include "../page/page3.h"
#include "../page/page4.h"
#include "../page/page5.h"
#include "../page/page6.h"
#include "../page/page7.h"
#include "../page/page8.h"
#include "../page/page9.h"
#include "../page/page10.h"
#include "../page/page11.h"
#include "../page/page12.h"

#include "../setting/setting.h"
char tempBuff3[1024];
void onReceive3(void* buff,int len){
	char* tmp = (char*)buff;
	//tmp[len] = 0;
	if(len >= 1024)len = 1023;
	for(int i = 0; i < len; ++ i){
		tempBuff3[i] = tmp[i];
	}
	tempBuff3[len] = 0;
	PostEvent(UART3,0,len,tempBuff3);
	//LOG_I("recevied %d %s",len,tmp);
}

char tempBuff2[1024];
void onReceive2(void* buff,int len){
	char* tmp = (char*)buff;
	//tmp[len] = 0;
	if(len >= 1024)len = 1023;
	for(int i = 0; i < len; ++ i){
		tempBuff2[i] = tmp[i];
	}
	tempBuff2[len] = 0;
	PostEvent(UART2,0,len,tempBuff2);
	//LOG_I("recevied %d %s",len,tmp);
}

void onExitInterupt(char port,char index){
	char data = GPIO_ReadInputData(GPIOA) & 0xC0;
	PostEvent(EXITR,data,index,0);
}
void onTimer2Callback(void* ){
	PostEvent(Timer2,0,0,0);
}
void onTimer7Callback(void* ){
	Setting::incRoundPerMinTicks();
	PostEvent(Timer1,0,0,0);
}
char uart3TxBuffer[1024];
char uart3RxBuffer[1024];
char uart2TxBuffer[1024];
char uart2RxBuffer[1024];

char LedRead[] = {0xA5,0x5A,0x03,0x81,0x03,0x02};
char KeyCode[] = {0xA5,0x5A,0x03,0x80,0x4F,0x01};
char BeepCode[] = {0xA5,0x5A,0x03,0x80,0x02,0xC8};
   Page0 page0;
	 Page1 page1;
	 Page2 page2;
	 Page3 page3;
	 Page4 page4;
	 Page5 page5;
	 Page6 page6;
	 Page7 page7;
	 Page8 page8;
   Page9 page9;
   Page10 page10;
   Page11 page11;
   Page12 page12;
 int main(void)
 {		
	 
	 
	 
	u16 axisDegree = 0;
	delay_init();	   	 	  //延时函数初始化	
   Page* page = 0;
	 Page* pages[] = {&page0,&page1,&page2,&page3,&page4,&page5,&page6,&page7,&page8,&page9,&page10,&page11,&page12};
	 
	 //page = pages[2];
	 //page->handleEnter();
	 
	NVIC_Configuration(); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	UART3_Init(115200,onReceive3,uart3RxBuffer,1024, uart3TxBuffer,1024);	  //串口初始化为115200(调试用)
	UART2_Init(115200,onReceive2,uart2RxBuffer,1024, uart2TxBuffer,1024);	  //串口初始化为115200(调试用)
 	ENCODER_Init(onExitInterupt);
	ext::Led::Init();
	ext::Fpga::Init();
	lcd::reset();
	 
  delay_ms(200);
	LOG_I("HELLO Reset");
	SysTick_Config(1024 * 1024 * 1);
	Setting::initSetting();
	ext::Keyboard::Init();
	
	page = pages[0];
	page->handleEnter();
	Timer2_Init(200,36000,onTimer2Callback,0);
	Timer7_Init(200,36000,onTimer7Callback,0);//0.01 s 
	for(;;)
	{
		 u32 dec,hex;
		 void* param;
		 EventType type;
		 if(ReadEvent(&type,&hex,&dec,&param)){
			 if(type == UART3){
				 LOG_P("send :%d  ",dec);
				 for(int i = 0;i < dec; ++i){
					 LOG_P("0x%x ",((char*)param)[i]);
				 }
				 LOG_P("\r\n");
				 UART2_SendBuffer((u8*)param,dec);
				 /*
				 if(tempBuff3[0] >= '0' && tempBuff3[0] <= '9'){
					 KeyCode[5] = tempBuff3[0] - '0';
					 LOG_P("send :  ");
					 for(int i = 0;i < ARRAY_SIZE(KeyCode); ++i){
						  LOG_P("0x%x ",((char*)KeyCode)[i]);
					 }
				   LOG_P("\r\n");
					 UART2_SendBuffer((u8*)KeyCode,ARRAY_SIZE(KeyCode));
				 }
				 if(tempBuff3[0] == 's'){
					 UART2_SendBuffer((u8*)LedRead,ARRAY_SIZE(LedRead));
				 }
				 if(tempBuff3[0] == 'b'){
					 UART2_SendBuffer((u8*)BeepCode,ARRAY_SIZE(BeepCode));
				 }*/
			 }
			 else if(type == UART2){
				 LOG_P("from lcd :%d  ",dec);
				 for(int i = 0;i < dec; ++i){
					 LOG_P("0x%x ",tempBuff2[i]);
				 }
				 LOG_I("");
				 lcd::parseCodes(tempBuff2,dec);
			 }
			 else if(type == EXITR){
				 LOG_I("interupt %d:0x%x",dec,hex>>6);
				 axisDegree += (hex>>7);
				 //test code, todo
				 if(page == &page12){
					 ((Page12*)page)->handleEncoder(hex>>7);
				 }
				 else if(page == &page6){
					 ((Page6*)page)->handleEncoder(hex>>7);
				 }
				 else if(page == &page10 || page == &page5){
					 ((InputPage*)page)->handleEncoder(hex>>7);
				 }
			 }
			 else if(type == PAGE){
				 int currpage = dec;
				 if(ARRAY_SIZE(pages) <= dec){
					 LOG_E("unknown page %d",dec);
				 }
				 else{
					 if(page != pages[currpage]){
						 page->handleLeave();
						 page = pages[currpage];
						 LOG_I("page %d enter",currpage);
						 page->handleEnter();
					 }
				 }
			 }
			 else if(type == Timer2){
				 page->handleTimer();
			 }
			 else if(type == Timer1){
				 //axisDegree = ext::Fpga::Read(1) << 8 | ext::Fpga::Read(0);
				 //更新主轴角度
				 Setting::setMainAxisAngleInPulse(axisDegree/4);
				 //更新转速
				 Setting::updateRoundPerMin(axisDegree);
			 }
		 }
		 else{
			 ext::ExeCommand cmd = ext::Keyboard::Scan();
			 if(cmd != ext::None){
					LOG_I("cmd %c[0x%x/%d]",cmd,cmd,cmd);
				  page->handleKeyPressed(cmd);
       }
			 delay_ms(200);
		 }
	 }	
}

 