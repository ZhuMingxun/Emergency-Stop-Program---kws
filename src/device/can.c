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



uint8_t px_flag = 0;    //0:δ����1���������
bool send_px_flag;//���ڴ��е�صĵ���ָ�ʱ�����á���ˣ�������е�صı��������ߵ���ʱ��������������ʱ������������������豸���򣬻��й㲥��Ӧ��֡����ʱû�е���ı���Ҳ����յ���������������´���
uint32_t CAN_ID = 0x3f;
uint8_t can_send_len;
uint16_t can_sound_time;
bool can_rev_monitor_flag;//can���ռ���
bool can_send_flag;
//CCAN_MSG_OBJ_T CAN_MSG[20];//�ڵ����з��֣�����ڽ����жϵĻص������У�����can���ݵĻ����п��ܷ�����ȥ������tx�ж�Ҳ���ᴥ������ˣ��ڽ����ж��а�Ҫ���͵����ݴ�������Ȼ�����������﷢����
uint8_t mod_flag[20];//canӦ���־���С�0ΪĬ��ֵ��//����һ�����Ϊ20�����Ļ��壬ʹ��can_send_len������
										//mod_flag[x]=1��Ӧ�𱾻�ID DLC=0��	
										//mod_flag[x]=2����ȡchipID��¼ DLC=4��
										//mod_flag[x]=3�����flash���� DLC=0��
										//mod_flag[x]=4����ȡ��ʷ��¼���� DLC=0��
bool can_sound_flag;//�յ�������־
bool heart_flag;//������־���յ�can������������������߽�����
//Chip_GPIO_SetPinDIRInput(LPC_GPIO, JT_PORT, JT_PORT_BIT);
uint8_t can_sound_buf[CAN_SOUND_BUF_SIZE];//�յ�������֡����
uint8_t can_sound_buf_count;//�յ��������������������ָ������
CCAN_MSG_OBJ_T msg_obj_d;
bool reset_flag;

uint32_t can_errors;
uint8_t second,minute,hour,day,month,year;
unsigned int chip_ID;//оƬΨһ��
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
			CAN_init();//CAN��ʼ��
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

void CAN_reset(void)//CAN���踴λ����
{
	NVIC_DisableIRQ(CAN_IRQn);
	*((uint32_t*)0x40048004)&=~(1<<3);//��λCAN���Ĵ�����PRESETCTRL
#if (__ICCARM__)
    asm("nop");
    asm("nop");
#else
	__nop();
	__nop();
#endif
	*((uint32_t*)0x40048004)|=(1<<3);//��λCAN��Ч
}
void CAN_init(void) //CAN���߳�ʼ��
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
	};//����Ƭ�ϻص������ṹ�����
	CCAN_MSG_OBJ_T config_msg;//��Ϣ����ṹ��
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
	baudrateCalculate(TEST_CCAN_BAUD_RATE, CanApiClkInitTable);//��λʱ������д��CanApiClkInitTable����
 	LPC_CCAN_API->init_can(&CanApiClkInitTable[0], TRUE);//����Ƭ�Ϻ�����CanApiClkInitTable�����е���д��CANCLKDIV �Ĵ�����CAN_BTR�Ĵ�����ͬʱ����CAN�ж�
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
 	can_api_timing_cfg[0] = 9; //������Ԥ��Ƶ�Ĵ���
	
	/*           λ��ʱ�Ĵ���                                         */
	/*      Tq=5us,10k,������ǰ��14�������5��ͬ����1�������㣺75%    */
// 	can_api_timing_cfg[1] = 0x4dd7;
	
	/*           λ��ʱ�Ĵ���                                        */
	/*      Tq=5us,10k,������ǰ��15�������4��ͬ����1�������㣺80%   */
 	can_api_timing_cfg[1] = 0x3ed7;
	
	//�ٷ���λʱ����㷽�����ڸ��ӣ�ע����������������ֱֵ��д�Ĵ���
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
//mod  = 0:IDδ���䣬Ĭ��63��
//��ʱ����
/**************************************************/
void can_data_send(uint8_t mod,uint8_t *can_frame)  //mod:ID�Ƿ���� //������������
{		
	uint8_t ID_Mod;
	CCAN_MSG_OBJ_T msg_obj_s;
	
//	uint8_t Data0_State = 0;
	ID_Mod = mod;
/////////////////////////IDδ����//////////////////////////////	
	if(ID_Mod == 0)                           //IDδ����,Ĭ��63
	{
		msg_obj_s.mode_id = (CAN_MSGOBJ_STD|ID10_09_PeriodSend|ID08_07_Equipments|ID05_00_DefaultID);
		msg_obj_s.dlc     = 4;
		msg_obj_s.data[0] = chip_ID%256;
		msg_obj_s.data[1] = (chip_ID-msg_obj_s.data[0])>>8%256;
		msg_obj_s.data[2] = (chip_ID-(unsigned int)msg_obj_s.data[1]*256-msg_obj_s.data[0])>>16%256;
		msg_obj_s.data[3] = (chip_ID-(unsigned int)msg_obj_s.data[2]*65536-(unsigned int)msg_obj_s.data[1]*256-msg_obj_s.data[0])>>24%256;;
		
	}
///////////////////////////////////////////////////////////////	
/////////////////////////ID�ѷ���//////////////////////////////	
	else if(ID_Mod == 1)                      //ID�ѷ���
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

//////////////////////��㡢�������ݴ��ݺ���////////////////////////////////

void CANSend_AMBESetSine(uint8_t soundflag)
{
	uint8_t SoundID;
	CCAN_MSG_OBJ_T msg_obj_s;
	SoundID = soundflag;
	

	switch(SoundID)
	{
		case 1: //��㿪ʼ
			msg_obj_s.mode_id = (CAN_MSGOBJ_STD|ID10_09_BSMConfirm|ID08_07_Soundcodes|CAN_ID);
			msg_obj_s.dlc     = 8;
			msg_obj_s.data[0] = 0x11; //��ʼ���ݣ�0x11
			msg_obj_s.data[1] = 0x11;
		break;
		
		case 2: //������
			msg_obj_s.mode_id = (CAN_MSGOBJ_STD|ID10_09_BSMConfirm|ID08_07_Soundcodes|CAN_ID);
			msg_obj_s.dlc     = 8;
			msg_obj_s.data[0] = 0x22; //�������ݣ�0x22
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

////////////////////////////��������///////////////////////////	


void can_rcv(CCAN_MSG_OBJ_T can_rcv_dat)
{
	CCAN_MSG_OBJ_T CAN_RD_Message ;
	uint32_t CAN_RD_ID;
	uint32_t CAN_RD_ID1;
	CCAN_MSG_OBJ_T msg_obj_s;
	
	CAN_RD_Message = can_rcv_dat;		
	
	if((CAN_RD_Message.mode_id & CAN_MSGOBJ_STD)==CAN_MSGOBJ_STD)  //��׼֡�ж�
	{
		//�����������������
		CAN_RD_ID = CAN_RD_Message.mode_id & ((uint16_t)0x3<<9);
		if (CAN_RD_ID==ID10_09_OrderToBSM)
		{
			//������������
			//�㲥��������
			
			/***********************�����ã��ϵ�ع���ָ��**************************/
			if((CAN_RD_Message.data[0] == 0x55)&&(CAN_RD_Message.data[1] == 0xaa))
			{
				POWEROFF_State_Close();
			}
			/************************************************************************/
			
			
			//////////////////ID���䣬��������////////////////////////////			
			if(((px_flag == 0)||((CAN_ID&0x3f)==0x3f))&&(flag_18ON==0))         //�ж�֮ǰû������
			{
				if((CAN_RD_Message.data[0] == 0x2a)&&(CAN_RD_Message.data[1] == 0xff)) //�޸�ID����
				{
					uint32_t chip;
					chip=CAN_RD_Message.data[2]+(uint32_t)(CAN_RD_Message.data[3]<<8)+(uint32_t)(CAN_RD_Message.data[4]<<16)+(uint32_t)(CAN_RD_Message.data[5]<<24);
					if (chip==0)
					{
						if((CAN_RD_Message.data[6] >= 1)&&(CAN_RD_Message.data[6] <= 60))   //ID������1-60֮��
						{
							px_flag = 1;     //�������
							CAN_ID = CAN_RD_Message.data[6];

							//CAN_MSG[can_send_len]=msg_obj_s;
							//mod_flag[x]=1��Ӧ�𱾻�ID DLC=0��
							mod_flag[can_send_len]=1;
							if (can_send_len<20)
								can_send_len++;
							JDQ_State_Close();//�̵������ϣ�����һ������
							#if NEW_BS
							POWEROFF_State_Open();//��ع����ͨ
							#endif
							reset_flag=true;
							send_px_flag=false;
							
							
						}
						
					}//if chip=0;
				}//if((CAN_RD_Message.data[0] == 0x2a)&&(CAN_RD_Message.data[1] == 0xff)) //�޸�ID����
				return ;  //ͬһ֡ ������ͬʱЯ�������Ϣ����ˣ����˷���,Ϊ����ߴ���Ч��
			}//if(px_flag == 0)         //�ж�֮ǰû������
			
			if((px_flag==1)&&(flag_18ON==0))//else
			{
			//�Ѿ�������,��ʱ�������Ѿ��������Ļ��������
				if((CAN_RD_Message.data[0] == 0x01)&&(CAN_RD_Message.data[1] == 0xff)) //��ʱ����
				{
					//��ʱ
					year=CAN_RD_Message.data[2];
					month=CAN_RD_Message.data[3];
					day=CAN_RD_Message.data[4];
					hour=CAN_RD_Message.data[5];
					minute=CAN_RD_Message.data[6];
					second=CAN_RD_Message.data[7];
					flag_1000ms_count=0;
					return;
				}//if((CAN_RD_Message.data[0] == 0x2a)&&(CAN_RD_Message.data[1] == 0xff)) //��ʱ����
			
				if((CAN_RD_Message.data[0] == 0x2a)&&(CAN_RD_Message.data[1] == 0xff)) //�޸�ID����
				{
					uint32_t chip;
					chip=CAN_RD_Message.data[2]+(uint32_t)(CAN_RD_Message.data[3]<<8)+(uint32_t)(CAN_RD_Message.data[4]<<16)+(uint32_t)(CAN_RD_Message.data[5]<<24);
					if (chip==chip_ID)
					{
						if((CAN_RD_Message.data[6] >= 1)&&(CAN_RD_Message.data[6] <= 60))   //ID������1-62֮��
						{
							
							CAN_ID = CAN_RD_Message.data[6];
							JDQ_State_Close();//�̵������ϣ�����һ������

							//mod_flag[x]=1��Ӧ�𱾻�ID DLC=0��
							mod_flag[can_send_len]=1;
							if (can_send_len<20)
								can_send_len++;
							
						}
						return ;  //ͬһ֡ ������ͬʱЯ�������Ϣ����ˣ����˷���,Ϊ����ߴ���Ч��
					}
				}//if((CAN_RD_Message.data[0] == 0x2a)&&(CAN_RD_Message.data[1] == 0xff)) //�޸�ID����
			}//if(px_flag == 0)  else       //������
			
			
			//���´����յ��Ĺ㲥֡ 
			if((CAN_RD_Message.data[0] == 0x02)&&(CAN_RD_Message.data[1] == 0xff)) //ֹͣ��������
			{
				heart_flag=false;
				return;
			}
			if((CAN_RD_Message.data[0] == 0x03)&&(CAN_RD_Message.data[1] == 0xff)) //�ָ���������
			{
				heart_flag=true;
				return;
			}
			#if NEW_BS
			if((CAN_RD_Message.data[0] == 0x04)&&(CAN_RD_Message.data[1] == 0xff)) //���flash�洢����
			{
				//��flash�ռ�洢��գ�������1����2����Ϊ0
				
				//mod_flag[x]=3�����flash���� DLC=0��
				mod_flag[can_send_len]=3;
				if (can_send_len<20)
					can_send_len++;
				return;
			}
			#endif
			//���´�����Ա���������
				//////////////////////////////////////////////////////////////		
			CAN_RD_ID = CAN_RD_Message.mode_id & 0x7f;//ȡ����can�ڵ��CAN_ID
			if(CAN_RD_ID == CAN_ID)   //����ǶԱ����������������CAN_IDֵ���)
			{
			//////////////////�����绰��������////////////////////////////////

				if((CAN_RD_Message.data[0] == 0x2b)&&(CAN_RD_Message.data[1] == 0xff))       //�޸ĳ�����
				{
					#if NEW_BS
					Set_Charge(CAN_RD_Message.data[2]);
					#endif
					//mod_flag[x]=1��Ӧ�𱾻�ID DLC=0��
					mod_flag[can_send_len]=1;
					if (can_send_len<20)
						can_send_len++;
					return;
				}
				//////////////////��ȡ������ʷ��¼�����////////////////////////////////
				#if NEW_BS
				if((CAN_RD_Message.data[0] == 0x05)&&(CAN_RD_Message.data[1] == 0xff))       //��ȡ��ʷ��¼
				{
					//mod_flag[x]=4����ȡ��ʷ��¼���� DLC=0��
					mod_flag[can_send_len]=4;
					if (can_send_len<20)
						can_send_len++;

					
					return;
				}//if((CAN_RD_Message.data[0] == 0x05)&&(CAN_RD_Message.data[1] == 0xff))       //��ȡ��ʷ��¼
				#endif
				if((CAN_RD_Message.data[0] == 0x6)&&(CAN_RD_Message.data[1] == 0xff))       //��chipid
				{
					

					//mod_flag[x]=2����ȡchipID���� DLC=4��
					mod_flag[can_send_len]=2;
					if (can_send_len<20)
						can_send_len++;	
					return;
					
				}//if((CAN_RD_Message.data[0] == 0x6)&&(CAN_RD_Message.data[1] == 0xff))       //��chipid
				//////////////////���������������������////////////////////////////////
				#if NEW_BS
				if((CAN_RD_Message.data[0] == 0x2c)&&(CAN_RD_Message.data[1] == 0xff))       //�޸ĳ�����
				{
					Set_Volume(CAN_RD_Message.data[2]);

					//mod_flag[x]=1��Ӧ������ DLC=0��
					mod_flag[can_send_len]=1;
					if (can_send_len<20)
						can_send_len++;
					return;
				}//if((CAN_RD_Message.data[0] == 0x2c)&&(CAN_RD_Message.data[1] == 0xff))       //�޸ĳ�����
				#endif
				
			}//if(CAN_RD_ID == CAN_ID)   //����ǶԱ����������������CAN_IDֵ���)
//////////////////////////////////////////////////////////////	

		
		}//if (CAN_RD_ID==ID10_09_OrderToBSM)
//////////////////////////////////////////////////////////////	
		
//////////////////���������////////////////////////////////
		CAN_RD_ID = CAN_RD_Message.mode_id & (0x3<<7);//canid ��7~8λȷ����������
		if (CAN_RD_ID==ID08_07_Soundcodes)
		{
			uint8_t tmpi;
			//CAN_RD_ID = CAN_RD_ID>>7;
			CAN_RD_ID1=CAN_RD_Message.mode_id&0x7f;
			//if((CAN_RD_ID == 0X03)&&(CAN_RD_ID1!=CAN_ID))   //�������������
//			if(CAN_RD_ID1!=CAN_ID)
//			{
				
				can_sound_time=0;
				#if NEW_BS
				for (tmpi=0;tmpi<8;tmpi++)
				{
					can_sound_buf[tmpi]=CAN_RD_Message.data[tmpi];//ȡ����
					//can_sound_buf_count++;
				}
				#endif
				can_sound_flag=true;//�յ���������֡
				flag_5s=0;
				flag_5s_count=0;
				return;
					
//			}//if(CAN_RD_ID == 0X03)   //�������������
		}
		
//////////////////////////////////////////////////////////////	
		
		
		
		
		
	}//if((CAN_RD_Message.mode_id & CAN_MSGOBJ_STD)==CAN_MSGOBJ_STD)  //��׼֡�ж�
}

	
