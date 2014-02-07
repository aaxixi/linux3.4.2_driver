

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "misc.h"

void  Delay (u32 nCount)
{
  for(; nCount != 0; nCount--);
}


void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |RCC_APB2Periph_AFIO, ENABLE);
//====================LED引脚PC2，PC3==============================
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_2|GPIO_Pin_3);

}													 


/****************************************************
 *功能：定时器初始化配置
 *TIM_Period：自动重装载寄存器周期的值(计数值)，
 *含义是定时器中断要跳几下才能进中断，这里2000表示要跳2000下进中断
 *即：累计 TIM_Period个频率后产生一个更新或者中断
 *TIM_Prescaler：时钟预分频，计算公式为时钟频率=72MHZ/(时钟预分频+1)
 *时钟频率：一秒钟可以跳几下。这里是72MHZ/36000=2000，即1s会跳2000次,
 *因此本配置是定时1s钟
 *
 * 芯嵌stm32 @ 2013-7-16
 ****************************************************/
void TIM2_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
	TIM_DeInit(TIM2);
	TIM_TimeBaseStructure.TIM_Period=1; 					//自动重装载寄存器周期的值(计数值)//每一次都进入中断，相当于20K的中断平率
	TIM_TimeBaseStructure.TIM_Prescaler= (3000 - 1); 		// 时钟预分频 //修改为20K次
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 	//采样分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update); 					//清除溢出中断标志
	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_Trigger,ENABLE);	//开启对应TIM2中断源
	TIM_Cmd(TIM2, ENABLE); 									//使能TIM2
}					   
	
					   
/*******************************************
 * 初始化NVIC中断控制器配置
 * 注意:STM32虽然多达60多个可用中断，但实际上每个中断只用了4bit
 * 就是这4bit对中断进行了分等级（分组），分为抢占优先级和子优先级。
 * 芯嵌stm32  @2013-6-2注释
*********************************************/
void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	    	//配置2位高优先级（抢占优先级），2位子优先级（响应优先级）
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  			//外部中断线0,1分别对应PC0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 	//io引脚中断抢占优先等级为第二级。
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 			//io引脚中断子优先等级为第二级。
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);

}

		
					   
int main(void)
{
	SystemInit();
	GPIO_Config();
	
	NVIC_Config();
	TIM2_Config();

	while(1){

	}


}


