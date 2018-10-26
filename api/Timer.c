#include "Timer.h"
void Timer_Configuration(TIM_TypeDef* TIMx,unsigned int period,unsigned int prescaler)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_DeInit(TIMx);//????????TIM?????
    TIM_TimeBaseStructure.TIM_Period=period-1;//??????????1
    TIM_TimeBaseStructure.TIM_Prescaler=prescaler-1;//???????36000
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//??????1,????????
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//????
    TIM_TimeBaseInit(TIMx,&TIM_TimeBaseStructure);
    TIM_ClearFlag(TIMx,TIM_FLAG_Update);//???????
    TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);//????
    TIM_Cmd(TIMx,ENABLE);//??TIM2???
}

void NVIC2_Configuration(u8 TIMx_IRQn)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = TIMx_IRQn; //3.0???????????TIM2_IRQChanel??TIM2_IRQn
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);//NVIC_Init??????misc.c????
}
#define TIMx_IRQHandler(TimerX,TIMx) \
      static void* g_##TIMx##UserData = 0; \
			void (*TIMx##_UserCallback)(void*) = 0;   \
			void TIMx##_IRQHandler(void)   \
			{   \
				if(TIM_GetITStatus(TIMx##,TIM_IT_Update)!=RESET) \
				{   \
					TIM_ClearITPendingBit(TIMx##,TIM_IT_Update); \
					TIMx##_UserCallback(g_##TIMx##UserData);   \
				}   \
			} \
			void TimerX##_Stop(){  \
				TIM_ClearFlag(TIMx,TIM_FLAG_Update); \
				TIM_ITConfig(TIMx,TIM_IT_Update,DISABLE); \
				TIM_Cmd(TIMx,DISABLE); \
			} \
			void TimerX##_Init(unsigned int period,unsigned int prescaler,void (*callback)(void*),void* userData){ \
			  RCC_APB1PeriphClockCmd(RCC_APB1Periph_##TIMx,ENABLE); \
			  NVIC2_Configuration(TIMx##_IRQn); \
			  TIMx##_UserCallback = callback; \
				g_##TIMx##UserData = userData; \
			  Timer_Configuration(TIMx,period,prescaler); \
			}
TIMx_IRQHandler(Timer2,TIM2);
TIMx_IRQHandler(Timer3,TIM3);
TIMx_IRQHandler(Timer4,TIM4);
TIMx_IRQHandler(Timer5,TIM5);
TIMx_IRQHandler(Timer6,TIM6);
TIMx_IRQHandler(Timer7,TIM7);
void Timer1_Init() {
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
}
//��ʱnus
//nusΪҪ��ʱ��us��.
void Timer1_DelayUS(uint us)
{
    u32 temp;
    SysTick->LOAD=SystemCoreClock/8000000 * us; //ʱ�����
    SysTick->VAL=0x00;        //��ռ�����
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����
    do
    {
        temp=SysTick->CTRL;
    }
    while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
    SysTick->VAL =0X00;       //��ռ�����
}
//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��72M������,nms<=1864
void Timer1_DelayMS(uint ms)
{
    u32 temp;
    SysTick->LOAD=SystemCoreClock/8000 * (u32)ms;//ʱ�����(SysTick->LOADΪ24bit)
    SysTick->VAL =0x00;           //��ռ�����
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����
    do
    {
        temp=SysTick->CTRL;
    }
    while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
    SysTick->VAL =0X00;       //��ռ�����
}