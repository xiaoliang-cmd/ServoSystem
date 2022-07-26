#include "stm32f10x.h"
#include "core_cm3.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
//#include "timer.h"
#include "exti.h"
#include "key.h"
#include "control.h"

/*************************
TIM2 定时提供时间基准
TIM3 输出PWM波
TIM4 捕获ABZ编码
电机->65536脉冲/r->75mm/r 即一个脉冲位移约0.00115mm
1.初始化程序，电机复位
2.按下开关，电机开始运行
*************************/

/************************
功能：1.实现电机模拟脉搏波规律运动
	  2.实现触摸屏幕人机交互控制电机运动
version 1.0    by chenliang in 2022/07/18
*************************/


extern u8  TIM4CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u16	TIM4CH1_CAPTURE_VAL;	//输入捕获值
extern int TIM2_Flg;
extern int Flg2;

int main(void)
{ 
//	// 关闭所有中断
//    __set_FAULTMASK(1); 
//    // 复位
//    NVIC_SystemReset(); 
	int i,X1=10;         //X1电机循环运动的次数
	delay_init();	    //延时函数初始化	
	delay_ms(500);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	EXTIX_Init();//初始化限位开关外部中断输入	
	uart_init(9600);	
	DIR_Crl_Init();  //初始化电机方向IO口	
	Motor_Init();      //电机初始化 找零点
	while(Flg2==0);   //等待
	delay_ms(500);
	//前进30mm
	Motor_CW;		//电机顺时针旋转
	TIM3_PWM_Init(7199,0,3599);	 //不分频。PWM频率=72000000/7200=10Khz,占空比450/900=50%
	Slave_TIM2(26214-1); 			//输出的脉冲个数
	TIM_Cmd(TIM3,ENABLE);
	TIM_Cmd(TIM2,ENABLE);
//	TIM4Cap_Init(0XFFFF,72-1);	//以1Mhz的频率计数 即1us计数1次
	while(1)
	{
		delay_ms(10);
		if(USART_RX_STA&0x8000)
		{
			
			if(USART_RX_BUF[0]==0x01)
			{
				for(i=X1;i>0;i--)
				{
					//前进6mm
					TIM2_Flg=0;
					Motor_CW;
					TIM3_PWM_Init(7199,0,3599);	 //不分频。PWM频率=72000000/450=160Khz,占空比225/450=50%
					Slave_TIM2(5242-1); 			//输出的脉冲个数
					TIM_Cmd(TIM3,ENABLE);
					TIM_Cmd(TIM2,ENABLE);
					while(TIM2_Flg!=1); //等待
					
					//后退2.5mm
					TIM2_Flg=0;
					Motor_CCW;
					Slave_TIM2(2184-1); 			//输出的脉冲个数
					TIM_Cmd(TIM3,ENABLE);
					TIM_Cmd(TIM2,ENABLE);
					while(TIM2_Flg!=1); //等待
					
					//前进0.9mm
					TIM2_Flg=0;
					Motor_CW;
					Slave_TIM2(786-1); 			//输出的脉冲个数
					TIM_Cmd(TIM3,ENABLE);
					TIM_Cmd(TIM2,ENABLE);
					while(TIM2_Flg!=1); //等待
					
					//后退4.4mm
					TIM2_Flg=0;
					Motor_CCW;
					Slave_TIM2(3844-1); 			//输出的脉冲个数
					TIM_Cmd(TIM3,ENABLE);
					TIM_Cmd(TIM2,ENABLE);
					while(TIM2_Flg!=1); //等待
				}
				USART_RX_STA=0;	
			}
			if(USART_RX_BUF[0]==0x02)
			{
				Motor_CW;		//电机顺时针旋转
				TIM3_PWM_Init(899,0,449);	 //不分频。PWM频率=72000000/900=80Khz,占空比450/900=50%
				TIM_Cmd(TIM3,ENABLE);
			
				USART_RX_STA=0;
			}
			if(USART_RX_BUF[0]==0x03)
			{
				
				TIM_Cmd(TIM3,DISABLE);
				TIM2_Flg=0;
				USART_RX_STA=0;
			}
		}
			
	}	
	
}



