

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
//====================LED����PC2��PC3==============================
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_2|GPIO_Pin_3);

}													 


/****************************************************
 *���ܣ���ʱ����ʼ������
 *TIM_Period���Զ���װ�ؼĴ������ڵ�ֵ(����ֵ)��
 *�����Ƕ�ʱ���ж�Ҫ�����²��ܽ��жϣ�����2000��ʾҪ��2000�½��ж�
 *�����ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж�
 *TIM_Prescaler��ʱ��Ԥ��Ƶ�����㹫ʽΪʱ��Ƶ��=72MHZ/(ʱ��Ԥ��Ƶ+1)
 *ʱ��Ƶ�ʣ�һ���ӿ��������¡�������72MHZ/36000=2000����1s����2000��,
 *��˱������Ƕ�ʱ1s��
 *
 * оǶstm32 @ 2013-7-16
 ****************************************************/
void TIM2_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
	TIM_DeInit(TIM2);
	TIM_TimeBaseStructure.TIM_Period=1; 					//�Զ���װ�ؼĴ������ڵ�ֵ(����ֵ)//ÿһ�ζ������жϣ��൱��20K���ж�ƽ��
	TIM_TimeBaseStructure.TIM_Prescaler= (3000 - 1); 		// ʱ��Ԥ��Ƶ //�޸�Ϊ20K��
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 	//������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update); 					//�������жϱ�־
	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_Trigger,ENABLE);	//������ӦTIM2�ж�Դ
	TIM_Cmd(TIM2, ENABLE); 									//ʹ��TIM2
}					   
	
					   
/*******************************************
 * ��ʼ��NVIC�жϿ���������
 * ע��:STM32��Ȼ���60��������жϣ���ʵ����ÿ���ж�ֻ����4bit
 * ������4bit���жϽ����˷ֵȼ������飩����Ϊ��ռ���ȼ��������ȼ���
 * оǶstm32  @2013-6-2ע��
*********************************************/
void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	    	//����2λ�����ȼ�����ռ���ȼ�����2λ�����ȼ�����Ӧ���ȼ���
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  			//�ⲿ�ж���0,1�ֱ��ӦPC0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 	//io�����ж���ռ���ȵȼ�Ϊ�ڶ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 			//io�����ж������ȵȼ�Ϊ�ڶ�����
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


