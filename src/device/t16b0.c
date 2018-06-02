#include "lpc11xx.h"
#include "t16b0.h"


uint8_t flag_5s ;
uint16_t flag_5s_count ;

uint16_t flag_1000ms ;
uint16_t flag_1000ms_count ;

uint8_t flag_10ms;//用于急停信号滤波
uint8_t flag_10ms_p1;//用于一级跑偏滤波
uint8_t flag_10ms_p2;//用于二级跑偏滤波
uint8_t flag_10ms_count;
uint8_t flag_CAN_Send;//用于判断CAN发送成功定时，10ms+1


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
/* 函数功能：初始化TIM32B0                              */
/* 入口参数：ms ：定时毫秒值（8ms以内）                  */
/* 说    明：当定时时间到了以后，会进入TIM32B0中断函数  */
/********************************************************/
void TIM16B0_INT_init(uint16_t ms)
{
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);	//使能TIM16B0时钟
	LPC_TMR16B0->TCR = 0x02;				//复位定时器（bit1：写1复位）
	LPC_TMR16B0->PR  = 0x05;				//把预分频寄存器置4，使PC+1，TC+1，每6个pclkTC+1
	LPC_TMR16B0->MR0 = ms*8000;  			//
	LPC_TMR16B0->IR  = 0x01;				//MR0中断复位,即清中断（bit0:MR0, bit1:MR1, bit2:MR2, bit3:MR3, bit4:CP0）
	LPC_TMR16B0->MCR = 0x03;				//MR0中断产生时停止TC和PC，并使TCR[0]=0, 停止定时器工作，并产生中断
	LPC_TMR16B0->TCR = 0x01;				//启动定时器：TCR[0]=1;
	NVIC_EnableIRQ(TIMER_16_0_IRQn);		//使能TIM16B0中断
	
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
					if(flag_1500ms_count > 4)  //如果排序没成功时，100ms发一次心跳
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
					
					if(flag_1500ms_count > 49)  //如果排序成功了，1s发一次心跳
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
												//闰年，2月有29天
												if (day==30)
												{
													day=1;
													month++;
												}
											}
											else
											{
												//平年，2月有28天
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











