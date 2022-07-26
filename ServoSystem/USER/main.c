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
TIM2 ��ʱ�ṩʱ���׼
TIM3 ���PWM��
TIM4 ����ABZ����
���->65536����/r->75mm/r ��һ������λ��Լ0.00115mm
1.��ʼ�����򣬵����λ
2.���¿��أ������ʼ����
*************************/

/************************
���ܣ�1.ʵ�ֵ��ģ�������������˶�
	  2.ʵ�ִ�����Ļ�˻��������Ƶ���˶�
version 1.0    by chenliang in 2022/07/18
*************************/


extern u8  TIM4CH1_CAPTURE_STA;		//���벶��״̬		    				
extern u16	TIM4CH1_CAPTURE_VAL;	//���벶��ֵ
extern int TIM2_Flg;
extern int Flg2;

int main(void)
{ 
//	// �ر������ж�
//    __set_FAULTMASK(1); 
//    // ��λ
//    NVIC_SystemReset(); 
	int i,X1=10;         //X1���ѭ���˶��Ĵ���
	delay_init();	    //��ʱ������ʼ��	
	delay_ms(500);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	EXTIX_Init();//��ʼ����λ�����ⲿ�ж�����	
	uart_init(9600);	
	DIR_Crl_Init();  //��ʼ���������IO��	
	Motor_Init();      //�����ʼ�� �����
	while(Flg2==0);   //�ȴ�
	delay_ms(500);
	//ǰ��30mm
	Motor_CW;		//���˳ʱ����ת
	TIM3_PWM_Init(7199,0,3599);	 //����Ƶ��PWMƵ��=72000000/7200=10Khz,ռ�ձ�450/900=50%
	Slave_TIM2(26214-1); 			//������������
	TIM_Cmd(TIM3,ENABLE);
	TIM_Cmd(TIM2,ENABLE);
//	TIM4Cap_Init(0XFFFF,72-1);	//��1Mhz��Ƶ�ʼ��� ��1us����1��
	while(1)
	{
		delay_ms(10);
		if(USART_RX_STA&0x8000)
		{
			
			if(USART_RX_BUF[0]==0x01)
			{
				for(i=X1;i>0;i--)
				{
					//ǰ��6mm
					TIM2_Flg=0;
					Motor_CW;
					TIM3_PWM_Init(7199,0,3599);	 //����Ƶ��PWMƵ��=72000000/450=160Khz,ռ�ձ�225/450=50%
					Slave_TIM2(5242-1); 			//������������
					TIM_Cmd(TIM3,ENABLE);
					TIM_Cmd(TIM2,ENABLE);
					while(TIM2_Flg!=1); //�ȴ�
					
					//����2.5mm
					TIM2_Flg=0;
					Motor_CCW;
					Slave_TIM2(2184-1); 			//������������
					TIM_Cmd(TIM3,ENABLE);
					TIM_Cmd(TIM2,ENABLE);
					while(TIM2_Flg!=1); //�ȴ�
					
					//ǰ��0.9mm
					TIM2_Flg=0;
					Motor_CW;
					Slave_TIM2(786-1); 			//������������
					TIM_Cmd(TIM3,ENABLE);
					TIM_Cmd(TIM2,ENABLE);
					while(TIM2_Flg!=1); //�ȴ�
					
					//����4.4mm
					TIM2_Flg=0;
					Motor_CCW;
					Slave_TIM2(3844-1); 			//������������
					TIM_Cmd(TIM3,ENABLE);
					TIM_Cmd(TIM2,ENABLE);
					while(TIM2_Flg!=1); //�ȴ�
				}
				USART_RX_STA=0;	
			}
			if(USART_RX_BUF[0]==0x02)
			{
				Motor_CW;		//���˳ʱ����ת
				TIM3_PWM_Init(899,0,449);	 //����Ƶ��PWMƵ��=72000000/900=80Khz,ռ�ձ�450/900=50%
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



