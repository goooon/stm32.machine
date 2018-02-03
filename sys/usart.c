#include "sys.h"
#include "usart.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
    USART_SendData(USART1,(uint8_t)ch);   
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
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
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART1_RX_BUF[USART1_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART1_RX_STA=0;       //����״̬���	  

//��ʼ��IO ����1 
//bound:������
void UART1_Init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART1);  //��λ����1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10


   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������
#if EN_USART1_RX		  //���ʹ���˽���  
   //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
#endif
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

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

//��ʼ��IO ����1 
//bound:������
void UART2_Init(u32 bound,OnReceive onReceive,char* rxBuff,int rxSize,char* txBuff,int txSize){
    //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART1��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
	USART_DeInit(USART2);  //��λ����1
	 //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3


   //USART ��ʼ������

	   USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	   USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	   USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	   USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART2, &USART_InitStructure); //��ʼ������
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
		 //Usart1 NVIC ����
			NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//�����ȼ�3
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
			NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
		 
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
	}
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
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
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
    UartIRQ(&hUart2);
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
}  
void USART3_IRQHandler(void)  
{  
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
    UartIRQ(&hUart3);
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
}  
void UART3_Init(u32 bound,OnReceive onReceive,char* rxBuff,int rxSize,char* txBuff,int txSize){
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART3);  //��λ����1
	 //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��PB10
   
    //USART3_RX	  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //��ʼ��PB11

   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART3, &USART_InitStructure); //��ʼ������
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
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
    }
    USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 
}

/*******************************************************************************
* ������  : UART3_SendString
* ����    : USART3�����ַ���
* ����    : *s�ַ���ָ��
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/

#define USART_ITConfigTxAll(port,enable)   \
USART_ITConfig(port, USART_IT_PE, enable); \
USART_ITConfig(port, USART_IT_TC, enable); \
USART_ITConfig(port, USART_IT_ERR, enable);\
USART_ITConfig(port, USART_IT_NE, enable); 

int UART_SendString(UART_HANDLE* uart, const char* string) {
	int length = 0;
	if (uart->usTxBufSize == 0) {
		while (*string != '\0')//����ַ���������
		{
			while (USART_GetFlagStatus(uart->port, USART_FLAG_TC) == RESET);
			USART_SendData(uart->port, *string);//���͵�ǰ�ַ�
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
		while (length != 0)//����ַ���������
		{
			length--;
			while (USART_GetFlagStatus(uart->port, USART_FLAG_TC) == RESET);
			USART_SendData(uart->port, *buff);//���͵�ǰ�ַ�
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
