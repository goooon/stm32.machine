#include "./Tracer.h"
#include "./Event.h"
#include "./lcd.h"
static char Unicode[128] = {0xA5,0x5A};
void lcd::reset(){
	char Reset[] = {0xA5,0x5A,0x04,0x80,0xEE,0x5A,0xA5};
	UART2_SendBuffer((u8*)Reset,ARRAY_SIZE(Reset));
}
void lcd::readCurrPage(){
	char PageCode[] = {0xA5,0x5A,0x03,0x81,0x03,0x02};
	UART2_SendBuffer((u8*)PageCode,ARRAY_SIZE(PageCode));
}
void lcd::jumpToPage(short page){
	char PageCode[] = {0xA5,0x5A,0x04,0x80,0x03,*(1 + (char*)&page),*(char*)&page};
	UART2_SendBuffer((u8*)PageCode,ARRAY_SIZE(PageCode));
	delay_ms(100);
	readCurrPage();
	PostEvent(PAGE,page,page,0);
	PostEvent(PAGE,page,page,0);
}
void lcd::jumpToPageDirect(short page){
	char PageCode[] = {0xA5,0x5A,0x04,0x80,0x03,*(1 + (char*)&page),*(char*)&page};
	UART2_SendBuffer((u8*)PageCode,ARRAY_SIZE(PageCode));
}
void lcd::parseCodes(char* codes,int len){
	//page 0xa5 0x5a 0x5 0x81 0x3 0x2 0x0 0x5
	if(len == 8 && codes[0] == 0xa5 && codes[1] == 0x5a && codes[2] == 0x5 &&
		codes[3] == 0x81 && codes[4] == 0x03 && codes[5] == 0x2){
		short page;
		*(1 + (char*)&page) = codes[6];
		*(char*)&page = codes[7];
		PostEvent(PAGE,page,page,0);
	}
	else if(len == 9 && codes[0] == 0xa5 && codes[1] == 0x5a && codes[2] == 0x6 &&
		codes[3] == 0x83){
		short code;
		*(1 + (char*)&code) = codes[7];
		*(char*)&code = codes[8];
		PostEvent(KEYCODE,code,code,0);
	}
}
void lcd::sendKeycode(char code){
	char KeyCode[] = {0xA5,0x5A,0x03,0x80,0x4F,code};
	UART2_SendBuffer((u8*)KeyCode,ARRAY_SIZE(KeyCode));
}
void lcd::sendAddrValue(short addr,short value){
	//A55A05820508000F
	char AddrValue[] = {0xA5,0x5A,0x05,0x82,*(1 + (char*)&addr),*(char*)&addr,*(1 + (char*)&value),*(char*)&value};
	UART2_SendBuffer((u8*)AddrValue,ARRAY_SIZE(AddrValue));
}
void lcd::sendAddrValue(short addr,short value1,short value2){
	//A55A05820508000F
	char AddrValue[] = {0xA5,0x5A,0x05,0x82,*(1 + (char*)&addr),*(char*)&addr,*(1 + (char*)&value1),*(char*)&value1,*(1 + (char*)&value2),*(char*)&value2};
	UART2_SendBuffer((u8*)AddrValue,ARRAY_SIZE(AddrValue));
}
void lcd::sendAddrValueLong(short addr,unsigned long value){
	//A55A05820508000F
	char AddrValue[] = {0xA5,0x5A,0x07,0x82,*(1 + (char*)&addr),*(char*)&addr,*(3 + (char*)&value),*(2 + (char*)&value),*(1 + (char*)&value),*(char*)&value};
	UART2_SendBuffer((u8*)AddrValue,ARRAY_SIZE(AddrValue));
}
void lcd::displayUnicode(short addr,short* buff,short len){
	//A55A 09 82 0514 0031006D4F60597D
	int totalLen = len * 2 + 3;
	if(totalLen >= ARRAY_SIZE(Unicode)){
		LOG_E("displayUnicode(%d) too long",len);
		return;
	}
	Unicode[2] = totalLen;
	Unicode[3] = 0x82;
	Unicode[4] = *(1 + (char*)&addr);
	Unicode[5] = *(char*)&addr;
	char* pd = &Unicode[6];
	for(int i = 0;i < len; ++ i){
		*pd = *(1 + (char*)buff);
		pd++;
		*pd = *((char*)buff);
		pd++;
		buff++;
	}
	UART2_SendBuffer((u8*)Unicode,totalLen + 3);
}
void lcd::displayUnicodeQueue(short addr,short* buff,short len){
	//A55A 09 82 0514 0031006D4F60597D
	int totalLen = len * 2 + 3;
	
	if(totalLen >= ARRAY_SIZE(Unicode)){
		LOG_E("displayUnicode(%d) too long",len);
		return;
	}
	Unicode[2] = totalLen;
	Unicode[3] = 0x82;
	Unicode[4] = *(1 + (char*)&addr);
	Unicode[5] = *(char*)&addr;
	char* pd = &Unicode[6];
	for(int i = 0;i < len; ++ i){
		*pd = *(1 + (char*)buff);
		pd++;
		*pd = *((char*)buff);
		pd++;
		buff++;
	}
	UART2_SendBuffer((u8*)Unicode,totalLen + 3);
}