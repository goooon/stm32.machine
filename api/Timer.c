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
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
}
//延时nus
//nus为要延时的us数.
void Timer1_DelayUS(uint us)
{
    u32 temp;
    SysTick->LOAD=SystemCoreClock/8000000 * us; //时间加载
    SysTick->VAL=0x00;        //清空计数器
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数
    do
    {
        temp=SysTick->CTRL;
    }
    while(temp&0x01&&!(temp&(1<<16)));//等待时间到达
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
    SysTick->VAL =0X00;       //清空计数器
}
//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864
void Timer1_DelayMS(uint ms)
{
    u32 temp;
    SysTick->LOAD=SystemCoreClock/8000 * (u32)ms;//时间加载(SysTick->LOAD为24bit)
    SysTick->VAL =0x00;           //清空计数器
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数
    do
    {
        temp=SysTick->CTRL;
    }
    while(temp&0x01&&!(temp&(1<<16)));//等待时间到达
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
    SysTick->VAL =0X00;       //清空计数器
}