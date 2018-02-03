#include "i2c.h"

//void I2C_Init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct);
void I2C1_init(void)
{
	I2C_InitTypeDef I2C_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_ClockSpeed = 200000;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x0a;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1,&I2C_InitStruct);
	I2C_Cmd(I2C1,ENABLE);
}
u8 At24c02_write(u8 page,u8 address,u8* buff,u8 count)
{
	u8 i,j,k;
	i = 8-address;//页内剩余空间
	if(count <= i)
	{
		At24c02_w8(page,address,buff,count);
	}
	else
	{
		j = (count - i)/8;//整页数
		k = (count - i)%8;//整页后剩余数
		At24c02_w8(page++,address,buff,i);
		buff += i;
		for(;j>0;j--)
		{
			At24c02_w8(page++,0x00,buff,8);
			buff +=8;
		}
		At24c02_w8(page,0x00,buff,k);
	}
	return 0;
}
/**********************************************************/
//功能：页写，最多写入8个字节
//参数：页地址，页内地址，待写入数据,页内地址不能超过0x07,个数不能超过8
//返回值：0成功，1失败
/**********************************************************/
u8 At24c02_w8(u8 page,u8 address,u8* buff,u8 count)
{
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY)==SET);
	
	I2C_GenerateSTART(I2C1,ENABLE);
	while(SUCCESS != I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));//EV5
	
	I2C_Send7bitAddress(I2C1,At24c02_address,I2C_Direction_Transmitter);
	//while(I2C_GetFlagStatus(I2C1,I2C_FLAG_AF));
	while(SUCCESS != I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//EV6

	I2C_SendData(I2C1,(page<<3)+address);
	while(SUCCESS != I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING));//EV8
	for(;count>0;count--)
	{
		I2C_SendData(I2C1,*buff++);
		while(SUCCESS != I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));//EV8_2
	}
	
	I2C_GenerateSTOP(I2C1,ENABLE);
	delay_ms(10);
	return 0;
}
/**********************************************************/
//功能：连续读
//参数：页地址，页内地址，数据指针，数量
//返回值：0成功，1失败
/**********************************************************/
u8 At24c02_read(u8 page,u8 address,u8* buff,u8 count)
{
	
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY)==SET);
	
	I2C_GenerateSTART(I2C1,ENABLE);
	while(SUCCESS != I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));//EV5
	
	I2C_Send7bitAddress(I2C1,At24c02_address,I2C_Direction_Transmitter);
	//while(I2C_GetFlagStatus(I2C1,I2C_FLAG_AF));
	while(SUCCESS != I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//EV6
	
	I2C_SendData(I2C1,(page<<3)+address);
	while(SUCCESS != I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	
	I2C_GenerateSTART(I2C1,ENABLE);
	while(SUCCESS != I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));//EV5
	
	I2C_Send7bitAddress(I2C1,At24c02_address,I2C_Direction_Receiver);
	while(SUCCESS != I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));//EV6
	
	for(;count>1;count--)
	{
		while(SUCCESS != I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED));//EV7
		*buff++ = I2C_ReceiveData(I2C1);
	}
	
	I2C_AcknowledgeConfig(I2C1,DISABLE);//关闭响应
	while(SUCCESS != I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED));//EV7
	*buff = I2C_ReceiveData(I2C1);
	
	I2C_GenerateSTOP(I2C1,ENABLE);
	I2C_AcknowledgeConfig(I2C1,ENABLE);
	return 0;
}
