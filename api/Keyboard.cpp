#include "Keyboard.h"
extern "C" {
#include "../sys/delay.h"
#include "../sys/usart.h"
}
//����ģ��
using namespace ext;
uint16_t Keyboard::key_prev = 0;
ScanLine scanLines[] = {//�����˼���ɨ���е�����ֵ�����м�����Ϊ7��Ϊ4��
    {GPIOE,0x0800,0},
    {GPIOE,0x0400,1},
    {GPIOG,0x0002,2},
    {GPIOG,0x0001,3},
    {GPIOF,0x8000,4},
    {GPIOF,0x4000,5},
    {GPIOF,0x2000,6}
};
//{��,0,ȷ��,null,��ʼ,��,ֹͣ}
const ExeCommand row0[] = {Numb_Dot,Numb_0,CMD_Enter,None,CMD_Start,CMD_Dn,CMD_Stop};
//{7,8,9,ɾ��,null,����,null}
const ExeCommand row1[] = {Numb_7,Numb_8,Numb_9,CMD_Backspace,None,CMD_Input,None};
//{4,5,6,�ظ�����,��,����,��}
const ExeCommand row2[] = {Numb_4,Numb_5,Numb_6,CMD_RepeatMeas,CMD_Left,CMD_Setting,CMD_Right};
//{1,2,3,����,�ֶ�����,��,��������}
const ExeCommand row3[] = {Numb_1,Numb_2,Numb_3,CMD_Measure,CMD_WheelFix,CMD_Up,CMD_KnifeFix};
//��ʼ������
bool Keyboard::Init() {	//���̵ĳ�ʼ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	  //����PE�ڴ�ʱ��
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOE, DISABLE);	  //�ر�PE��ʧ��

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = INPinsE;//�Ǽ���IO��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;//����IO���ٶ�
    GPIO_Init(GPIOE, &GPIO_InitStructure);//GPIO_Init(ϵͳ�ڲ�����������IO

    GPIO_InitStructure.GPIO_Pin = OutPinsE;	   //�Ǽ���IO��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //����Ϊ�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	 //����IO���ٶ�
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE); //����PG�ڴ�ʱ��
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOG, DISABLE); //�ر�Pg��ʧ��

    GPIO_InitStructure.GPIO_Pin = OutPinsG;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;////����Ϊ�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOF, DISABLE);

    GPIO_InitStructure.GPIO_Pin = OutPinsF;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//����Ϊ�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB, DISABLE);

    GPIO_InitStructure.GPIO_Pin = INReturnPins;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//���Ǹ�������	 (���ղ����ǲ�����������������ô)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//���IO����������������������ʹ�ã����ǵ�һ����Ƶ��ķ��ذ���ʹ�ã�
    LOG_I("keyboard Inited()");

    GPIO_SetBits(GPIOE,0x0C00);//PE11,PE10,	//����ߵ�ƽ
    GPIO_SetBits(GPIOG,0x0003);//PG1,PG0	//����ߵ�ƽ
    GPIO_SetBits(GPIOF,0xE000);//PF15,PF14,PF13 //����ߵ�ƽ
    return true;
}
//�жϵ������
uint16_t Keyboard::SubScanCol(uint16_t row) {  //���м��̵���ɨ�躯��
    uint16_t col = 0xFFFF;
    GPIO_ResetBits(GPIOE,0x0C00);//PE11,PE10, //����͵�ƽ
    GPIO_ResetBits(GPIOG,0x0003);//PG1,PG0	//����͵�ƽ
    GPIO_ResetBits(GPIOF,0xE000);//PF15,PF14,PF13  //����͵�ƽ
    for(int i = 0; i < ARRAY_SIZE(scanLines); ++ i) {//ARRAY_SIZE��scanLines����ȡ���鳤��
        GPIO_SetBits(scanLines[i].GPIOx,scanLines[i].mask);	 //һ�ΰ��⼸��IO������(scanLines)��������������
        uint16_t read = GPIO_ReadInputData(GPIOE) & 0xF000;	 //���������ȡ��PE�ڵ����ݣ������& 0xF000ֻ��ȡ�˸�4λ�����ݣ�
        if(read == row) {//��Pe�ڶ��������ݵ��ڱ��������ݽ����row���ֵ�ʱ��
            col = scanLines[i].col;	//col = �ṹ���� col������ֻ����0~6
            break;
        }
    }
    GPIO_SetBits(GPIOE,0x0C00);//PE11,PE10,	 	//�ְ�7·IO�ڵĵ�ƽ����Ϊ��
    GPIO_SetBits(GPIOG,0x0003);//PG1,PG0		//�ְ�7·IO�ڵĵ�ƽ����Ϊ��
    GPIO_SetBits(GPIOF,0xE000);//PF15,PF14,PF13	//�ְ�7·IO�ڵĵ�ƽ����Ϊ��
    return col;
}

const static uint16_t scanedRows[] = {0x1000,0x2000,0x4000,0x8000};//��ȡ���м����еĸ���λ����������е�ϣ�
 //���̻ص�
ExeCommand ext::Keyboard::Scan() { //���м��̵���ɨ�躯��
    ExeCommand cmd = None;//
    uint16_t keyDown = GPIO_ReadInputData(GPIOB) & 0x8000;//��ⷵ�ؼ�
    if(keyDown == 0) {//��⵽�����˷��ؼ�
        if(key_prev == CMD_FanHui) {//�����������Ƚϰ�����ֵ�Ƿ��Ƿ��ؼ�����������key_prevӦ�õ���0��������������
            return None;//�����ȷ��ؿ�
        }
        delay_ms(10); //
        keyDown = GPIO_ReadInputData(GPIOB) & 0x8000;//��ⷵ�ؼ�
        if(keyDown == 0) {//��⵽�����˷��ؼ�
            LOG_I("Fan Hui Returned");
            key_prev = CMD_FanHui;
            return CMD_FanHui;
        }
    }
    else if(key_prev == CMD_FanHui) {
        key_prev = None;
    }

    uint16_t row = ext::Keyboard::ScanRow() & 0xF000;//��ȡ�е�����
    if(row != key_prev) {
        if(row == 0x1000 || row == 0x2000 || row == 0x4000 || row == 0x8000) {//��⵽�е��κ�һ��IO�ڵ�ƵΪ��-----����ʵ�������д�� if ��row |= 0x0000����
            uint16_t col = ext::Keyboard::SubScanCol(row);
            LOG_I("key[0x%04x][0x%x]",row,col);
            if(col == 0xFFFF) {
                LOG_E("Error col 0x%x",col);
                return None;
            }
            key_prev = row;
            if(row == 0x1000)return row0[col];	//����ط�Ӧ��д��case���
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