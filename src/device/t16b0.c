#include "lpc11xx.h"
#include "t16b0.h"


uint8_t flag_5s ;
uint16_t flag_5s_count ;

uint16_t flag_1000ms ;
uint16_t flag_1000ms_count ;

uint8_t flag_10ms;//���ڼ�ͣ�ź��˲�
uint8_t flag_10ms_p1;//����һ����ƫ�˲�
uint8_t flag_10ms_p2;//���ڶ�����ƫ�˲�
uint8_t flag_10ms_count;
uint8_t flag_CAN_Send;//�����ж�CAN���ͳɹ���ʱ��10ms+1


uint8_t flag_20ms;
uint8_t flag_20ms_count;

uint8_t flag_jt_50ms;
uint8_t flag_jt_50ms_count;

uint8_t flag_1500ms;
uint8_t flag_1500ms_count;

uint8_t flag_18V;

extern uint8_t can_send_flag;

//uint8_t flag_test=0;


/********************************************************/
/* �������ܣ���ʼ��TIM32B0                              */
/* ��ڲ�����ms ����ʱ����ֵ��8ms���ڣ�                  */
/* ˵    ��������ʱʱ�䵽���Ժ󣬻����TIM32B0�жϺ���  */
/********************************************************/
void TIM16B0_INT_init(uint16_t ms)
{
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);	//ʹ��TIM16B0ʱ��
	LPC_TMR16B0->TCR = 0x02;				//��λ��ʱ����bit1��д1��λ��
	LPC_TMR16B0->PR  = 0x05;				//��Ԥ��Ƶ�Ĵ�����4��ʹPC+1��TC+1��ÿ6��pclkTC+1
	LPC_TMR16B0->MR0 = ms*8000;  			//
	LPC_TMR16B0->IR  = 0x01;				//MR0�жϸ�λ,�����жϣ�bit0:MR0, bit1:MR1, bit2:MR2, bit3:MR3, bit4:CP0��
	LPC_TMR16B0->MCR = 0x03;				//MR0�жϲ���ʱֹͣTC��PC����ʹTCR[0]=0, ֹͣ��ʱ���������������ж�
	LPC_TMR16B0->TCR = 0x01;				//������ʱ����TCR[0]=1;
	NVIC_EnableIRQ(TIMER_16_0_IRQn);		//ʹ��TIM16B0�ж�
	
	//LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);
	//LPC_SYSCON->SYSAHBCLKCTRL |= (1<<16);
	//LPC_IOCON->PIO2_11 &= ~0x07;
// 	LPC_IOCON->PIO2_11 &= ~0x07;
// 	LPC_IOCON->PIO1_10 &= ~0x07;
// 	LPC_IOCON->PIO0_9 &= ~0x07;
// 	LPC_IOCON->PIO0_8 &= ~0x07;
// 	LPC_IOCON->PIO2_2 &= ~0x07;
// 	LPC_IOCON->PIO2_10 &= ~0x07;
// 	LPC_IOCON->PIO2_9 &= ~0x07;
// 	LPC_IOCON->PIO0_7 &= ~0x07;
// 	LPC_IOCON->PIO0_6 &= ~0x07;
//	LPC_IOCON->PIO2_5 &= ~0x07;
	
// 	LPC_IOCON->PIO0_2 &= ~0x07;
// 	LPC_GPIO0->DIR|=(1<<2);
// 	//LPC_GPIO2->DATA|=(1<<11);
// 	LPC_GPIO0->DATA&=~(1<<2);
}

void TIMER16_0_IRQHandler(void)
{
	
	if((LPC_TMR16B0->IR & 0x1)==1) 
	{
		
//		if(flag_test==0)
//		{
//			LPC_GPIO3->DATA&=~(1<<2);
//			flag_test=1;
//		}
//		else if(flag_test==1)
//		{
//			LPC_GPIO3->DATA|=(1<<2);
//			flag_test=0;
//		}
		flag_10ms_count++;
		if(flag_10ms_count > 1 )    //10ms
		{
			extern uint8_t px_flag;
			flag_10ms = 1;
			flag_10ms_p1=1;
			flag_10ms_p2=1;
			flag_18V = 1;
			
			if(can_send_flag==1)
			{
			flag_CAN_Send++;
			}
			else if(can_send_flag==0)
				flag_CAN_Send=0;
			
			flag_10ms_count = 0;
			flag_20ms_count++;
			if(flag_20ms_count > 1 )    //20ms
			{
				flag_20ms = 1;
				flag_20ms_count = 0;
				flag_1500ms_count++;
				flag_1000ms_count++;
				
				if (px_flag==0)
				{
					if(flag_1500ms_count > 4)  //�������û�ɹ�ʱ��100ms��һ������
					{
						flag_1500ms = 1;
						flag_1500ms_count   = 0;							
					}
//					if (flag_1000ms_count>=50)
//					{
//						flag_1000ms_count=0;
//						flag_1000ms=1;
//					}
				}//if (px_flag==0)
				else
				{
					
					if(flag_1500ms_count > 49)  //�������ɹ��ˣ�1s��һ������
					{
						flag_1500ms = 1;
						flag_1500ms_count   = 0;		
						
					}
				}
				
				if (flag_1000ms_count==50)
					{
//						extern uint8_t second,minute,hour,day,month,year;
						flag_1000ms_count=0;
						flag_1000ms=1;
						second++;
						if (second==60)
						{
							second=0;
							minute++;
							if (minute==60)
							{
								minute=0;
								hour++;
								if (hour==24)
								{
									hour=0;
									day++;
									switch(month)
									{
										case 1:{
											if (day==32)
											{
												day=1;
												month++;
											}
										};break;
										case 2:{
											if ((((year%4)==0)&&(year%100)!=0)||((year%400)==0))
											{
												//���꣬2����29��
												if (day==30)
												{
													day=1;
													month++;
												}
											}
											else
											{
												//ƽ�꣬2����28��
												if (day==29)
												{
													day=1;
													month++;
												}
											}										
										};break;
										case 3:{
											if (day==32)
											{
												day=1;
												month++;
											}
										};break;
										case 4:{
											if (day==31)
											{
												day=1;
												month++;
											}
										};break;
										case 5:{
											if (day==32)
											{
												day=1;
												month++;
											}
										};break;
										case 6:{
											if (day==31)
											{
												day=1;
												month++;
											}
										};break;
										case 7:{
											if (day==32)
											{
												day=1;
												month++;
											}
										};break;
										case 8:{
											if (day==32)
											{
												day=1;
												month++;
											}
										};break;
										case 9:{
											if (day==31)
											{
												day=1;
												month++;
											}
										};break;
										case 10:{
											if (day==32)
											{
												day=1;
												month++;
											}
										};break;
										case 11:{
											if (day==31)
											{
												day=1;
												month++;
											}
										};break;
										case 12:{
											if (day==32)
											{
												day=1;
												month=1;
												year ++;
											}
										};break;
									}
									
								}
							}
						}
						
						
					}
			}
			
			
		}
		
		
		LPC_TMR16B0->IR = 0x1F; 	
	}		
}











