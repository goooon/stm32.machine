#include "Keyboard.h"
extern "C" {
#include "../sys/delay.h"
#include "../sys/usart.h"
}
//键盘模块
using namespace ext;
uint16_t Keyboard::key_prev = 0;
ScanLine scanLines[] = {//定义了键盘扫描行的数据值（阵列键盘行为7列为4）
    {GPIOE,0x0800,0},
    {GPIOE,0x0400,1},
    {GPIOG,0x0002,2},
    {GPIOG,0x0001,3},
    {GPIOF,0x8000,4},
    {GPIOF,0x4000,5},
    {GPIOF,0x2000,6}
};
//{点,0,确定,null,开始,下,停止}
const ExeCommand row0[] = {Numb_Dot,Numb_0,CMD_Enter,None,CMD_Start,CMD_Dn,CMD_Stop};
//{7,8,9,删除,null,输入,null}
const ExeCommand row1[] = {Numb_7,Numb_8,Numb_9,CMD_Backspace,None,CMD_Input,None};
//{4,5,6,重复测量,左,设置,右}
const ExeCommand row2[] = {Numb_4,Numb_5,Numb_6,CMD_RepeatMeas,CMD_Left,CMD_Setting,CMD_Right};
//{1,2,3,测量,手动修正,上,进刀修正}
const ExeCommand row3[] = {Numb_1,Numb_2,Numb_3,CMD_Measure,CMD_WheelFix,CMD_Up,CMD_KnifeFix};
//初始化键盘
bool Keyboard::Init() {	//键盘的初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	  //设置PE口打开时钟
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOE, DISABLE);	  //关闭PE口失能

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = INPinsE;//那几个IO口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//下拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;//配置IO的速度
    GPIO_Init(GPIOE, &GPIO_InitStructure);//GPIO_Init(系统内部函数）配置IO

    GPIO_InitStructure.GPIO_Pin = OutPinsE;	   //那几个IO口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //设置为推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	 //配置IO的速度
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE); //设置PG口打开时钟
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOG, DISABLE); //关闭Pg口失能

    GPIO_InitStructure.GPIO_Pin = OutPinsG;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;////设置为推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOF, DISABLE);

    GPIO_InitStructure.GPIO_Pin = OutPinsF;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//设置为推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB, DISABLE);

    GPIO_InitStructure.GPIO_Pin = INReturnPins;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//就是浮空输入	 (浮空不就是不接上拉，或者下拉么)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//这个IO口设计是用于引出方便后面使用（就是第一版设计吊的返回按键使用）
    LOG_I("keyboard Inited()");

    GPIO_SetBits(GPIOE,0x0C00);//PE11,PE10,	//输出高电平
    GPIO_SetBits(GPIOG,0x0003);//PG1,PG0	//输出高电平
    GPIO_SetBits(GPIOF,0xE000);//PF15,PF14,PF13 //输出高电平
    return true;
}
//判断点击按键
uint16_t Keyboard::SubScanCol(uint16_t row) {  //阵列键盘的列扫描函数
    uint16_t col = 0xFFFF;
    GPIO_ResetBits(GPIOE,0x0C00);//PE11,PE10, //输出低电平
    GPIO_ResetBits(GPIOG,0x0003);//PG1,PG0	//输出低电平
    GPIO_ResetBits(GPIOF,0xE000);//PF15,PF14,PF13  //输出低电平
    for(int i = 0; i < ARRAY_SIZE(scanLines); ++ i) {//ARRAY_SIZE（scanLines）获取数组长度
        GPIO_SetBits(scanLines[i].GPIOx,scanLines[i].mask);	 //一次把这几个IO口拉高(scanLines)上面设置了数组
        uint16_t read = GPIO_ReadInputData(GPIOE) & 0xF000;	 //这个方法读取了PE口的数据（这里的& 0xF000只获取了高4位的数据）
        if(read == row) {//当Pe口读出的数据等于本函数传递进入的row数字的时候
            col = scanLines[i].col;	//col = 结构体重 col的数字只可能0~6
            break;
        }
    }
    GPIO_SetBits(GPIOE,0x0C00);//PE11,PE10,	 	//又把7路IO口的电平设置为高
    GPIO_SetBits(GPIOG,0x0003);//PG1,PG0		//又把7路IO口的电平设置为高
    GPIO_SetBits(GPIOF,0xE000);//PF15,PF14,PF13	//又把7路IO口的电平设置为高
    return col;
}

const static uint16_t scanedRows[] = {0x1000,0x2000,0x4000,0x8000};//读取阵列键盘列的高四位（这个代码有点瓜）
 //键盘回调
ExeCommand ext::Keyboard::Scan() { //阵列键盘的行扫描函数
    ExeCommand cmd = None;//
    uint16_t keyDown = GPIO_ReadInputData(GPIOB) & 0x8000;//检测返回键
    if(keyDown == 0) {//检测到按下了返回键
        if(key_prev == CMD_FanHui) {//？？？？？比较按键的值是否是返回键（但是这里key_prev应该等于0？？？？？？）
            return None;//如果相等返回空
        }
        delay_ms(10); //
        keyDown = GPIO_ReadInputData(GPIOB) & 0x8000;//检测返回键
        if(keyDown == 0) {//检测到按下了返回键
            LOG_I("Fan Hui Returned");
            key_prev = CMD_FanHui;
            return CMD_FanHui;
        }
    }
    else if(key_prev == CMD_FanHui) {
        key_prev = None;
    }

    uint16_t row = ext::Keyboard::ScanRow() & 0xF000;//读取列的数据
    if(row != key_prev) {
        if(row == 0x1000 || row == 0x2000 || row == 0x4000 || row == 0x8000) {//检测到列的任何一个IO口电频为高-----（其实这里可以写成 if （row |= 0x0000））
            uint16_t col = ext::Keyboard::SubScanCol(row);
            LOG_I("key[0x%04x][0x%x]",row,col);
            if(col == 0xFFFF) {
                LOG_E("Error col 0x%x",col);
                return None;
            }
            key_prev = row;
            if(row == 0x1000)return row0[col];	//这个地方应该写成case语句
            if(row == 0x2000)return row1[col];
            if(row == 0x4000)return row2[col];
            if(row == 0x8000)return row3[col];
        }
        else if((row & key_prev) == 0) {
            LOG_I("key 0x%x up",key_prev);
            key_prev = 0;
        }
        else {
            LOG_W("more than one key is pressed 0x%x",row);
        }
    }
    return cmd;
}