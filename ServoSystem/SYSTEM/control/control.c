#include "stm32f10x.h"
#include "control.h"


//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_PWM_Init(u16 arr,u16 psc,u16 pulse)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3������ӳ��  TIM3_CH2->PB5    
 
   //���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��	GPIOA.7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
 
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM3 Channe2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_Pulse=pulse;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_Cmd(TIM3, DISABLE);  //��ʹ��TIM3���ж���Ӧ���ڿ�������֤���Ӷ�ʱ��ͬ������
	
	TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Enable);  //TIM3ѡ��ʹ������ģʽ����ʱ��
	TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update);         //TIM3ѡ������¼���Ϊtrgo����Դ


}

/*
����TIM3 �ӣ�TIM2 ->ITR2
�Ӷ�ʱ�����ú���
period: TIM3�����ֵ ���趨��pwm������
*/
void Slave_TIM2(u16 period)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM2��ʼ��
	TIM_TimeBaseStructure.TIM_Period = period; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM2->CR1 |= TIM_CR1_URS;	//��ֹ����ʱ������ʱ�������ж�
	TIM_TimeBaseStructure.TIM_Prescaler =0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
   
	TIM_SelectSlaveMode(TIM2,TIM_SlaveMode_External1); //TIM2ѡ��Ӷ�ʱ��ģʽ
	TIM_SelectInputTrigger(TIM2,TIM_TS_ITR2);          //TIM2ѡ���ڲ�������ԴΪTIM3 
	
//	TIM_ARRPreloadConfig(TIM2, ENABLE);   //ʹ��Ԥװ��
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM2�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM2, DISABLE);  //��ʹ��TIMx					 
}

/*
����������ú���
PA6
�ߵ�ƽ��ת��˳ʱ��
�͵�ƽ��ת����ʱ��
*/
void DIR_Crl_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

}

/*
����������
*/
//void TIM4_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
//    TIM_ICInitTypeDef TIM_ICInitStructure;
//    NVIC_InitTypeDef NVIC_InitStructure;

//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
////���ø�����Ϊ�����������
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //A��B��
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������
//	
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO

////��ʱ������-------------------------------------------------------------   
//	TIM_TimeBaseInitStructure.TIM_Period = 4095*4;  //��װ��ֵ (����������-1)*4  ���������һ������4�μ���
//    TIM_TimeBaseInitStructure.TIM_Prescaler=0x0;  //Ԥ��Ƶ
//    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���
//    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; //ʱ�ӷָ�

//    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);//��ʼ��TIM4

////������ģʽ����--------------------------------------------------------------                 

//    TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//����ģʽ3

//    TIM_ICStructInit(&TIM_ICInitStructure); //���ṹ���е�����ȱʡ����
//    TIM_ICInitStructure.TIM_ICFilter = 10;//�˲���ֵ
//    TIM_ICInit(TIM4, &TIM_ICInitStructure);  //��TIM_ICInitStructure�е�ָ��������ʼ��TIM4
////����ж�����--------------------------------------------------------------  
//    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //����TIM4����ж�

//    NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01;
//    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//    NVIC_Init(&NVIC_InitStructure);


//  TIM_SetCounter(TIM4,0); //TIM4->CNT=0
//  TIM_Cmd(TIM4, DISABLE);//******//


//}

/*
�����ʼ������λ�������
*/
void Motor_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //Z��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

   //GPIOB.8	  �ж����Լ��жϳ�ʼ������ �½��ش��� 
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line8;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ�ܰ���KEY1���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//��ռ���ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//�����ȼ�1 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

		
	Motor_CCW;		//�����ʱ����ת
	TIM3_PWM_Init(7199,0,3599);	 //����Ƶ��PWMƵ��=72000000/900=80Khz,ռ�ձ�450/900=50%
	TIM_Cmd(TIM3,ENABLE);
	
}


//TIM2�жϷ�����
int TIM2_Flg=0;
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM2�����жϷ������
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		TIM_Cmd(TIM3,DISABLE);							//TIM2�����жϺ�ر�TIM3
		TIM_Cmd(TIM2,DISABLE);
		TIM2_Flg = 1;	
		
	}

}


//int circle_count=0;//Ȧ��
//void TIM4_IRQHandler(void)
//{
//    if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //�Ƿ�����ж�
//    {      
//        if((TIM4->CR1>>4 & 0x01)==0) //DIR==0
//            circle_count++;
//        else if((TIM4->CR1>>4 & 0x01)==1)//DIR==1
//            circle_count--;
//    }
//    TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
//}
/*����ֵ�ļ��㣺
��������  TIM_GetCounter(TIM4)/4  

int angle=0;  //ת���ܽǶ�
int Realyangle = 0;  //��ǰʵ�ʽǶ� 0~360
int Distiance=0;   //���о���
extern int circle_count;   //ת��Ȧ��


Realyangle = TIM_GetCounter(TIM4)/4/���������� ;      //�ȳ�4   ��������������

angle=Realyangle +circle_count*360;//��ǰ�Ƕ�


Distiance = angle/360*�����������ܳ�  + circle_count*�����������ܳ�

*/

int Flg2=0;
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line8)!= RESET)   //�ж��Ƿ�͵�ƽ
	{
		TIM_Cmd(TIM3,DISABLE);
		//TIM_Cmd(TIM2,DISABLE);  //�ر�2��3ʱ�� ��ֹͣ���PWM��
		EXTI_ClearITPendingBit(EXTI_Line8);
		EXTI->IMR &= ~(EXTI_Line8); //�ر�PB.8���ⲿ�ж�
		Flg2=1;
	}
	
}

