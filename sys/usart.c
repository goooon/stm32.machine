#include "sys.h"
#include "usart.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
    USART_SendData(USART1,(uint8_t)ch);   
	return ch;
}
#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART1_RX_STA=0;       //接收状态标记	  

//初始化IO 串口1 
//bound:波特率
void UART1_Init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART1);  //复位串口1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10


   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
#if EN_USART1_RX		  //如果使能了接收  
   //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
   
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
#endif
    USART_Cmd(USART1, ENABLE);                    //使能串口 

}


#endif	

typedef struct {
	USART_TypeDef*   port;
	OnReceive onReceive;
	OnSendOver onSendOver;
	char* pTxBuf;
	char* pRxBuf;
	int   usTxBufSize;
	int   usRxBufSize;
	int   usTxWrite;
	int   usTxRead;
	int   usRxWrite;
	int   usRxRead;
	unsigned int ticksPrev;
}UART_HANDLE;

UART_HANDLE hUart1;
UART_HANDLE hUart2;
UART_HANDLE hUart3;
extern unsigned int GetSysTickCounter(void);  

//初始化IO 串口1 
//bound:波特率
void UART2_Init(u32 bound,OnReceive onReceive,char* rxBuff,int rxSize,char* txBuff,int txSize){
    //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART1，
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA时钟
	USART_DeInit(USART2);  //复位串口1
	 //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3


   //USART 初始化设置

	   USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	   USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	   USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	   USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口
  hUart2.port = USART2;
	hUart2.onReceive = onReceive;
	hUart2.onSendOver = 0;
	hUart2.pRxBuf = rxBuff;
	hUart2.pTxBuf = txBuff;
	hUart2.usRxBufSize = rxSize;
	hUart2.usTxBufSize = txSize;
	hUart2.usTxRead = 0;
	hUart2.usTxWrite = 0;
	hUart2.usRxRead = rxSize;
	hUart2.usRxWrite = 0;
	hUart2.ticksPrev = 0;
	if(onReceive){   
		 //Usart1 NVIC 配置
			NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//子优先级3
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
			NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		 
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
	}
    USART_Cmd(USART2, ENABLE);                    //使能串口 
}


void CheckUartBuffer(UART_HANDLE* uart,unsigned int ticks){
	if(uart->usRxWrite != 0){
		if(ticks - uart->ticksPrev > 5){
			if (uart->onReceive)
      {
          (*uart->onReceive)(uart->pRxBuf,uart->usRxWrite);
					uart->usRxWrite = 0;
      }
			uart->ticksPrev = ticks;
		}
	}
}

void CheckUartBufferIRQ(unsigned int ticks){
	CheckUartBuffer(&hUart2,ticks);
	CheckUartBuffer(&hUart3,ticks);
}

void UartIRQ(UART_HANDLE *_pUart)  
{
    if (USART_GetITStatus(_pUart->port, USART_IT_RXNE) != RESET)
    {
        /* Read one byte from the receive data register */
        _pUart->pRxBuf[_pUart->usRxWrite] = USART_ReceiveData(_pUart->port);
        if (++_pUart->usRxWrite >= _pUart->usRxBufSize)
        {//fifo read rewind
            _pUart->usRxWrite = 0;
        }
        if ( _pUart->usRxWrite == _pUart->usRxRead )
        {
            if (_pUart->onReceive)
            {
                (*_pUart->onReceive)(_pUart->pRxBuf,_pUart->usRxWrite);
							  _pUart->usRxWrite = 0;
            }
        }
				_pUart->ticksPrev = GetSysTickCounter();
    }  
  
    /* sending buffer interrupt,TXEmpty*/
    if (USART_GetITStatus(_pUart->port, USART_IT_TXE) != RESET)  
    {     
        if (_pUart->usTxRead == _pUart->usTxWrite)  
        {  
            /* disable TX interrupt,enable it after TX done */  
            USART_ITConfig(_pUart->port, USART_IT_TXE, DISABLE);  
            USART_ITConfig(_pUart->port, USART_IT_TC, ENABLE);  
        }   
        else  
        {         
            /* Write one byte to the transmit data register */  
            USART_SendData(_pUart->port, _pUart->pTxBuf[_pUart->usTxRead]);                      
            if (++_pUart->usTxRead >= _pUart->usTxBufSize)  
            {//fifo write rewind
                _pUart->usTxRead = 0;  
            }  
        }  
     
    }  
    /* interrupted by one char sent out*/  
    else if (USART_GetITStatus(_pUart->port, USART_IT_TC) != RESET)  
    {  
        if (_pUart->usTxRead == _pUart->usTxWrite)  
        {  
            /* disable sending buffer interrupt,enable it when TX done*/  
            USART_ITConfig(_pUart->port, USART_IT_TC, DISABLE);  
                
            if (_pUart->onSendOver)  
            {  
                (*_pUart->onSendOver)();    
            }  
        }   
        else  
        {         
            /* Write one byte to the transmit data register */  
            USART_SendData(_pUart->port, _pUart->pTxBuf[_pUart->usTxRead]);                      
            if (++_pUart->usTxRead >= _pUart->usTxBufSize)  
            {//fifo write rewind
                _pUart->usTxRead = 0;  
            }  
        }  
    }  
}  

void USART1_IRQHandler(void)  
{  
    UartIRQ(&hUart1);
}  
void USART2_IRQHandler(void)  
{  
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();    
#endif
    UartIRQ(&hUart2);
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
}  
void USART3_IRQHandler(void)  
{  
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();    
#endif
    UartIRQ(&hUart3);
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
}  
void UART3_Init(u32 bound,OnReceive onReceive,char* rxBuff,int rxSize,char* txBuff,int txSize){
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART3);  //复位串口1
	 //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PB10
   
    //USART3_RX	  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PB11

   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART3, &USART_InitStructure); //初始化串口
	hUart3.port = USART3;
	hUart3.onReceive = onReceive;
	hUart3.onSendOver = 0;
	hUart3.pRxBuf = rxBuff;
	hUart3.pTxBuf = txBuff;
	hUart3.usRxBufSize = rxSize;
	hUart3.usTxBufSize = txSize;
	hUart3.usTxRead = 0;
	hUart3.usTxWrite = 0;
	hUart3.usRxRead = rxSize;
	hUart3.usRxWrite = 0;
	hUart3.ticksPrev = 0;
if(onReceive){ 
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
   
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
    }
    USART_Cmd(USART3, ENABLE);                    //使能串口 
}

/*******************************************************************************
* 函数名  : UART3_SendString
* 描述    : USART3发送字符串
* 输入    : *s字符串指针
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/

#define USART_ITConfigTxAll(port,enable)   \
USART_ITConfig(port, USART_IT_PE, enable); \
USART_ITConfig(port, USART_IT_TC, enable); \
USART_ITConfig(port, USART_IT_ERR, enable);\
USART_ITConfig(port, USART_IT_NE, enable); 

int UART_SendString(UART_HANDLE* uart, const char* string) {
	int length = 0;
	if (uart->usTxBufSize == 0) {
		while (*string != '\0')//检测字符串结束符
		{
			while (USART_GetFlagStatus(uart->port, USART_FLAG_TC) == RESET);
			USART_SendData(uart->port, *string);//发送当前字符
			length ++;
			string ++;
		}
		while (USART_GetFlagStatus(uart->port, USART_FLAG_TC) == RESET);
	}
	else {
		u16 i;
		u8 ucNew = 0;

		USART_ITConfigTxAll(uart->port,DISABLE);
		if (uart->usTxWrite == uart->usTxRead)
		{
			ucNew = 1;
		}
		USART_ITConfigTxAll(uart->port,ENABLE);

		for(;;)
		{
			if (*string == 0)break;
			uart->pTxBuf[uart->usTxWrite] = *string;

			USART_ITConfigTxAll(uart->port,DISABLE);
			if (++uart->usTxWrite >= uart->usTxBufSize)
			{
				uart->usTxWrite = 0;
			}
			USART_ITConfigTxAll(uart->port,ENABLE);
			
			string++;
			length ++;
		};

		if (ucNew == 1)
		{
			USART_ITConfig(uart->port, USART_IT_TXE, ENABLE);
		}
	}
	return length;
}
int UART_SendBuffer(UART_HANDLE* uart, u8* buff, u16 length) {
	if (uart->usTxBufSize == 0) {
		while (length != 0)//检测字符串结束符
		{
			length--;
			while (USART_GetFlagStatus(uart->port, USART_FLAG_TC) == RESET);
			USART_SendData(uart->port, *buff);//发送当前字符
			buff++;
		}
		while (USART_GetFlagStatus(uart->port, USART_FLAG_TC) == RESET);
	}
	else {
		u16 i;
		u8 ucNew = 0;

		USART_ITConfigTxAll(uart->port,DISABLE);
		if (uart->usTxWrite == uart->usTxRead)
		{
			ucNew = 1;
		}
		USART_ITConfigTxAll(uart->port,ENABLE);

		for (i = 0; i < length; i++)
		{
			uart->pTxBuf[uart->usTxWrite] = buff[i];

			USART_ITConfigTxAll(uart->port,DISABLE);
			if (++uart->usTxWrite >= uart->usTxBufSize)
			{//fifo write
				uart->usTxWrite = 0;
			}
			USART_ITConfigTxAll(uart->port,ENABLE);
		}

		if (ucNew == 1)
		{
			USART_ITConfig(uart->port, USART_IT_TXE, ENABLE);
		}
	}
	return length;
}
int UART3_SendString(char* s)
{
	return UART_SendString(&hUart3,s);
}
int UART3_SendBuffer(u8 *buff, u16 length)
{  
	return UART_SendBuffer(&hUart3,buff,length);
} 
int UART2_SendString(char* s)
{
	return UART_SendString(&hUart2,s);
}
int UART2_SendBuffer(u8 *buff, u16 length)
{  
	return UART_SendBuffer(&hUart2,buff,length);
}
