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
//中段接收数据方发
//用于初始化外部串口调试
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
//用于初始化内部串口调试
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
//初始化旋转编码器
void onExitInterupt(char port,char index) {
    char data = GPIO_ReadInputData(GPIOA) & 0xC0;
    PostEvent(EXITR,data,index,0);
}

void onTimer2Callback(void* ) {
    PostEvent(Timer2,0,0,0);
}

void onTimer7Callback(void* ) { // main axis reader	每0.02秒读取一次主轴的数据通过中断发送出去，在main里面接收信息刷新UI
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
    delay_init();//延时
    //初始化所有页面
    Page* page = 0;
    Page* pages[] = {&page0,&page1,&page2,&page3,&page4,&page5,&page6,&page7,&page8,&page9,&page10,&page11,&page12};

    NVIC_Configuration(); //全局芯片的初始化
    UART3_Init(115200,onReceive3,uart3RxBuffer,sizeof(uart3RxBuffer), uart3TxBuffer,sizeof(uart3TxBuffer));	  //外部调试串口初始化
    UART2_Init(115200,onReceive2,uart2RxBuffer,sizeof(uart2RxBuffer), uart2TxBuffer,sizeof(uart2TxBuffer));	  //内部串口相关(键盘,lcd,转盘...)
    ENCODER_Init(onExitInterupt);	 //初始化编码盘，旋转编码器
    ext::Led::Init();				   //  初始化三个灯泡初始化
    ext::Led::SetLed(ext::Led::LED_1,true);	 //灯泡1
    ext::Led::SetLed(ext::Led::LED_2,true);	 //灯泡2
    ext::Led::SetLed(ext::Led::LED_3,true);	 //灯泡3
    ext::Fpga::Init();


	//初始化显示屏
    lcd::reset();
	//测试叫声
	//lcd::BeepON();
	//修改显示器亮度
	//lcd::changeBrightness();
	//延迟
    delay_ms(200);
    LOG_I("Reset");

    SysTick_Config(1024 * 1024 * 1);   //系统时钟的初始化
    Setting::initSetting();	//参数设置 基本参数设置	 初始化
    ext::Keyboard::Init();	  //键盘
    //ext::Led::SetLed(0);

	//初始化选择第一个页面
    page = pages[0];
    page->handleEnter();
    //0.1s per interrupt in 72M osc 计时器,每隔0.1秒刷新一次数据
    Timer2_Init(200,36000,onTimer2Callback,0);
    //0.02 s per interrupt in 72M osc 计时器 ,每隔0.02秒刷新一个主轴信息
    Timer7_Init(40,36000,onTimer7Callback,0);

    FPGA_RESET();  //FPGA芯片重新置位
    ext::Fpga::Write(15,0); //transpant mode
    for(;;)//循环读取消息
    {
        u32 dec,hex; 				   
        void* param;
        EventType type;		   
        if(ReadEvent(&type,&hex,&dec,&param)) {	 //ReadEvent读消息
            if(type == UART3) {
                LOG_P("send :%d  ",dec);   //串口3打印数据
                for(int i = 0; i < dec; ++i) {
                    LOG_P("0x%x ",((char*)param)[i]);
                }
                LOG_P("\r\n");
                UART2_SendBuffer((u8*)param,dec);
            }
            else if(type == UART2) { //data received from UART2(lcd)   根据计时器刷新UI
                lcd::parseCodes(uart2Buff,dec);
            }
            else if(type == EXITR) { //interrupt from Encoder 旋转编码器的改变
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
            else if(type == PAGE) { //刷新UI
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
            else if(type == Timer1) { //定时器,检测主轴脉冲的,放入缓存
                u32 ticks = hex;
                u16 axisDegreeInFpga = dec;
				//将主轴信息放入缓存
                Setting::setMainAxisAngleInPulse(axisDegreeInFpga/4);
                Setting::updateRoundPerMin(ticks,axisDegreeInFpga);
            }
        }
        else {
            ext::ExeCommand cmd = ext::Keyboard::Scan();  //键盘回调
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