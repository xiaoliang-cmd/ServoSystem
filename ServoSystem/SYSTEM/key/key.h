#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define KEY_L PBin(10)   	//PB10
#define KEY_R PBin(11)	//PB11 


#define KEY0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)//��ȡ����
#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)//��ȡ����


 

//#define KEY0_PRES 	1	//KEY0����
//#define KEY1_PRES	  2	//KEY1����
//#define WKUP_PRES   3	//KEY_UP����(��WK_UP/KEY_UP)


void KEY_Init(void);//IO��ʼ��
				    
#endif

