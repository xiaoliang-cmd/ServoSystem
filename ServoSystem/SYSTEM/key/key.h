#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define KEY_L PBin(10)   	//PB10
#define KEY_R PBin(11)	//PB11 


#define KEY0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)//读取开关
#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)//读取开关


 

//#define KEY0_PRES 	1	//KEY0按下
//#define KEY1_PRES	  2	//KEY1按下
//#define WKUP_PRES   3	//KEY_UP按下(即WK_UP/KEY_UP)


void KEY_Init(void);//IO初始化
				    
#endif

