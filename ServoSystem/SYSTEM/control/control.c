#include "stm32f10x.h"
#include "control.h"


//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u16 arr,u16 psc,u16 pulse)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3部分重映射  TIM3_CH2->PB5    
 
   //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOA.7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channe2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_Pulse=pulse;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
	TIM_Cmd(TIM3, DISABLE);  //不使能TIM3，中断响应后在开启，保证主从定时器同步工作
	
	TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Enable);  //TIM3选择使能主从模式主定时器
	TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update);         //TIM3选择更新事件作为trgo触发源


}

/*
主：TIM3 从：TIM2 ->ITR2
从定时器配置函数
period: TIM3的溢出值 即设定的pwm脉冲数
*/
void Slave_TIM2(u16 period)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	
	//定时器TIM2初始化
	TIM_TimeBaseStructure.TIM_Period = period; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM2->CR1 |= TIM_CR1_URS;	//防止启动时发生定时器更新中断
	TIM_TimeBaseStructure.TIM_Prescaler =0; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
   
	TIM_SelectSlaveMode(TIM2,TIM_SlaveMode_External1); //TIM2选择从定时器模式
	TIM_SelectInputTrigger(TIM2,TIM_TS_ITR2);          //TIM2选择内部触发来源为TIM3 
	
//	TIM_ARRPreloadConfig(TIM2, ENABLE);   //使能预装载
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM2, DISABLE);  //不使能TIMx					 
}

/*
方向控制配置函数
PA6
高电平正转：顺时针
低电平反转：逆时针
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
编码器捕获
*/
//void TIM4_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
//    TIM_ICInitTypeDef TIM_ICInitStructure;
//    NVIC_InitTypeDef NVIC_InitStructure;

//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
////设置该引脚为复用输出功能
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //A、B相
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
//	
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO

////定时器设置-------------------------------------------------------------   
//	TIM_TimeBaseInitStructure.TIM_Period = 4095*4;  //重装载值 (编码器线数-1)*4  两相计数，一个脉冲4次计数
//    TIM_TimeBaseInitStructure.TIM_Prescaler=0x0;  //预分频
//    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数
//    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; //时钟分割

//    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);//初始化TIM4

////编码器模式设置--------------------------------------------------------------                 

//    TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//计数模式3

//    TIM_ICStructInit(&TIM_ICInitStructure); //将结构体中的内容缺省输入
//    TIM_ICInitStructure.TIM_ICFilter = 10;//滤波器值
//    TIM_ICInit(TIM4, &TIM_ICInitStructure);  //将TIM_ICInitStructure中的指定参数初始化TIM4
////溢出中断设置--------------------------------------------------------------  
//    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //允许TIM4溢出中断

//    NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01;
//    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//    NVIC_Init(&NVIC_InitStructure);


//  TIM_SetCounter(TIM4,0); //TIM4->CNT=0
//  TIM_Cmd(TIM4, DISABLE);//******//


//}

/*
电机初始化，复位电机动作
*/
void Motor_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //Z相
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

   //GPIOB.8	  中断线以及中断初始化配置 下降沿触发 
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line8;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键KEY1所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级1 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

		
	Motor_CCW;		//电机逆时针旋转
	TIM3_PWM_Init(7199,0,3599);	 //不分频。PWM频率=72000000/900=80Khz,占空比450/900=50%
	TIM_Cmd(TIM3,ENABLE);
	
}


//TIM2中断服务函数
int TIM2_Flg=0;
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		TIM_Cmd(TIM3,DISABLE);							//TIM2产生中断后关闭TIM3
		TIM_Cmd(TIM2,DISABLE);
		TIM2_Flg = 1;	
		
	}

}


//int circle_count=0;//圈数
//void TIM4_IRQHandler(void)
//{
//    if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //是否溢出中断
//    {      
//        if((TIM4->CR1>>4 & 0x01)==0) //DIR==0
//            circle_count++;
//        else if((TIM4->CR1>>4 & 0x01)==1)//DIR==1
//            circle_count--;
//    }
//    TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
//}
/*各个值的计算：
脉冲数：  TIM_GetCounter(TIM4)/4  

int angle=0;  //转过总角度
int Realyangle = 0;  //当前实际角度 0~360
int Distiance=0;   //运行距离
extern int circle_count;   //转过圈数


Realyangle = TIM_GetCounter(TIM4)/4/编码器线数 ;      //先除4   最后除编码器线数

angle=Realyangle +circle_count*360;//当前角度


Distiance = angle/360*编码器齿轮周长  + circle_count*编码器齿轮周长

*/

int Flg2=0;
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line8)!= RESET)   //判断是否低电平
	{
		TIM_Cmd(TIM3,DISABLE);
		//TIM_Cmd(TIM2,DISABLE);  //关闭2、3时钟 即停止输出PWM波
		EXTI_ClearITPendingBit(EXTI_Line8);
		EXTI->IMR &= ~(EXTI_Line8); //关闭PB.8的外部中断
		Flg2=1;
	}
	
}

