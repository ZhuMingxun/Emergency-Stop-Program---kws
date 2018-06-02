#include "can.h"
#include "ambe.h"
#include "dataio.h"
#include "speak.h"
#include "adc.h"
#include "t16b0.h"
#include "AT24C16.h"
#include "watchDog.h"
#include "cari_config.h"
#define TEST_CCAN_BAUD_RATE 10000



uint8_t px_flag = 0;    //0:未排序，1：排序完成
bool send_px_flag;//用于带有电池的掉电恢复时排序用。因此，如果带有电池的闭锁在沿线掉电时，不会重启，此时，如果沿线有其他的设备排序，会有广播的应答帧。此时没有掉电的闭锁也会接收到排序的序号命令，导致错误
uint32_t CAN_ID = 0x3f;
uint8_t can_send_len;
uint16_t can_sound_time;
bool can_rev_monitor_flag;//can接收监视
bool can_send_flag;
//CCAN_MSG_OBJ_T CAN_MSG[20];//在调试中发现，如果在接受中断的回调函数中，发送can数据的话，有可能发不出去，并且tx中断也不会触发，因此，在接收中断中把要发送的数据存起来，然后在主程序里发出。
uint8_t mod_flag[20];//can应答标志队列。0为默认值。//建立一个深度为20个包的缓冲，使用can_send_len计数。
										//mod_flag[x]=1：应答本机ID DLC=0。	
										//mod_flag[x]=2：获取chipID记录 DLC=4。
										//mod_flag[x]=3：清除flash命令 DLC=0。
										//mod_flag[x]=4：获取历史记录命令 DLC=0。
bool can_sound_flag;//收到语音标志
bool heart_flag;//心跳标志，收到can总线命令，允许心跳或者禁心跳
//Chip_GPIO_SetPinDIRInput(LPC_GPIO, JT_PORT, JT_PORT_BIT);
uint8_t can_sound_buf[CAN_SOUND_BUF_SIZE];//收到的语音帧缓冲
uint8_t can_sound_buf_count;//收到语音缓冲计数器，用于指定数据
CCAN_MSG_OBJ_T msg_obj_d;
bool reset_flag;

uint32_t can_errors;
uint8_t second,minute,hour,day,month,year;
unsigned int chip_ID;//芯片唯一码
//bool mute_flag;
void can_delay(void)
{
	uint16_t i,j;
	for(j=0;j<25;j++)
	{
		for(i=0;i<2048;i++);
	}
}
bool can_send(CCAN_MSG_OBJ_T *msg_obj_s)
{


	while (can_send_flag==true)
	{
		if(flag_CAN_Send>50)
		{
			flag_CAN_Send=0;
			can_send_flag=false;
			CAN_reset();
			CAN_init();//CAN初始化
		}
	}
	can_send_flag=true;
	msg_obj_s->mask=0x7FF;
	msg_obj_s->msgobj=0x1;
	LPC_CCAN_API->can_transmit(msg_obj_s);


	return true;
}
//bool can_send(CCAN_MSG_OBJ_T *msg_obj_s)
//{
//	long timeout;
//	uint8_t timeouttimes;
//	while (can_send_flag==true);
//	timeouttimes=0;
//	
//	can_send_flag=true;
//	msg_obj_s->mask=0x7FF;
//	msg_obj_s->msgobj=0x1;
//	LPC_CCAN_API->can_transmit(msg_obj_s);
////	timeout=25*2048;
////	while (can_send_flag==true)
////	{
////		timeout--;
////		if (timeout==0)
////		{
////			timeouttimes++;
////			timeout=25*2048;
////			if (timeouttimes==10)
////			{
////				uint32_t CanApiClkInitTable[2];
////				CCAN_CALLBACKS_T callbacks = {
////																				CAN_rx,
////																				CAN_tx,
////																				CAN_error,
////																				NULL,
////																				NULL,
////																				NULL,
////																				NULL,
////																				NULL,
////																			};
////				CCAN_MSG_OBJ_T config_msg;
////				config_msg.msgobj=0;
////				config_msg.mask=0;
////				
////				//LPC_CCAN_API->can_transmit(msg_obj_s);
////				timeouttimes=0;
////				
////				can_send_flag=false;
////				NVIC_DisableIRQ(CAN_IRQn);
////				baudrateCalculate(TEST_CCAN_BAUD_RATE, CanApiClkInitTable);
////				LPC_CCAN_API->init_can(&CanApiClkInitTable[0], TRUE);
////				
////				/* Configure the CAN callback functions */
////				LPC_CCAN_API->config_calb(&callbacks);
////				/* Enable the CAN Interrupt */
////				NVIC_EnableIRQ(CAN_IRQn);
////				
////				LPC_CCAN_API->config_rxmsgobj(&config_msg);
////				#if NEW_BS				
////				return false;
////				#else
////				DEBUGOUT("Send frame id:0x%x,data[0]:0x%x\r\n",msg_obj_s->mode_id,msg_obj_s->data[0]);
////				return false;
////				#endif
////			}
////		}
////	}
//	return true;
//}
void CAN_IRQHandler (void)
{
	LPC_CCAN_API->isr();	
}

void CAN_tx(uint8_t msg_obj_num) 
{
	
		can_send_flag=false;
	
}

void CAN_reset(void)//CAN外设复位函数
{
	NVIC_DisableIRQ(CAN_IRQn);
	*((uint32_t*)0x40048004)&=~(1<<3);//复位CAN，寄存器：PRESETCTRL
#if (__ICCARM__)
    asm("nop");
    asm("nop");
#else
	__nop();
	__nop();
#endif
	*((uint32_t*)0x40048004)|=(1<<3);//复位CAN无效
}
void CAN_init(void) //CAN总线初始化
{
	uint32_t CanApiClkInitTable[2];
	
		CCAN_CALLBACKS_T callbacks = {
		CAN_rx,
		CAN_tx,
		CAN_error,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
	};//定义片上回调函数结构体变量
	CCAN_MSG_OBJ_T config_msg;//消息对象结构体
	config_msg.msgobj=0;
	config_msg.mask=0;
	can_sound_buf_count=0;
	//can_sound_flag=false;
 	
	can_errors=0;
	second=0;
	can_send_flag=false;
	can_send_len=0;
	send_px_flag=false;
	heart_flag=true;
	minute=hour=day=month=year=0;
	baudrateCalculate(TEST_CCAN_BAUD_RATE, CanApiClkInitTable);//将位时序数据写入CanApiClkInitTable数组
 	LPC_CCAN_API->init_can(&CanApiClkInitTable[0], TRUE);//调用片上函数将CanApiClkInitTable数组中的数写入CANCLKDIV 寄存器和CAN_BTR寄存器，同时允许CAN中断
	LPC_CCAN_API->config_rxmsgobj(&config_msg);//
	/* Configure the CAN callback functions */
 	LPC_CCAN_API->config_calb(&callbacks);
	/* Enable the CAN Interrupt */
 	NVIC_EnableIRQ(CAN_IRQn);
}
void CAN_rx(uint8_t msg_obj_num) {
	can_rev_monitor_flag=true;
	msg_obj_d.msgobj = msg_obj_num;	
	LPC_CCAN_API->can_receive(&msg_obj_d);
	can_rcv(msg_obj_d);
}

void CAN_error(uint32_t error_info)
{
	
	//DEBUGSTR("Go into can_err!\r\n");
	if (error_info & CAN_ERROR_PASS)
	{
		//DEBUGSTR("Go into can_EPASS!\r\n");
		
	}
	else if (error_info & CAN_ERROR_WARN)
	{
		
		
	}
	else if (error_info & CAN_ERROR_BOFF)
	{
		
		
	}
	else if (error_info & CAN_ERROR_STUF)
	{
		
	}
	else if (error_info & CAN_ERROR_FORM)
	{
		
		
	}
	else if (error_info & CAN_ERROR_ACK)
	{
		
	}
	else if (error_info & CAN_ERROR_BIT1)
	{
		
	}
	else if (error_info & CAN_ERROR_BIT0)
	{
		
	}
	else if (error_info & CAN_ERROR_CRC)
	{
		
	}
	
	
}

void baudrateCalculate(uint32_t baud_rate, uint32_t *can_api_timing_cfg)
{
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_CAN);
 	can_api_timing_cfg[0] = 9; //波特率预分频寄存器
	
	/*           位定时寄存器                                         */
	/*      Tq=5us,10k,采样点前：14采样点后：5，同步段1，采样点：75%    */
// 	can_api_timing_cfg[1] = 0x4dd7;
	
	/*           位定时寄存器                                        */
	/*      Tq=5us,10k,采样点前：15采样点后：4，同步段1，采样点：80%   */
 	can_api_timing_cfg[1] = 0x3ed7;
	
	//官方的位时序计算方法过于复杂，注掉，采用以上两个值直接写寄存器
//	uint32_t pClk, div, quanta, segs, seg1, seg2, clk_per_bit, can_sjw;
//	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_CAN);
//	pClk = Chip_Clock_GetMainClockRate();

//	clk_per_bit = pClk / baud_rate;

//	for (div = 0; div <= 15; div++) {
//		for (quanta = 1; quanta <= 32; quanta++) {
//			for (segs = 3; segs <= 17; segs++) {
//				if (clk_per_bit == (segs * quanta * (div + 1))) {
//					segs -= 3;
//					seg1 = segs / 2;
//					seg2 = segs - seg1;
//					can_sjw = seg1 > 3 ? 3 : seg1;
//					can_api_timing_cfg[0] = div;
//					can_api_timing_cfg[1] =
//						((quanta - 1) & 0x3F) | (can_sjw & 0x03) << 6 | (seg1 & 0x0F) << 8 | (seg2 & 0x07) << 12;
//					return;
//				}
//			}
//		}
//	}
//	 can_api_timing_cfg[0] = 9;
////can_api_timing_cfg[1] = 0x4def ;
//	can_api_timing_cfg[1] = 0x3eef ;
}


/**************************************************/
//mod  = 0:ID未分配，默认63；
//定时发送
/**************************************************/
void can_data_send(uint8_t mod,uint8_t *can_frame)  //mod:ID是否分配 //心跳发生函数
{		
	uint8_t ID_Mod;
	CCAN_MSG_OBJ_T msg_obj_s;
	
//	uint8_t Data0_State = 0;
	ID_Mod = mod;
/////////////////////////ID未分配//////////////////////////////	
	if(ID_Mod == 0)                           //ID未分配,默认63
	{
		msg_obj_s.mode_id = (CAN_MSGOBJ_STD|ID10_09_PeriodSend|ID08_07_Equipments|ID05_00_DefaultID);
		msg_obj_s.dlc     = 4;
		msg_obj_s.data[0] = chip_ID%256;
		msg_obj_s.data[1] = (chip_ID-msg_obj_s.data[0])>>8%256;
		msg_obj_s.data[2] = (chip_ID-(unsigned int)msg_obj_s.data[1]*256-msg_obj_s.data[0])>>16%256;
		msg_obj_s.data[3] = (chip_ID-(unsigned int)msg_obj_s.data[2]*65536-(unsigned int)msg_obj_s.data[1]*256-msg_obj_s.data[0])>>24%256;;
		
	}
///////////////////////////////////////////////////////////////	
/////////////////////////ID已分配//////////////////////////////	
	else if(ID_Mod == 1)                      //ID已分配
	{
		msg_obj_s.mode_id = (CAN_MSGOBJ_STD|ID10_09_PeriodSend|ID08_07_Equipments|CAN_ID); 
		msg_obj_s.dlc     = 2;
		msg_obj_s.data[0] = can_frame[0];
		msg_obj_s.data[1] = can_frame[1];
	
	
	}
///////////////////////////////////////////////////////////////		

#if NEW_BS
	if (can_send(&msg_obj_s)==false)
		can_errors++;
#else

	if (can_send(&msg_obj_s)==false)
	{
		DEBUGOUT("SEND ERROR!\r\n");
	}
	else
		DEBUGOUT("SEND OK!\r\n");
	
#endif
}

//////////////////////打点、讲话数据传递函数////////////////////////////////

void CANSend_AMBESetSine(uint8_t soundflag)
{
	uint8_t SoundID;
	CCAN_MSG_OBJ_T msg_obj_s;
	SoundID = soundflag;
	

	switch(SoundID)
	{
		case 1: //打点开始
			msg_obj_s.mode_id = (CAN_MSGOBJ_STD|ID10_09_BSMConfirm|ID08_07_Soundcodes|CAN_ID);
			msg_obj_s.dlc     = 8;
			msg_obj_s.data[0] = 0x11; //开始数据：0x11
			msg_obj_s.data[1] = 0x11;
		break;
		
		case 2: //打点结束
			msg_obj_s.mode_id = (CAN_MSGOBJ_STD|ID10_09_BSMConfirm|ID08_07_Soundcodes|CAN_ID);
			msg_obj_s.dlc     = 8;
			msg_obj_s.data[0] = 0x22; //结束数据：0x22
			msg_obj_s.data[1] = 0x22;		
		break;
	}
	msg_obj_s.data[2] = 0x0;
	msg_obj_s.data[3] = 0x0;
	msg_obj_s.data[4] = 0x0;
	msg_obj_s.data[5] = 0x0;
	msg_obj_s.data[6] = 0x0;
	msg_obj_s.data[7] = 0x0;
	
	can_send(&msg_obj_s);
}

////////////////////////////语音数据///////////////////////////	


void can_rcv(CCAN_MSG_OBJ_T can_rcv_dat)
{
	CCAN_MSG_OBJ_T CAN_RD_Message ;
	uint32_t CAN_RD_ID;
	uint32_t CAN_RD_ID1;
	CCAN_MSG_OBJ_T msg_obj_s;
	
	CAN_RD_Message = can_rcv_dat;		
	
	if((CAN_RD_Message.mode_id & CAN_MSGOBJ_STD)==CAN_MSGOBJ_STD)  //标准帧判断
	{
		//解码板主机配置命令
		CAN_RD_ID = CAN_RD_Message.mode_id & ((uint16_t)0x3<<9);
		if (CAN_RD_ID==ID10_09_OrderToBSM)
		{
			//主机配置命令
			//广播类型命令
			
			/***********************测试用，断电池供电指令**************************/
			if((CAN_RD_Message.data[0] == 0x55)&&(CAN_RD_Message.data[1] == 0xaa))
			{
				POWEROFF_State_Close();
			}
			/************************************************************************/
			
			
			//////////////////ID分配，排序命令////////////////////////////			
			if(((px_flag == 0)||((CAN_ID&0x3f)==0x3f))&&(flag_18ON==0))         //判断之前没有排序
			{
				if((CAN_RD_Message.data[0] == 0x2a)&&(CAN_RD_Message.data[1] == 0xff)) //修改ID命令
				{
					uint32_t chip;
					chip=CAN_RD_Message.data[2]+(uint32_t)(CAN_RD_Message.data[3]<<8)+(uint32_t)(CAN_RD_Message.data[4]<<16)+(uint32_t)(CAN_RD_Message.data[5]<<24);
					if (chip==0)
					{
						if((CAN_RD_Message.data[6] >= 1)&&(CAN_RD_Message.data[6] <= 60))   //ID控制在1-60之间
						{
							px_flag = 1;     //排序完成
							CAN_ID = CAN_RD_Message.data[6];

							//CAN_MSG[can_send_len]=msg_obj_s;
							//mod_flag[x]=1：应答本机ID DLC=0。
							mod_flag[can_send_len]=1;
							if (can_send_len<20)
								can_send_len++;
							JDQ_State_Close();//继电器吸合，给下一级供电
							#if NEW_BS
							POWEROFF_State_Open();//电池供电接通
							#endif
							reset_flag=true;
							send_px_flag=false;
							
							
						}
						
					}//if chip=0;
				}//if((CAN_RD_Message.data[0] == 0x2a)&&(CAN_RD_Message.data[1] == 0xff)) //修改ID命令
				return ;  //同一帧 不可能同时携带多个信息，因此，到此返回,为了提高处理效率
			}//if(px_flag == 0)         //判断之前没有排序
			
			if((px_flag==1)&&(flag_18ON==0))//else
			{
			//已经排序了,授时必须在已经完成排序的基础上完成
				if((CAN_RD_Message.data[0] == 0x01)&&(CAN_RD_Message.data[1] == 0xff)) //授时命令
				{
					//授时
					year=CAN_RD_Message.data[2];
					month=CAN_RD_Message.data[3];
					day=CAN_RD_Message.data[4];
					hour=CAN_RD_Message.data[5];
					minute=CAN_RD_Message.data[6];
					second=CAN_RD_Message.data[7];
					flag_1000ms_count=0;
					return;
				}//if((CAN_RD_Message.data[0] == 0x2a)&&(CAN_RD_Message.data[1] == 0xff)) //授时命令
			
				if((CAN_RD_Message.data[0] == 0x2a)&&(CAN_RD_Message.data[1] == 0xff)) //修改ID命令
				{
					uint32_t chip;
					chip=CAN_RD_Message.data[2]+(uint32_t)(CAN_RD_Message.data[3]<<8)+(uint32_t)(CAN_RD_Message.data[4]<<16)+(uint32_t)(CAN_RD_Message.data[5]<<24);
					if (chip==chip_ID)
					{
						if((CAN_RD_Message.data[6] >= 1)&&(CAN_RD_Message.data[6] <= 60))   //ID控制在1-62之间
						{
							
							CAN_ID = CAN_RD_Message.data[6];
							JDQ_State_Close();//继电器吸合，给下一级供电

							//mod_flag[x]=1：应答本机ID DLC=0。
							mod_flag[can_send_len]=1;
							if (can_send_len<20)
								can_send_len++;
							
						}
						return ;  //同一帧 不可能同时携带多个信息，因此，到此返回,为了提高处理效率
					}
				}//if((CAN_RD_Message.data[0] == 0x2a)&&(CAN_RD_Message.data[1] == 0xff)) //修改ID命令
			}//if(px_flag == 0)  else       //有排序
			
			
			//以下处理收到的广播帧 
			if((CAN_RD_Message.data[0] == 0x02)&&(CAN_RD_Message.data[1] == 0xff)) //停止心跳命令
			{
				heart_flag=false;
				return;
			}
			if((CAN_RD_Message.data[0] == 0x03)&&(CAN_RD_Message.data[1] == 0xff)) //恢复心跳命令
			{
				heart_flag=true;
				return;
			}
			#if NEW_BS
			if((CAN_RD_Message.data[0] == 0x04)&&(CAN_RD_Message.data[1] == 0xff)) //清除flash存储命令
			{
				//将flash空间存储清空，即，【1】【2】均为0
				
				//mod_flag[x]=3：清除flash命令 DLC=0。
				mod_flag[can_send_len]=3;
				if (can_send_len<20)
					can_send_len++;
				return;
			}
			#endif
			//以下处理针对本机的命令
				//////////////////////////////////////////////////////////////		
			CAN_RD_ID = CAN_RD_Message.mode_id & 0x7f;//取沿线can节点号CAN_ID
			if(CAN_RD_ID == CAN_ID)   //如果是对本机操作（与自身的CAN_ID值相符)
			{
			//////////////////扩音电话充电命令处理////////////////////////////////

				if((CAN_RD_Message.data[0] == 0x2b)&&(CAN_RD_Message.data[1] == 0xff))       //修改充电电流
				{
					#if NEW_BS
					Set_Charge(CAN_RD_Message.data[2]);
					#endif
					//mod_flag[x]=1：应答本机ID DLC=0。
					mod_flag[can_send_len]=1;
					if (can_send_len<20)
						can_send_len++;
					return;
				}
				//////////////////获取闭锁历史记录命令处理////////////////////////////////
				#if NEW_BS
				if((CAN_RD_Message.data[0] == 0x05)&&(CAN_RD_Message.data[1] == 0xff))       //获取历史记录
				{
					//mod_flag[x]=4：获取历史记录命令 DLC=0。
					mod_flag[can_send_len]=4;
					if (can_send_len<20)
						can_send_len++;

					
					return;
				}//if((CAN_RD_Message.data[0] == 0x05)&&(CAN_RD_Message.data[1] == 0xff))       //获取历史记录
				#endif
				if((CAN_RD_Message.data[0] == 0x6)&&(CAN_RD_Message.data[1] == 0xff))       //读chipid
				{
					

					//mod_flag[x]=2：获取chipID命令 DLC=4。
					mod_flag[can_send_len]=2;
					if (can_send_len<20)
						can_send_len++;	
					return;
					
				}//if((CAN_RD_Message.data[0] == 0x6)&&(CAN_RD_Message.data[1] == 0xff))       //读chipid
				//////////////////扩音放音音量调整命令处理////////////////////////////////
				#if NEW_BS
				if((CAN_RD_Message.data[0] == 0x2c)&&(CAN_RD_Message.data[1] == 0xff))       //修改充电电流
				{
					Set_Volume(CAN_RD_Message.data[2]);

					//mod_flag[x]=1：应答命令 DLC=0。
					mod_flag[can_send_len]=1;
					if (can_send_len<20)
						can_send_len++;
					return;
				}//if((CAN_RD_Message.data[0] == 0x2c)&&(CAN_RD_Message.data[1] == 0xff))       //修改充电电流
				#endif
				
			}//if(CAN_RD_ID == CAN_ID)   //如果是对本机操作（与自身的CAN_ID值相符)
//////////////////////////////////////////////////////////////	

		
		}//if (CAN_RD_ID==ID10_09_OrderToBSM)
//////////////////////////////////////////////////////////////	
		
//////////////////语音命令处理////////////////////////////////
		CAN_RD_ID = CAN_RD_Message.mode_id & (0x3<<7);//canid 第7~8位确定数据类型
		if (CAN_RD_ID==ID08_07_Soundcodes)
		{
			uint8_t tmpi;
			//CAN_RD_ID = CAN_RD_ID>>7;
			CAN_RD_ID1=CAN_RD_Message.mode_id&0x7f;
			//if((CAN_RD_ID == 0X03)&&(CAN_RD_ID1!=CAN_ID))   //如果是语音类型
//			if(CAN_RD_ID1!=CAN_ID)
//			{
				
				can_sound_time=0;
				#if NEW_BS
				for (tmpi=0;tmpi<8;tmpi++)
				{
					can_sound_buf[tmpi]=CAN_RD_Message.data[tmpi];//取数据
					//can_sound_buf_count++;
				}
				#endif
				can_sound_flag=true;//收到语音数据帧
				flag_5s=0;
				flag_5s_count=0;
				return;
					
//			}//if(CAN_RD_ID == 0X03)   //如果是语音类型
		}
		
//////////////////////////////////////////////////////////////	
		
		
		
		
		
	}//if((CAN_RD_Message.mode_id & CAN_MSGOBJ_STD)==CAN_MSGOBJ_STD)  //标准帧判断
}

	
