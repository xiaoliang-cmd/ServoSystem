#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"



#define Motor_CW  GPIO_ResetBits(GPIOA,GPIO_Pin_6)   //电机顺时针旋转
#define Motor_CCW   GPIO_SetBits(GPIOA,GPIO_Pin_6)     //电机逆时针旋转
#define PB8  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) //读取Z相I0口值

void TIM3_PWM_Init(u16 arr,u16 psc,u16 pulse); 	//主定时器配置初始化函数
void Slave_TIM2(u16 period);      		//从定时器配置初始化函数
void DIR_Crl_Init(void);				//方向控制IO口初始化函数
void Motor_Init(void);					//电机初始化函数

#endif

