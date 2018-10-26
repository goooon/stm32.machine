extern "C" {
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

#define UART_BUFF_SIZE 1024

char uart3Buff[UART_BUFF_SIZE];
//�жν������ݷ���
//���ڳ�ʼ���ⲿ���ڵ���
void onReceive3(void* buff,int len) {
    char* tmp = (char*)buff;
    if(len >= UART_BUFF_SIZE) {
        LOG_W("uart3 buffer size(%d) is not enough : %d",UART_BUFF_SIZE,len);
        len = UART_BUFF_SIZE - 1;
    }
    for(int i = 0; i < len; ++ i) {
        uart3Buff[i] = tmp[i];
    }
    uart3Buff[len] = 0;
    PostEvent(UART3,0,len,uart3Buff);
}

char uart2Buff[UART_BUFF_SIZE];
//���ڳ�ʼ���ڲ����ڵ���
void onReceive2(void* buff,int len) {
    char* tmp = (char*)buff;
    if(len >= UART_BUFF_SIZE) {
        LOG_W("uart2 buffer size(%d) is not enough : %d",UART_BUFF_SIZE,len);
        len = UART_BUFF_SIZE - 1;
    }
    for(int i = 0; i < len; ++ i) {
        uart2Buff[i] = tmp[i];
    }
    uart2Buff[len] = 0;
    PostEvent(UART2,0,len,uart2Buff);
}
//��ʼ����ת������
void onExitInterupt(char port,char index) {
    char data = GPIO_ReadInputData(GPIOA) & 0xC0;
    PostEvent(EXITR,data,index,0);
}

void onTimer2Callback(void* ) {
    PostEvent(Timer2,0,0,0);
}

void onTimer7Callback(void* ) { // main axis reader	ÿ0.02���ȡһ�����������ͨ���жϷ��ͳ�ȥ����main���������Ϣˢ��UI
    Setting::incRoundPerMinTicks(); //ticks every 0.02 second
    u32 axisDegree = (ext::Fpga::Read(1) << 8) | ext::Fpga::Read(0);
    PostEvent(Timer1,Setting::getRoundPerMinTicks(),axisDegree,0);
}

char uart3TxBuffer[UART_BUFF_SIZE];
char uart3RxBuffer[UART_BUFF_SIZE];
char uart2TxBuffer[UART_BUFF_SIZE];
char uart2RxBuffer[UART_BUFF_SIZE];

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
ext::ExeCommand preCmd;
int main(void)
{
    delay_init();//��ʱ
    //��ʼ������ҳ��
    Page* page = 0;
    Page* pages[] = {&page0,&page1,&page2,&page3,&page4,&page5,&page6,&page7,&page8,&page9,&page10,&page11,&page12};

    NVIC_Configuration(); //ȫ��оƬ�ĳ�ʼ��
    UART3_Init(115200,onReceive3,uart3RxBuffer,sizeof(uart3RxBuffer), uart3TxBuffer,sizeof(uart3TxBuffer));	  //�ⲿ���Դ��ڳ�ʼ��
    UART2_Init(115200,onReceive2,uart2RxBuffer,sizeof(uart2RxBuffer), uart2TxBuffer,sizeof(uart2TxBuffer));	  //�ڲ��������(����,lcd,ת��...)
    ENCODER_Init(onExitInterupt);	 //��ʼ�������̣���ת������
    ext::Led::Init();				   //  ��ʼ���������ݳ�ʼ��
    ext::Led::SetLed(ext::Led::LED_1,true);	 //����1
    ext::Led::SetLed(ext::Led::LED_2,true);	 //����2
    ext::Led::SetLed(ext::Led::LED_3,true);	 //����3
    ext::Fpga::Init();


	//��ʼ����ʾ��
    lcd::reset();
	//���Խ���
	//lcd::BeepON();
	//�޸���ʾ������
	//lcd::changeBrightness();
	//�ӳ�
    delay_ms(200);
    LOG_I("Reset");

    SysTick_Config(1024 * 1024 * 1);   //ϵͳʱ�ӵĳ�ʼ��
    Setting::initSetting();	//�������� ������������	 ��ʼ��
    ext::Keyboard::Init();	  //����
    //ext::Led::SetLed(0);

	//��ʼ��ѡ���һ��ҳ��
    page = pages[0];
    page->handleEnter();
    //0.1s per interrupt in 72M osc ��ʱ��,ÿ��0.1��ˢ��һ������
    Timer2_Init(200,36000,onTimer2Callback,0);
    //0.02 s per interrupt in 72M osc ��ʱ�� ,ÿ��0.02��ˢ��һ��������Ϣ
    Timer7_Init(40,36000,onTimer7Callback,0);

    FPGA_RESET();  //FPGAоƬ������λ
    ext::Fpga::Write(15,0); //transpant mode
    for(;;)//ѭ����ȡ��Ϣ
    {
        u32 dec,hex; 				   
        void* param;
        EventType type;		   
        if(ReadEvent(&type,&hex,&dec,&param)) {	 //ReadEvent����Ϣ
            if(type == UART3) {
                LOG_P("send :%d  ",dec);   //����3��ӡ����
                for(int i = 0; i < dec; ++i) {
                    LOG_P("0x%x ",((char*)param)[i]);
                }
                LOG_P("\r\n");
                UART2_SendBuffer((u8*)param,dec);
            }
            else if(type == UART2) { //data received from UART2(lcd)   ���ݼ�ʱ��ˢ��UI
                lcd::parseCodes(uart2Buff,dec);
            }
            else if(type == EXITR) { //interrupt from Encoder ��ת�������ĸı�
                if(page == &page12) {
                    ((Page12*)page)->handleEncoder(hex>>7);
                }
                else if(page == &page6) {
                    ((Page6*)page)->handleEncoder(hex>>7);
                }
                else if(page == &page10 || page == &page5) {
                    ((InputPage*)page)->handleEncoder(hex>>7);
                }
            }
            else if(type == PAGE) { //ˢ��UI
                int currpage = dec;
                if(ARRAY_SIZE(pages) <= dec) {
                    LOG_E("unknown page %d",dec);
                }
                else {
                    if(page != pages[currpage]) {
                        page->handleLeave();
                        page = pages[currpage];
                        LOG_I("page %d enter",currpage);
                        page->handleEnter();
                    }
                }
            }
            else if(type == Timer2) {
                page->handleTimer();
            }
            else if(type == Timer1) { //��ʱ��,������������,���뻺��
                u32 ticks = hex;
                u16 axisDegreeInFpga = dec;
				//��������Ϣ���뻺��
                Setting::setMainAxisAngleInPulse(axisDegreeInFpga/4);
                Setting::updateRoundPerMin(ticks,axisDegreeInFpga);
            }
        }
        else {
            ext::ExeCommand cmd = ext::Keyboard::Scan();  //���̻ص�
            if(cmd != ext::None) {
                if(preCmd != cmd) {
                    LOG_I("cmd %c[0x%x/%d]",cmd,cmd,cmd);
                    page->handleKeyPressed(cmd);
                }
            }
            preCmd = cmd;
            delay_ms(200);
        }
    }
}