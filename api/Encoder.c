#include "./Encoder.h"
const static int INPinsA = GPIO_Pin_6 | GPIO_Pin_7;
static void (*g_onExitInterupt)(char,char);
/*******************************************************************************
* Function Name : EXTI4_IRQHandler
* Description : This function handles External interrupt Line 4 request.
* Input : None
* Output : None
* Return : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        //Led_RW_OFF();
        //GPIO_ResetBits(GPIOC, GPIO_Pin_4 | GPIO_Pin_5);
        g_onExitInterupt(0,6);
        /* Clear the EXTI line 9 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line7);
    }
    if(EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
        // Led_RW_ON();
        //GPIO_SetBits(GPIOC, GPIO_Pin_4| GPIO_Pin_5 );
        g_onExitInterupt(0,7);
        /* Clear the EXTI line 9 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line6);
    }
}
u8 ENCODER_Init(void (*onExitInterupt)(char,char)) {
    u8 input;
    u8 inH,inL;
    u8 ret = 1;
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    //RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, DISABLE);

    GPIO_InitStructure.GPIO_Pin  = INPinsA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    input = GPIO_ReadInputData(GPIOA);
    inH = (input >> 1) & 0x40;
    inL =  input & 0x40;

    if(0 == (inH ^ inL)) {
        ret = 0;
    }

    //change it to interrupt mode
    g_onExitInterupt = onExitInterupt;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    GPIO_InitStructure.GPIO_Pin  = INPinsA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource6);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource7);

    EXTI_InitStructure.EXTI_Line= EXTI_Line6;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);


    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    return ret;
}