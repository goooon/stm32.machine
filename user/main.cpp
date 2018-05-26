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
	
	FPGA_RESET();
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


/* ???? - mian.c */  
/** 
  ****************************************************************************** 
  * @file    main.c 
  * @author  fire 
  * @version V1.0 
  * @date    2013-xx-xx 
  * @brief   I2C EEPROM(AT24C02)??,??????USART1?????????? 
  ****************************************************************************** 
  */  
    
#include "stm32f10x.h"  
#include <string.h>  
  
#define  EEP_Firstpage      0x00  
u8 I2c_Buf_Write[256];  
u8 I2c_Buf_Read[256];  
/* EEPROM Addresses defines */  
#define EEPROM_Block0_ADDRESS   0xA0 /* E2 = 0 */  
//#define EEPROM_Block1_ADDRESS 0xA2 /* E2 = 0 */  
//#define EEPROM_Block2_ADDRESS 0xA4 /* E2 = 0 */  
//#define EEPROM_Block3_ADDRESS 0xA6 /* E2 = 0 */  
  
void I2C_EE_Init(void);  
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite);  
void I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr);  
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite);  
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);  
void I2C_EE_WaitEepromStandbyState(void);  
void I2C_Test(void);  
  
/** 
  * @brief  ??? 
  * @param  ? 
  * @retval ? 
  */  
int main2(void)  
{   
    
      
    //printf("\r\n ????I2C??(AT24C02)?????? \r\n");  
    //LED_GPIO_Config();  
  
  
    //printf("\r\n ????I2C??(AT24C02)?????? \r\n");     
    I2C_EE_Init(); 
    I2C_Test();  
    
  while (1)  
  {        
  }  
}  
  

/** 
  * @brief  I2C(AT24C02)???? 
  * @param  ? 
  * @retval ? 
  */  
void I2C_Test(void)  
{  
    u16 i;  
  
  
    //printf("?????\n\r");  
      
    for ( i=0; i<=255; i++ ) // ????  
		{                              
    I2c_Buf_Write[i] = i;  
  
  
    //printf("0x%02X ", I2c_Buf_Write[i]);  
			if(i%16 == 15)     { 
        //printf("\n\r");  
			}				
		}  
  
    //?I2c_Buf_Write??????????EERPOM?   
    //LED1(ON);  
    I2C_EE_BufferWrite (I2c_Buf_Write, EEP_Firstpage, 256);  
    //LED1(OFF);     
    
  //printf("\n\r???\n\r");  
     
   //printf("\n\r?????\n\r");  
  //?EEPROM?????????I2c_Buf_Read?  
    //LED2(ON);     
    I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, 256);   
   //LED2(OFF);  
     
  //?I2c_Buf_Read??????????  
    for (i=0; i<256; i++)  
    {     
        if(I2c_Buf_Read[i] != I2c_Buf_Write[i])  
        {  
            //printf("0x%02X ", I2c_Buf_Read[i]);  
            //printf("??:I2C EEPROM???????????\n\r");  
            return;  
        }  
				//printf("0x%02X ", I2c_Buf_Read[i]);  
				if(i%16 == 15){
					//printf("\n\r");  
				}
      
    }  
  //printf("I2C(AT24C02)??????\n\r");  
}  
  

  
  
//--------------------------------------------------------  
/** 
  ****************************************************************************** 
  * @file    bsp_i2c_ee.c 
  * @author  STMicroelectronics 
  * @version V1.0 
  * @date    2013-xx-xx 
  * @brief   i2c EEPROM(AT24C02)????bsp 
  ****************************************************************************** 
  */   
  
/* STM32 I2C ???? */  
#define I2C_Speed              400000  
  
/* ???????STM32???I2C????????? */  
#define I2C1_OWN_ADDRESS7      0X0A     
  
/* AT24C01/02???8??? */  
#define I2C_PageSize           8  
  
/* AT24C04/08A/16A???16??? */  
//#define I2C_PageSize           16           
  
int EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;  

void I2C_GPIO_Config(void)
{  
  GPIO_InitTypeDef  GPIO_InitStructure;   
  
  /* ??? I2C1 ????? */  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);    
      
  /* PB6-I2C1_SCL?PB7-I2C1_SDA*/  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;          // ????  
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
}  
  
/** 
  * @brief  I2C ?????? 
  * @param  ? 
  * @retval ? 
  */  
static void I2C_Mode_Configu(void)  
{  
  I2C_InitTypeDef  I2C_InitStructure;   
  
  /* I2C ?? */  
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;  
      
  /* ???????,??????? SCL ??????? */  
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;  
      
  I2C_InitStructure.I2C_OwnAddress1 = I2C1_OWN_ADDRESS7;   
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;  
      
  /* I2C????? */  
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;  
      
  /* ???? */  
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;  
    
  /* I2C1 ??? */  
  I2C_Init(I2C1, &I2C_InitStructure);  
    
  /* ?? I2C1 */  
  I2C_Cmd(I2C1, ENABLE);     
}  
 
void I2C_EE_Init(void)
{  
  
  I2C_GPIO_Config();   
   
  I2C_Mode_Configu();  
  
/* ?????i2c_ee.h???????EEPROM?????? */  
#ifdef EEPROM_Block0_ADDRESS  
  /* ?? EEPROM Block0 ??? */  
  EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;  
#endif  
  
#ifdef EEPROM_Block1_ADDRESS    
    /* ?? EEPROM Block1 ??? */  
  EEPROM_ADDRESS = EEPROM_Block1_ADDRESS;  
#endif  
  
#ifdef EEPROM_Block2_ADDRESS    
    /* ?? EEPROM Block2 ??? */  
  EEPROM_ADDRESS = EEPROM_Block2_ADDRESS;  
#endif  
  
#ifdef EEPROM_Block3_ADDRESS    
    /* ?? EEPROM Block3 ??? */  
  EEPROM_ADDRESS = EEPROM_Block3_ADDRESS;  
#endif  
}  
  
/** 
  * @brief   ??????????I2C EEPROM? 
  * @param    
  *     @arg pBuffer:????? 
  *     @arg WriteAddr:??? 
  *     @arg NumByteToWrite:????? 
  * @retval  ? 
  */  
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite)  
{  
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;  
  
  Addr = WriteAddr % I2C_PageSize;              // addr = 8bit % 8  
  count = I2C_PageSize - Addr;                  // 8 - addr  
  NumOfPage =  NumByteToWrite / I2C_PageSize;  
  NumOfSingle = NumByteToWrite % I2C_PageSize;  
   
  /* If WriteAddr is I2C_PageSize aligned  */  
  if(Addr == 0)   
  {  
    /* If NumByteToWrite < I2C_PageSize */  
    if(NumOfPage == 0)   
    {  
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);  
      I2C_EE_WaitEepromStandbyState();  
    }  
    /* If NumByteToWrite > I2C_PageSize */  
    else    
    {  
      while(NumOfPage--)  
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);   
        I2C_EE_WaitEepromStandbyState();  
        WriteAddr +=  I2C_PageSize;  
        pBuffer += I2C_PageSize;  
      }  
  
      if(NumOfSingle!=0)  
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);  
        I2C_EE_WaitEepromStandbyState();  
      }  
    }  
  }  
  /* If WriteAddr is not I2C_PageSize aligned  */  
  else   
  {  
    /* If NumByteToWrite < I2C_PageSize */  
    if(NumOfPage== 0)   
    {  
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);  
      I2C_EE_WaitEepromStandbyState();  
    }  
    /* If NumByteToWrite > I2C_PageSize */  
    else  
    {  
      NumByteToWrite -= count;  
      NumOfPage =  NumByteToWrite / I2C_PageSize;  
      NumOfSingle = NumByteToWrite % I2C_PageSize;    
        
      if(count != 0)  
      {    
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);  
        I2C_EE_WaitEepromStandbyState();  
        WriteAddr += count;  
        pBuffer += count;  
      }   
        
      while(NumOfPage--)  
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);  
        I2C_EE_WaitEepromStandbyState();  
        WriteAddr +=  I2C_PageSize;  
        pBuffer += I2C_PageSize;    
      }  
      if(NumOfSingle != 0)  
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);   
        I2C_EE_WaitEepromStandbyState();  
      }  
    }  
  }    
}  
  
/** 
  * @brief   ??????I2C EEPROM? 
  * @param    
  *     @arg pBuffer:????? 
  *     @arg WriteAddr:???  
  * @retval  ? 
  */  
void I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr)  
{  
  /* Send STRAT condition */  
  I2C_GenerateSTART(I2C1, ENABLE);  
  
  /* Test on EV5 and clear it */  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));    
  
  /* Send EEPROM address for write */  
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);  
    
  /* Test on EV6 and clear it */  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  
        
  /* Send the EEPROM's internal address to write to */  
  I2C_SendData(I2C1, WriteAddr);  
    
  /* Test on EV8 and clear it */  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));  
  
  /* Send the byte to be written */  
  I2C_SendData(I2C1, *pBuffer);   
     
  /* Test on EV8 and clear it */  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));  
    
  /* Send STOP condition */  
  I2C_GenerateSTOP(I2C1, ENABLE);  
}  
  
/** 
  * @brief   ?EEPROM??????????????,????????? 
  *          ????EEPROM????,AT24C02???8??? 
  * @param    
  *     @arg pBuffer:????? 
  *     @arg WriteAddr:??? 
  *     @arg NumByteToWrite:????? 
  * @retval  ? 
  */  
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)  
{  
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); // Added by Najoua 27/08/2008  
      
  /* ??start?? */  
  I2C_GenerateSTART(I2C1, ENABLE);  
    
  /* Test on EV5 and clear it */  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));   
    
  /* Send EEPROM address for write */  
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);  
    
  /* Test on EV6 and clear it */  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));    
  
  /* Send the EEPROM's internal address to write to */      
  I2C_SendData(I2C1, WriteAddr);    
  
  /* Test on EV8 and clear it */  
  while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));  
  
  /* While there is data to be written */  
  while(NumByteToWrite--)    
  {  
    /* Send the current byte */  
    I2C_SendData(I2C1, *pBuffer);   
  
    /* Point to the next byte to be written */  
    pBuffer++;   
    
    /* Test on EV8 and clear it */  
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));  
  }  
  
  /* Send STOP condition */  
  I2C_GenerateSTOP(I2C1, ENABLE);  
}  
  
/** 
  * @brief   ?EEPROM????????  
  * @param    
  *     @arg pBuffer:???EEPROM??????????? 
  *     @arg WriteAddr:?????EEPROM??? 
  *     @arg NumByteToWrite:??EEPROM?????? 
  * @retval  ? 
  */  
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)  
{    
  //*((u8 *)0x4001080c) |=0x80;   
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); // Added by Najoua 27/08/2008      
      
  /* Send START condition */  
  I2C_GenerateSTART(I2C1, ENABLE);  
  //*((u8 *)0x4001080c) &=~0x80;  
    
  /* Test on EV5 and clear it */  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));  
  
  /* Send EEPROM address for write */  
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);  
  
  /* Test on EV6 and clear it */  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  
    
  /* Clear EV6 by setting again the PE bit */  
  I2C_Cmd(I2C1, ENABLE);  
  
  /* Send the EEPROM's internal address to write to */  
  I2C_SendData(I2C1, ReadAddr);    
  
  /* Test on EV8 and clear it */  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));  
    
  /* Send STRAT condition a second time */    
  I2C_GenerateSTART(I2C1, ENABLE);  
    
  /* Test on EV5 and clear it */  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));  
    
  /* Send EEPROM address for read */  
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Receiver);  
    
  /* Test on EV6 and clear it */  
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));  
    
  /* While there is data to be read */  
  while(NumByteToRead)    
  {  
    if(NumByteToRead == 1)  
    {  
      /* Disable Acknowledgement */  
      I2C_AcknowledgeConfig(I2C1, DISABLE);  
        
      /* Send STOP Condition */  
      I2C_GenerateSTOP(I2C1, ENABLE);  
    }  
  
    /* Test on EV7 and clear it */  
    if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))    
    {        
      /* Read a byte from the EEPROM */  
      *pBuffer = I2C_ReceiveData(I2C1);  
  
      /* Point to the next location where the byte read will be saved */  
      pBuffer++;   
        
      /* Decrement the read bytes counter */  
      NumByteToRead--;          
    }     
  }  
  
  /* Enable Acknowledgement to be ready for another reception */  
  I2C_AcknowledgeConfig(I2C1, ENABLE);  
}  
  
/** 
  * @brief  Wait for EEPROM Standby state  
  * @param  ? 
  * @retval ? 
  */  
void I2C_EE_WaitEepromStandbyState(void)        
{  
  vu16 SR1_Tmp = 0;  
  
  do  
  {  
    /* Send START condition */  
    I2C_GenerateSTART(I2C1, ENABLE);  
    /* Read I2C1 SR1 register */  
    SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);  
    /* Send EEPROM address for write */  
    I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);  
  }while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));  
    
  /* Clear AF flag */  
  I2C_ClearFlag(I2C1, I2C_FLAG_AF);  
    /* STOP condition */      
    I2C_GenerateSTOP(I2C1, ENABLE);   
}  


 