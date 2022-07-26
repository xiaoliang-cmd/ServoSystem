#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"



#define Motor_CW  GPIO_ResetBits(GPIOA,GPIO_Pin_6)   //���˳ʱ����ת
#define Motor_CCW   GPIO_SetBits(GPIOA,GPIO_Pin_6)     //�����ʱ����ת
#define PB8  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) //��ȡZ��I0��ֵ

void TIM3_PWM_Init(u16 arr,u16 psc,u16 pulse); 	//����ʱ�����ó�ʼ������
void Slave_TIM2(u16 period);      		//�Ӷ�ʱ�����ó�ʼ������
void DIR_Crl_Init(void);				//�������IO�ڳ�ʼ������
void Motor_Init(void);					//�����ʼ������

#endif

