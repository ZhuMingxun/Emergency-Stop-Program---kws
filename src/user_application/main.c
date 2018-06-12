
#include "cari_config.h"
#include "user_config.h"
#include "ambe.h"
#include "t16b0.h"
#include "speak.h"
#include "can.h"
#include "AT24C16.h"
#include "BspIapDriver.h"
#include "watchDog.h"
#include "adc.h"
#define D18V_LOST_TIMEOUT  28800  //单位：秒，18V掉电后，沿线可能还需要进行语音对讲，扩音电话工作时间8小时
//#define DEBUG_ENABLE

#define ENABLE_WATCH_DOG 0
#define ENABLE_AT24C16 1
#define DEBUG_CAN 0
#define CAN_SOUND_TIMEOUT 10


////函数声明
void SysTick_Handler(void);								//系统节拍中断处理函数
void Device_Init(void);										//组件初始化
void HardFault_Handler(void )
{
	NVIC_SystemReset();
}
//void Key_process(uint8_t Scanvalue);


/////系统消息结构体变量
MSG_TypeDef  msg;


#define QUITIME  5000	 //按键退出时间，单位ms

#if DEBUG_UART_C
uint8_t flag_18off=0;//用于串口打印，在20ms部分，第一次进入18V掉电程序时，打印完后置1，18V恢复后清零
#endif


uint16_t error=0;
extern bool can_sound_flag;//收到语音标志
bool mute_flag;//功放打开标志。 
///////////////////////////////////////////////////////////
CCAN_MSG_OBJ_T msg_obj;	
///////////////////////////////////////////////////////////

int reset_count;
bool is_YY_Alive;// 扩音电话是否存在标志。由于不是所有的闭锁都有扩音电话.
bool reset_flag1;
uint16_t ADC_value;
uint8_t flag_18ON=1;
bool D18V_IN_flag;//18V电源检测到
uint8_t D18V_IN_count;//18V检测到计数器，用于检测到18V  2s之后送上心跳
//bool watchdog_reset_times;  未使用
bool key1_flag;//对讲按键按下标志
bool key2_flag;//打点按键按下标志
bool is_YY_Init;//扩音电话初始化标志。上电延时后初始化，希望用于解决ambe1000上电初始化不能用的问题。
	
/*********************增加定义的变量 - 2018.5.25 - zhumx ***************************/
uint8_t g_d18_onlinetime_cnt;//沿线得电时间计数器，1Hz自增
bool g_battsupply_enable = false;//电池供电使能
#define D18V_STAT_ON    1
#define D18V_STAT_OFF   2
uint8_t d18v_stat=D18V_STAT_ON;

/***********************************************************************************/
	
	
	
int main(void)
{	
	//char *p;
	

	uint8_t key_count;		 //按键次数统计变量,用于分析是不是有人在这儿操作，进一步实现在电力不够的情况下降低播放功耗

	bool jt_flag; //急停按键标志
	bool pp1_flag;//一级跑偏标志
	bool pp2_flag;//二级跑偏标志
	
	
	bool is_AT24C16_Alive;
	//at24c16有2048个字节容量。
	//其中：地址【0】用于测试，当【0】==0x88时，表示存在。
	//【1】、【2】用于存放记录的索引。[1]为低位地址，【2】为高位地址
	//每次记录月、日、分、时、秒、事件  6个字节
	//事件1 表示操作闭锁
	//事件2 表示释放闭锁
	//事件3 表示操作打点
	//事件4 表示操作对讲
	//事件5 表示一级跑偏
	//事件6 表示二级跑偏
	//事件7 表示一级跑偏撤消
	//事件8 表示二级跑偏撤消
	uint16_t record_address;//事件记录地址，保存事件的地址，存于at24c16第2、3个字节中。当地址==2000时，从3开始。


	uint8_t jt_count; //急停按键按下计数器，用于消抖动
	uint8_t pp1_count;//一级跑偏闭锁计数器，用于消抖动
	uint8_t pp2_count;//二级跑偏闭锁计数器，用于消抖动
	uint32_t D18V_Lost_count;//18V掉电时间计数器
	uint8_t D18V_LOST_FAST_count;//用于解决短暂掉电不能发送排位数据的问题。

	extern unsigned int chip_ID;//芯片唯一码
	uint32_t reset_id;//复位原因
	CCAN_MSG_OBJ_T msg;//主程序范围内的can帧全局变量
	reset_flag=false;//如果上电发现是通电上电时，resetid.0==1,那么复位。如果reset_count==15（1.5秒）时，还没有取得CAN_ID，那么再复位一次。
	reset_flag1=false;
	reset_count=0;
	D18V_LOST_FAST_count=0;
	//系统初始化
	SystemCoreClockUpdate();
	//读取芯片唯一码，只能在这个位置读
	IAP_Entry = (void(*)())IAP_ENTER_ADR;
	chip_ID=ReadUID();
	reset_id=Chip_SYSCTL_GetSystemRSTStatus();//获取复位原因
	Chip_SYSCTL_ClearSystemRSTStatus(reset_id);
	Board_Init();
	//须在此函数后调用DEBUGOUT()，否则会死机。

//	DEBUGOUT("reset reason:0x%x\r\n",reset_id);//打印复位原因
	Chip_GPIO_Init(LPC_GPIO);	
  Gpio_Init();//IO初始状态，继电器、跑偏、急停等
//	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 3, 2);//用于测试定时器
	
	#if NEW_BS
	//初始化未完成，将电池电断开
//	POWEROFF_State_Close();
//	if((reset_id&0x03)!=0x0)
//	{
//		//该单片机是1.8V到3.6V工作，而闭锁采用监控分院的软启动电源供电，上电冲击小但上电速度也比较慢，因此，需要做一次复位。为了保障电源软启动模块已经正常工作。
//		uint32_t tmpi;
//		for (tmpi=0;tmpi<0xff;tmpi++);//延时
////		NVIC_SystemReset();	//如果是上电复位，那么重新复位一次,为了解决目前存在的遗留问题
//	}//在电路上已解决此问题，因此注掉，孔维社，20170725
	#endif
	Device_Init();

		
	#if ENABLE_WATCH_DOG
	watchdog_INIT(1);//初始化看门狗
	#endif

//	watchdog_reset_times=false;
	#if NEW_BS
	#if ENABLE_AT24C16
	
	if ((reset_id&0x04)==0x04)
	{
		//看门狗复位
		uint8_t wd_reset_times;//看门狗连续复位次数
		record_address=(uint16_t)AT24C16_ReadOneByte(1)+((uint16_t)AT24C16_ReadOneByte(2)<<8);
		AT24C16_WriteOneByte(record_address,month);
		record_address++;
		AT24C16_WriteOneByte(record_address,day);
		record_address++;
		AT24C16_WriteOneByte(record_address,hour);
		record_address++;
		AT24C16_WriteOneByte(record_address,minute);
		record_address++;
		AT24C16_WriteOneByte(record_address,second);
		record_address++;
		AT24C16_WriteOneByte(record_address,9);//9为看门狗复位事件
		record_address++;
		if (record_address>=1980+3)//从地址3开始记录,类似行车记录仪，每当记录330*6=1980  330组数据后，就从头再开始
			record_address=3;
//		//保存地址
		AT24C16_WriteOneByte(1,record_address%256);
		AT24C16_WriteOneByte(2,record_address/256);
		
		//读看门狗连续复位次数
		//BYTE 2003 看门狗复位次数。如果看门狗连续复位超过10次都无法获得闭锁排序序号，那么不再继续申请排序。接通下一级。这一位就是如果正常启动，那么执行清零。如果看门狗复位，那么+1.
		wd_reset_times=AT24C16_ReadOneByte(2003);
		if (wd_reset_times==10)
		{
//			watchdog_reset_times=true;
			px_flag=1;
			CAN_ID=0;
			JDQ_State_Close();//继电器吸合，给下一级供电
		}
		else
		{
			//加1写回去
			AT24C16_WriteOneByte(2003,wd_reset_times+1);
		}
	}
	else  //if ((reset_id&0x04)==0x04)
	{
		//不为看门狗复位，那么清0
			AT24C16_WriteOneByte(2003,0);
	}
	#endif
	#endif
	//Chip_GPIO_Init(LPC_GPIO);
	is_YY_Init=false;
	#if NEW_BS
	SBoard_Init();//语音相关引脚初始化
//	Mute_OFF();//打开功放
	#endif
	
	//变量初始化
	key1_flag=false; 
	key2_flag=false;//打点按键按下标志
	D18V_Lost_count=0;
	jt_flag=false;
	jt_count=0;
	pp1_count=0;
	pp1_flag=false;
	pp2_count=0;
	pp2_flag=0;
	mute_flag=false;
	D18V_IN_flag=false;
	D18V_IN_count=0;
	is_AT24C16_Alive=false;
	
	reset_flag1=true;	
		
	TIM16B0_INT_init(5); //5ms
	
	flag_1000ms=0;
	flag_1000ms_count=0;
//	while(flag_1000ms==0){}//延时500ms
	
	#if NEW_BS
Mute_ON();    //功放关闭，静音			
//		if ((key2_flag==false)||(can_sound_flag==false))//key2_flag：打点按下
//	{
//			msg.data[1]=ADC_IRQ_Value_final%256;
//			msg.data[0]&=0x3f;//清除msg.data[0]的最高两位
//			msg.data[0]|=(((ADC_IRQ_Value_final-msg.data[1])>>2)&0xc0);
//			if ((ADC_IRQ_Value_final/1024)*3.3*4>2.0)
//			{
//				is_YY_Alive=true;		
//				Mute_ON();    //功放关闭，静音			
//			}
//			else
//				is_YY_Alive=false;
//	}//此处的判断扩音电话是否存在的标志放到主循环中，孔维社，2017.7.30
	
	#if ENABLE_AT24C16
	if (AT24C16_Check()==0)
		is_AT24C16_Alive=true;
	#endif
	#endif
	
	
	TIM16B0_INT_init(5); //5ms
	flag_1000ms=0;
	flag_1000ms_count=0;
	#if ENABLE_WATCH_DOG
	watchdog_INIT(200);//初始化看门狗
	#endif 
	
	#if NEW_BS
	//初始化未完成，将电池电断开
//	POWEROFF_State_Close();
	#endif
	
	reset_count=0;
//	DEBUGOUT("start!");
	/*初始化完成后采集一次电池电压，以后在打点或语音播放时采集*/
	msg.data[1]=ADC_IRQ_Value_final%256;
	msg.data[0]&=0x3f;//清除msg.data[0]的最高两位
	msg.data[0]|=(((ADC_IRQ_Value_final-msg.data[1])>>2)&0xc0);
	ADC_value=ADC_IRQ_Value_final;
	
	while (1) 
	{	
		watchdog_feed_flag=true;
		flag_18ON=D18V_State();	
	//Chip_WWDT_Feed(LPC_WWDT);
	//	while (1);
	/*判断是否有语音*/
	if ((key2_flag==true)||(can_sound_flag==true))//打点按下或有语音播放时采集电池电压
	{
		msg.data[1]=ADC_IRQ_Value_final%256;
		msg.data[0]&=0x3f;//清除msg.data[0]的最高两位
		msg.data[0]|=(((ADC_IRQ_Value_final-msg.data[1])>>2)&0xc0);
		ADC_value=ADC_IRQ_Value_final;

	}
    
    if ((((float)ADC_IRQ_Value_final)/1024)*3.3*4>2.0)
    {
        is_YY_Alive=true;		
    }
    else
    {
        is_YY_Alive=false;
        is_YY_Init = false;
    }

		//处理急停按键
	if (flag_10ms==1)
	{
		flag_10ms=0;
		if (Chip_GPIO_GetPinState(LPC_GPIO, JT_PORT, JT_PORT_BIT)==1)
		{			 
			jt_flag=true;				
			if(jt_count<15)jt_count++;//每10ms读一次闭锁状态			
		}
		else if ((jt_flag==true)&&(Chip_GPIO_GetPinState(LPC_GPIO, JT_PORT, JT_PORT_BIT)==0))
		{
			 
			if (jt_count>0)
				jt_count--;//出现抖动或者已经松开闭锁
			if (jt_count==0)
			{
				if ((msg.data[0]&0x01)==0x01)
				{//撤消闭锁
					#if NEW_BS
					if (is_AT24C16_Alive==true)
					{
						//at24c16存在
						//at24c16有2048个字节容量。
						//其中：地址【0】用于测试，当【0】==0x88时，表示存在。
						//【1】、【2】用于存放记录的索引。[1]为低位地址，【2】为高位地址
						//每次记录月、日、分、时、秒、事件  6个字节
						//事件1 表示操作闭锁
						//事件2 表示释放闭锁
						//事件3 表示操作打点
						//事件4 表示操作对讲
						//事件5 表示一级跑偏
						//事件6 表示二级跑偏
						//事件7 表示一级跑偏撤消
						//事件8 表示二级跑偏撤消
						record_address=(uint16_t)AT24C16_ReadOneByte(1)+((uint16_t)AT24C16_ReadOneByte(2)<<8);
						AT24C16_WriteOneByte(record_address,month);
						record_address++;
						AT24C16_WriteOneByte(record_address,day);
						record_address++;
						AT24C16_WriteOneByte(record_address,hour);
						record_address++;
						AT24C16_WriteOneByte(record_address,minute);
						record_address++;
						AT24C16_WriteOneByte(record_address,second);
						record_address++;
						AT24C16_WriteOneByte(record_address,2);//2为闭锁释放事件
						record_address++;
						if (record_address>=1980+3)//从地址3开始记录,类似行车记录仪，每当记录330*6=1980  330组数据后，就从头再开始
							record_address=3;
						//保存地址
						AT24C16_WriteOneByte(1,record_address%256);
						AT24C16_WriteOneByte(2,record_address/256);
					}//if (is_AT24C16_Alive==true) at24c16存在
					#endif
					msg.mode_id=ID10_09_ChangeSend|ID08_07_Equipments|CAN_ID;//需要发送canid
					msg.data[0]&=0xfe;
					msg.dlc=2;
					can_send(&msg);//变化发送：闭锁撤销
					flag_1500ms_count = 0;
					flag_1500ms = 0;//将定时发送标志清零
				}//if ((msg.data[0]&0x01)==0x01)
				
				jt_flag=false;//清除急停标志位
				
			}
		}
		if (((jt_count>14)||(jt_count==0))&&(jt_flag==true))
		{
			msg.mode_id=ID10_09_ChangeSend|ID08_07_Equipments|CAN_ID;//需要发送canid
			
			if (jt_count>0)
			{
				//发送闭锁信号
				//jt_count=0;
				if ((msg.data[0]&0x01)==0x00)//实现不变不传，如果在此之前已经处于闭锁状态，那么msg.data[0].0位应该已经置1了
				{//实现变化发送
					msg.data[0]|=0x01;//将data[0].0置位为1，表示有闭锁，用于心跳数据或者急停状态发送。
					msg.dlc=2;
					can_send(&msg);//急停状态发送
					flag_1500ms_count = 0;
					flag_1500ms = 0;//将定时发送标志清零
					#if NEW_BS
					//增加i2c记录
					if (is_AT24C16_Alive==true)
					{
						//at24c16存在
						//at24c16有2048个字节容量。
						//其中：地址【0】用于测试，当【0】==0x88时，表示存在。
						//【1】、【2】用于存放记录的索引。[1]为低位地址，【2】为高位地址
						//每次记录月、日、分、时、秒、事件  6个字节
						//事件1 表示操作闭锁
						//事件2 表示释放闭锁
						//事件3 表示操作打点
						//事件4 表示操作对讲
						//事件5 表示一级跑偏
						//事件6 表示二级跑偏
						//事件7 表示一级跑偏撤消
						//事件8 表示二级跑偏撤消
						record_address=(uint16_t)AT24C16_ReadOneByte(1)+((uint16_t)AT24C16_ReadOneByte(2)<<8);
						AT24C16_WriteOneByte(record_address,month);
						record_address++;
						AT24C16_WriteOneByte(record_address,day);
						record_address++;
						AT24C16_WriteOneByte(record_address,hour);
						record_address++;
						AT24C16_WriteOneByte(record_address,minute);
						record_address++;
						AT24C16_WriteOneByte(record_address,second);
						record_address++;
						AT24C16_WriteOneByte(record_address,1);//1为闭锁事件
						record_address++;
						if (record_address>=1980+3)//从地址3开始记录,类似行车记录仪，每当记录330*6=1980  330组数据后，就从头再开始
							record_address=3;
						//保存地址
						AT24C16_WriteOneByte(1,record_address%256);
						AT24C16_WriteOneByte(2,record_address/256);
					}//if (is_AT24C16_Alive==true) 闭锁存在
					#endif
				}//if ((msg.data[0]&0x01)==0x00)//实现不变不传，如果在此之前已经处于闭锁状态，那么msg.data[0].0位应该已经置1了
			}//
		}// 处理急停按键
	}	
				
	//处理一级跑偏
	if(flag_10ms_p1==1)
	{	
		flag_10ms_p1=0;
		if (Chip_GPIO_GetPinState(LPC_GPIO, PP1_PORT, PP1_PORT_BIT)==false)
		{
			pp1_flag=true;
			if(pp1_count<15)pp1_count++;//每10ms读一次一级跑偏状态				 
		}
		else if ((pp1_flag==true)&&(Chip_GPIO_GetPinState(LPC_GPIO, PP1_PORT, PP1_PORT_BIT)==true))
		{
			if (pp1_count>0)
				pp1_count--;//出现抖动或者已经松开一级跑偏
			if (pp1_count==0)
			{
				if ((msg.data[0]&0x02)==0x02)
				{
					#if NEW_BS
					if (is_AT24C16_Alive==true)
					{
						//at24c16存在
						//at24c16有2048个字节容量。
						//其中：地址【0】用于测试，当【0】==0x88时，表示存在。
						//【1】、【2】用于存放记录的索引。[1]为低位地址，【2】为高位地址
						//每次记录月、日、分、时、秒、事件  6个字节
						//事件1 表示操作闭锁
						//事件2 表示释放闭锁
						//事件3 表示操作打点
						//事件4 表示操作对讲
						//事件5 表示一级跑偏
						//事件6 表示二级跑偏
						//事件7 表示一级跑偏撤消
						//事件8 表示二级跑偏撤消
						record_address=(uint16_t)AT24C16_ReadOneByte(1)+((uint16_t)AT24C16_ReadOneByte(2)<<8);
						AT24C16_WriteOneByte(record_address,month);
						record_address++;
						AT24C16_WriteOneByte(record_address,day);
						record_address++;
						AT24C16_WriteOneByte(record_address,hour);
						record_address++;
						AT24C16_WriteOneByte(record_address,minute);
						record_address++;
						AT24C16_WriteOneByte(record_address,second);
						record_address++;
						AT24C16_WriteOneByte(record_address,7);//7为一级跑偏释放事件
						record_address++;
						if (record_address>=1980+3)//从地址3开始记录,类似行车记录仪，每当记录330*6=1980  330组数据后，就从头再开始
							record_address=3;
						//保存地址
						AT24C16_WriteOneByte(1,record_address%256);
						AT24C16_WriteOneByte(2,record_address/256);
					}//if (is_AT24C16_Alive==true) 闭锁存在
					#endif
					msg.mode_id=ID10_09_ChangeSend|ID08_07_Equipments|CAN_ID;//需要发送canid
					msg.data[0]&=0xfd;
					msg.dlc=2;
					can_send(&msg);//变化发送：一级跑偏撤销
					flag_1500ms_count = 0;
					flag_1500ms = 0;//将定时发送标志清零
				}//if msg.data[0]&0x02)==0x02
				
				pp1_flag=false;//清除个级跑偏标志位
				//增加i2c记录
				 
			}
		}
		if (((pp1_count>14)||(pp1_count==0))&&(pp1_flag==true))
		{
			msg.mode_id=ID10_09_ChangeSend|ID08_07_Equipments|CAN_ID;//需要发送canid
			if (pp1_count>0)
			{
				//发送一级跑偏信号
//				pp1_count=0;
				if ((msg.data[0]&0x02)==0x00)//实现不变不传，如果在此之前已经处于闭锁状态，那么msg.data[0].1位应该已经置1了
				{//实现变化发送
					msg.data[0]|=0x02;//将data[0].1置位为1，表示有一级跑偏，用于心跳数据或者急停状态发送。
					msg.dlc=2;
					can_send(&msg);//一级跑偏状态发送
					flag_1500ms_count = 0;
					flag_1500ms = 0;//将定时发送标志清零
					//增加i2c记录
					#if NEW_BS
					if (is_AT24C16_Alive==true)
					{
						//at24c16存在
						//at24c16有2048个字节容量。
						//其中：地址【0】用于测试，当【0】==0x88时，表示存在。
						//【1】、【2】用于存放记录的索引。[1]为低位地址，【2】为高位地址
						//每次记录月、日、分、时、秒、事件  6个字节
						//事件1 表示操作闭锁
						//事件2 表示释放闭锁
						//事件3 表示操作打点
						//事件4 表示操作对讲
						//事件5 表示一级跑偏
						//事件6 表示二级跑偏
						//事件7 表示一级跑偏撤消
						//事件8 表示二级跑偏撤消
						record_address=(uint16_t)AT24C16_ReadOneByte(1)+((uint16_t)AT24C16_ReadOneByte(2)<<8);
						AT24C16_WriteOneByte(record_address,month);
						record_address++;
						AT24C16_WriteOneByte(record_address,day);
						record_address++;
						AT24C16_WriteOneByte(record_address,hour);
						record_address++;
						AT24C16_WriteOneByte(record_address,minute);
						record_address++;
						AT24C16_WriteOneByte(record_address,second);
						record_address++;
						AT24C16_WriteOneByte(record_address,5);//5为一级跑偏事件
						record_address++;
						if (record_address>=1980+3)//从地址3开始记录,类似行车记录仪，每当记录330*6=1980  330组数据后，就从头再开始
							record_address=3;
						//保存地址
						AT24C16_WriteOneByte(1,record_address%256);
						AT24C16_WriteOneByte(2,record_address/256);
					}//if (is_AT24C16_Alive==true) 闭锁存在
					#endif
					
				}
				 
			}
		}
	}//处理一级跑偏
		
		//处理二级跑偏	
		if(flag_10ms_p2==1)
		{
			flag_10ms_p2=0;
		if (Chip_GPIO_GetPinState(LPC_GPIO, PP2_PORT, PP2_PORT_BIT)==0)
		{
			pp2_flag=true;
			if (pp2_count<15)
				pp2_count++;//每10ms读一次二级跑偏状态
				 
			
		}
		else if ((pp2_flag==true)&&(Chip_GPIO_GetPinState(LPC_GPIO, PP2_PORT, PP2_PORT_BIT)==1))
		{
			if (pp2_count>0)
				pp2_count--;//出现抖动或者已经松开二级跑偏
			if (pp2_count==0)
			{
				if ((msg.data[0]&0x04)==0x04)
				{
					#if NEW_BS
					if (is_AT24C16_Alive==true)
					{
						//at24c16存在
						//at24c16有2048个字节容量。
						//其中：地址【0】用于测试，当【0】==0x88时，表示存在。
						//【1】、【2】用于存放记录的索引。[1]为低位地址，【2】为高位地址
						//每次记录月、日、分、时、秒、事件  6个字节
						//事件1 表示操作闭锁
						//事件2 表示释放闭锁
						//事件3 表示操作打点
						//事件4 表示操作对讲
						//事件5 表示一级跑偏
						//事件6 表示二级跑偏
						//事件7 表示一级跑偏撤消
						//事件8 表示二级跑偏撤消
						record_address=(uint16_t)AT24C16_ReadOneByte(1)+((uint16_t)AT24C16_ReadOneByte(2)<<8);
						AT24C16_WriteOneByte(record_address,month);
						record_address++;
						AT24C16_WriteOneByte(record_address,day);
						record_address++;
						AT24C16_WriteOneByte(record_address,hour);
						record_address++;
						AT24C16_WriteOneByte(record_address,minute);
						record_address++;
						AT24C16_WriteOneByte(record_address,second);
						record_address++;
						AT24C16_WriteOneByte(record_address,8);//8为二级跑偏释放事件
						record_address++;
						if (record_address>=1980+3)//从地址3开始记录,类似行车记录仪，每当记录330*6=1980  330组数据后，就从头再开始
							record_address=3;
						//保存地址
						AT24C16_WriteOneByte(1,record_address%256);
						AT24C16_WriteOneByte(2,record_address/256);
					}//if (is_AT24C16_Alive==true) 闭锁存在
					#endif
					msg.mode_id=ID10_09_ChangeSend|ID08_07_Equipments|CAN_ID;//需要发送canid	
					msg.data[0]&=0xfb;
					msg.dlc=2;
					can_send(&msg);//变化发送：二级跑偏撤销
					flag_1500ms_count = 0;
					flag_1500ms = 0;//将定时发送标志清零
				}//if ((msg.data[0]&0xfb)==0x04)
				
				pp2_flag=false;//清除二级跑偏标志位
				
				}
				 
		}
		if (((pp2_count>14)||(pp2_count==0))&&(pp2_flag==true))
		{
			
			msg.mode_id=ID10_09_ChangeSend|ID08_07_Equipments|CAN_ID;//需要发送canid			
			if (pp2_count>0)
			{
				//发送二级跑偏信号
//				pp2_count=0;
				if ((msg.data[0]&0x04)==0x00)//实现不变不传，如果在此之前已经处于二级跑偏状态，那么msg.data[0].2位应该已经置1了
				{//实现变化发送
					msg.data[0]|=0x04;//将data[0].2置位为1，表示有二级跑偏，用于心跳数据或者急停状态发送。
					msg.dlc=2;
					can_send(&msg);//二级跑偏状态发送
					flag_1500ms_count = 0;
					flag_1500ms = 0;//将定时发送标志清零
					//增加i2c记录
					#if NEW_BS
					if (is_AT24C16_Alive==true)
					{
						//at24c16存在
						//at24c16有2048个字节容量。
						//其中：地址【0】用于测试，当【0】==0x88时，表示存在。
						//【1】、【2】用于存放记录的索引。[1]为低位地址，【2】为高位地址
						//每次记录月、日、分、时、秒、事件  6个字节
						//事件1 表示操作闭锁
						//事件2 表示释放闭锁
						//事件3 表示操作打点
						//事件4 表示操作对讲
						//事件5 表示一级跑偏
						//事件6 表示二级跑偏
						//事件7 表示一级跑偏撤消
						//事件8 表示二级跑偏撤消
						record_address=(uint16_t)AT24C16_ReadOneByte(1)+((uint16_t)AT24C16_ReadOneByte(2)<<8);
						AT24C16_WriteOneByte(record_address,month);
						record_address++;
						AT24C16_WriteOneByte(record_address,day);
						record_address++;
						AT24C16_WriteOneByte(record_address,hour);
						record_address++;
						AT24C16_WriteOneByte(record_address,minute);
						record_address++;
						AT24C16_WriteOneByte(record_address,second);
						record_address++;
						AT24C16_WriteOneByte(record_address,6);//6为二级跑偏事件
						record_address++;
						if (record_address>=1980+3)//从地址3开始记录,类似行车记录仪，每当记录330*6=1980  330组数据后，就从头再开始
							record_address=3;
						//保存地址
						AT24C16_WriteOneByte(1,record_address%256);
						AT24C16_WriteOneByte(2,record_address/256);
					}//if (is_AT24C16_Alive==true) 闭锁存在
					#endif
				}//if ((msg.data[0]&0x04)==0x00)//实现不变不传，如果在此之前已经处于二级跑偏状态，那么msg.data[0].2位应该已经置1了
			}
			 
		}
	}////处理二级跑偏
		#if NEW_BS
		if (is_YY_Init==true)
		{

			if(Get_Key1() == 1  )     //对讲事件开始
			{
				if ((can_sound_flag==false)&&(key2_flag==false))//只有在不放音以及没有打点的时候才可以讲话。//有机会测试全双工功能
					key1_flag=true;			
				 
			}
			else if (key1_flag==true)
			{
				key1_flag=false;
				//放音结束后重新读取电池电压，急停、跑偏数据
				msg.data[0]=0;
				if(jt_flag==false)msg.data[0]&=0xfe;
				else if(jt_flag==true)msg.data[0]|=0x01;
				if(pp1_flag==true)msg.data[0]|=0x02;
				else if(pp1_flag==false)msg.data[0]&=0xfd;
				if(pp2_flag==true)msg.data[0]|=0x04;
				else if(pp2_flag==false)msg.data[0]&=0xfb;
				msg.data[1]=ADC_value%256;
				msg.data[0]&=0x3f;//清除msg.data[0]的最高两位
				msg.data[0]|=(((ADC_value-msg.data[1])>>2)&0xc0);
				
//				Sine_Close();//打点关闭
				 
			}
			if((Get_Key2() == 1))       //打点事件开始
			{
				if ((can_sound_flag==false)&&(key1_flag==false))//只有在不放音的时候以及没有讲话的时候才可以打点
				{
					key2_flag=true;
//				Sine_Open();//打点开启
				}
				
			}
			else if ((key2_flag==true))
			{
				key2_flag=false;
				Sine_Close();//打点关闭
				 
			}
		}
		#endif
		if (can_send_len>0)
		{
			bool send_status;


			switch (mod_flag[0])
			{
				//为了不影响到主程序的运行，这里单独申请一个局部变量用于发送can数据
				CCAN_MSG_OBJ_T mod_can_msg;
				//mod_flag[x]=1：应答本机ID DLC=0。
				case 0x01:{
					mod_can_msg.mode_id = (CAN_MSGOBJ_STD|ID10_09_BSMConfirm|ID08_07_Equipments|CAN_ID); 
					mod_can_msg.dlc     = 0;
					mod_can_msg.msgobj=0;
					send_status=can_send(&mod_can_msg);
				};break;
				//mod_flag[x]=2：获取chipID记录 DLC=4。
				case 0x02:{
					mod_can_msg.mode_id = (CAN_MSGOBJ_STD|ID10_09_BSMConfirm|ID08_07_Equipments|CAN_ID); 
					mod_can_msg.dlc     = 4;
					mod_can_msg.data[0] = chip_ID%256;
					mod_can_msg.data[1] = (chip_ID-mod_can_msg.data[0])>>8%256;
					mod_can_msg.data[2] = (chip_ID-(unsigned int)mod_can_msg.data[1]*256-mod_can_msg.data[0])>>16%256;
					mod_can_msg.data[3] = (chip_ID-(unsigned int)mod_can_msg.data[2]*65536-(unsigned int)mod_can_msg.data[1]*256-mod_can_msg.data[0])>>24%256;;
					send_status=can_send(&mod_can_msg);
				};break;
				//mod_flag[x]=3：清除flash命令 DLC=0。
				#if NEW_BS
				case 0x03:{
					if (AT24C16_Check()==0)
					{
						AT24C16_WriteOneByte(1,3);
						AT24C16_WriteOneByte(2,0);
					}
				};break;
				//mod_flag[x]=4：获取历史记录命令 DLC=0。
				case 0x04:{
					//					CAN_RD_Message.data[0] = 0x00;
					if (AT24C16_Check()==0)
					{
						int len,tmpi;
						len=AT24C16_ReadOneByte(1)+((uint16_t)AT24C16_ReadOneByte(2)<<8);
						len=(len-3)/6;
						//先发送数据包起始及长度
						mod_can_msg.mode_id = (CAN_MSGOBJ_STD|ID10_09_BSMConfirm|ID08_07_Equipments|CAN_ID); 
						mod_can_msg.dlc     = 4;
						mod_can_msg.data[0]=0x44;
						mod_can_msg.data[1]=0x44;
						mod_can_msg.data[2]=len%256;//发送数据包数量的低位
						mod_can_msg.data[3]=len/256;//发送数据包数量的高位
						can_send(&mod_can_msg);
						for (tmpi=0;tmpi<len;tmpi++)
						{
							
							mod_can_msg.dlc     = 8;
							mod_can_msg.data[0]=0x55;
							mod_can_msg.data[1]=0x55;
							mod_can_msg.data[2]=AT24C16_ReadOneByte(tmpi*6+3);//月
							mod_can_msg.data[3]=AT24C16_ReadOneByte(tmpi*6+4);//日
							mod_can_msg.data[4]=AT24C16_ReadOneByte(tmpi*6+5);//时
							mod_can_msg.data[5]=AT24C16_ReadOneByte(tmpi*6+6);//分
							mod_can_msg.data[6]=AT24C16_ReadOneByte(tmpi*6+7);//秒
							mod_can_msg.data[7]=AT24C16_ReadOneByte(tmpi*6+8);//事件							
							can_send(&mod_can_msg);
							
						}//for (tmpi=0;tmpi<len;tmpi++)
						mod_can_msg.dlc     = 2;
						mod_can_msg.data[0]=0x66;
						mod_can_msg.data[1]=0x66;
						can_send(&mod_can_msg);
						
					}
				};break;
				#endif
				default:break;
				
				
				
			}
			if (send_status==true)
			{
				if (can_send_len>1)
				{
					//实现fifo功能
					int tmpi_can;
					for (tmpi_can=0;tmpi_can<can_send_len-1;tmpi_can++)
						mod_flag[tmpi_can]=mod_flag[tmpi_can+1];
					
				}
				can_send_len--;
			}
		}
		
		if (flag_20ms==1)
		{//每20ms触发一次
			flag_20ms=0;
			
			//如果有can播放数据在，每20ms播放一次
			#if NEW_BS
			if(can_sound_flag==true)//具有语音接口并且已经初始化了			
			{//从沿线接收到语音包，需要播放
				
				if (mute_flag==true)//判断功放是否打开
				{	
					//功放已经打开
					if (((can_sound_buf[0]==0x11)&&(can_sound_buf[1]==0x11)&&(is_YY_Init==true)))//播放打点
					{					
						//播放打点 
						//while (!AMBEGetWrStatus());//等待ambe1000可以被写操作
						AMBESetSine();        //本机打点发声	
						DEBUGOUT("\r\nAMBESetSine()finished!\r\n");		
						
						
					}//播放打点
					else if ((can_sound_buf[0]==0x22)&&(can_sound_buf[1]==0x22))//播放结束
					{
						//已经播放完成，可以关闭功放
						can_sound_buf_count=0;
						can_sound_flag=false;
						mute_flag=false;	
						can_sound_flag=false;
						
					
						Mute_ON();//关闭扩音电话功放					
						Led_OFF();//关闭扩音电话的显示灯
						
					}//播放结束 
					else if (((can_sound_buf[0]==0x33)&&(can_sound_buf[1]==0x33)&&(is_YY_Init==true)))//播放语音
					{
						char *p;						

						p=&can_sound_buf[2];//语音播放数据指针直接指向语音数据区
						//while (!AMBEGetWrStatus());
						AMBEWrData(p);//语音播放		
						
						
					}//播放语音			
					

					
				}//if mute_flag==true
				else
				{//mute_flag==false;
					//如果从总线上接收到第一帧语音数据时，此时应该先打开功放
					mute_flag=true;
					Mute_OFF();//打开功放
					Led_ON();//点亮语音灯
					
					flag_1000ms = 0;//将心跳计时器清0。由于播放语音时，总线上并不发送心跳，因此，将心跳计时器用于播放开始延时的计时。延时一个心跳后，开始采集电池电压
				}//mute_flag==false;
				 
			}//if(can_sound_flag==true) 如果有要播放的语音，先播放，半双工语音通信
			else if (is_YY_Init==true)
			{//没有要播放的语音时，采集对讲或者打点
				if (key1_flag==true)
				{//对讲，每20ms发一次数据
					char *p;
					uint16_t canid;
					uint8_t tmpi;
					
					key1_flag=true;
					/*======发送板：发送语音======*/
				
					//当判断到AMBEGetRdStatus()函数返回值
					//为非零时才可以进行编码读取的操作
					while(!AMBEGetRdStatus());   //判读是否可读，语音是否编码
				
					//语音编码被存放在数组FrameRdData中
					//将FrameRdData[10]~FrameRdData[15]
					//共计6个字节数据通过CAN总线发送出去
					//其中指针p指向FrameRdData[10]地址
					p=AMBERdData();
					canid = (CAN_MSGOBJ_STD|ID10_09_BSMConfirm|ID08_07_Soundcodes|CAN_ID); 
					msg.data[0]=msg.data[1]=0x33;
					for (tmpi=0;tmpi<6;tmpi++)
					{
						msg.data[tmpi+2]=*p;
						p++;
					}
					msg.mode_id=canid;
					msg.dlc=8;
					can_send(&msg);
					flag_5s=0;
					flag_5s_count=0;
				}//每20ms发一帧语音数据  if (key1_flag==true)
				if (key2_flag==true)
				{
					//打点，每20ms发一次数据，同时，自己也播放
					Sine_Open();//打点开启
					flag_1000ms = 0;
				}
				
			}
			#endif
			#if NEW_BS
			//检测18V掉线。
			if (flag_18ON==1)
			{
                //d18v_stat = D18V_STAT_OFF;
                JDQ_State_Open();//继电器吸合，断开下一级供电
                px_flag=0;
                D18V_LOST_FAST_count=50;//20ms进入一次，等18V电力恢复后，延时1000ms再重新排序
                
			}
			else
			{
                //d18v_stat = D18V_STAT_ON;
				#if DEBUG_UART_C
				flag_18off=0;
				#endif
				if(D18V_LOST_FAST_count>0)
					D18V_LOST_FAST_count--;
			}
			
			
			#endif
		}//20ms
		if(flag_1500ms == 1)   //flag_1500ms：1s定时标志，定时发送
		{
			//每秒发送一次心跳
			flag_1500ms = 0;
			if(can_rev_monitor_flag==true)
				can_rev_monitor_flag=false;
			else
			{
//				CAN_init();
				msg_obj.msgobj = 2;
				msg_obj.mask = 0x0;
				LPC_CCAN_API->config_rxmsgobj(&msg_obj);
			}

			#if NEW_BS	
            
            switch(d18v_stat)
			{
                case D18V_STAT_ON:
                {
					//沿线18V电压在，没有断电
					D18V_Lost_count=0;//将电池断电计数器清零
					
					if(D18V_IN_count<15)
					{ 
						D18V_IN_count++;
					}
					g_d18_onlinetime_cnt = D18V_IN_count;
                    
                    if (is_YY_Alive==true && is_YY_Init==false)
                    {
                        AMBEInit();		
                        Mute_ON();
                        is_YY_Init=true;
                    }
                    
                    if (is_YY_Alive==true)
                        msg.data[0]|=0x10;
                    else
                        msg.data[0]&=0xef;
        
                    if (px_flag==0)
                    {
                        JDQ_State_Open();							
                    }
                    else
                    {
                        JDQ_State_Close();//继电器吸合，给下一级供电							
                        msg.data[0]|=0x08;					
                        
                    }
                    if (heart_flag==true)
                    {
                        can_data_send(px_flag,&(msg.data[0]));
                    }
                    
                    if (flag_18ON==1)//18V 掉线
                    {
                        px_flag=0;
                        heart_flag=true;//2018.5.25 zhumx
                        D18V_IN_count = 0;
                        AMBEReset();
                        AMBESetGain();
                        d18v_stat =  D18V_STAT_OFF;                       
                    }
                    break;
                }
                
                case D18V_STAT_OFF:
                {
                    /*************************************沿线18V掉线处理 start *************************************************/ 
                    
                    if(g_d18_onlinetime_cnt>10)
                    {
                        g_battsupply_enable = true;
                        POWEROFF_State_Open();
                    }

                    if(g_battsupply_enable==false)
                    {
                        POWEROFF_State_Close();
                    }

                    D18V_Lost_count++;
                    if(D18V_Lost_count>D18V_LOST_TIMEOUT)//如果掉电时间超时，自动关机
                    {
                        //POWEROFF_State_Close();
                    }
                    
                    if (flag_18ON==0)//18V 上电
                    {
                        AMBEReset();
                        AMBESetGain();
                        d18v_stat =  D18V_STAT_ON;  
                    }
                    /*************************************沿线18V掉线处理end*************************************************/
                    break;
                }
                
                default:
                {
                    d18v_stat = D18V_STAT_ON;
                    break;
                }
            }
			#endif

		}//if(flag_1500ms == 1)   //1500ms定时到，定时发送

	}//while(1)
	
}//main()

/************************************************************************************************
** Function name:     Device_Init(void)
** Descriptions:      系统节拍初始化，消息队列初始化，软件定时器初始化，组件初始化
**										
** input parameters:  无				
** output parameters: 无 
** Returned value:    无
*************************************************************************************************/
void Device_Init(void)		
{
	SysTick_Config(((float)SystemCoreClock/1000000UL)*SYSTEM_TIMER_INT_PERIOD);	

//	InitMessageQueue();
//	InitSoftTimers();
	#ifdef 	ENABLE_CAN				//CAN组件初始化
	CAN_reset();	//在boot跳转到App的情况下，必须加此部分，否则CAN总线会出现工作不正常的现象
	CAN_init();
	msg_obj.msgobj = 0;
	msg_obj.mode_id = 0x00000000;
	msg_obj.mask = 0x0;
	LPC_CCAN_API->config_rxmsgobj(&msg_obj);
	#endif
	
// 	#ifdef 	ENABLE_KEY				//按键组件初始化
// 	Key_Init();
// 	#endif

// 	#ifdef 	ENABLE_LCD				//LCD组件初始化
// 	LCD_Init();
// 	#endif	
// 	
	#if NEW_BS
 	#ifdef 	ENABLE_ADC
 	ADCInit();
// 	ADC_Start();			        //启动AD转换
 	#endif	
	
	#endif
	#if NEW_BS
	#if ENABLE_AT24C16
 	#ifdef	ENABLE_EEPROM			//I2c总线初始化
 	i2c_app_init(I2C0, 100000);
 	#endif
	#endif
	#endif
 	
// 	#ifdef	ENABLE_LED				//I2c总线初始化
// 	led_Init();
// 	#endif


// 	
// 	#ifdef 	ENABLE_RS485			//按键组件初始化
// 	RS485_Init();
// 	#endif
	

}

/************************************************************************************************
** Function name:     SysTick_Handler(void)
** Descriptions:      系统节拍中断处理函数
**										
** input parameters:  无				
** output parameters: 无 
** Returned value:    无
*************************************************************************************************/

extern bool can_sound_flag;//收到语音标志
void SysTick_Handler(void)
{
 	volatile uint32_t RStau;
// 	static uint16_t s_counter_key = 0;
// 	static uint16_t s_counter_led = 0;
// 	static uint16_t s_counter_adc = 0;
// 	static uint8_t  channel_num = 1;
	
	RStau = SysTick->CTRL ;			//清计时器
//	DEBUGOUT("1+\r\n");
	DEBUGOUT("\r\n px_flag==%d, flag_18ON==%d, CAN_ID==%x, can_send_flag==%d, is_YY_Init==%d, is_YY_Alive==%d, ",px_flag,flag_18ON,CAN_ID,can_send_flag,is_YY_Init,is_YY_Alive);//打印px_flag，CAN_ID，flag_18ON
	DEBUGOUT("can_sound_flag==%d, mute_flag==%d",can_sound_flag,mute_flag);
	//DEBUGOUT("heart_flag==%d, key1_flag==%d, key2_flag==%d, can_sound_flag==%d, ",heart_flag,key1_flag,key2_flag,can_sound_flag);
	DEBUGOUT("D18V_IN_flag==%d, D18V_IN_count==%d, waitcnt==%d",D18V_IN_flag,D18V_IN_count,waitcnt);
    DEBUGOUT("\r\n 15:34");
    
    //	SoftTimerService();	
	if ((watchdog_feed_flag==true)&&(can_send_flag==false))
	{
		watchdog_feed_flag=false;
		#if ENABLE_WATCH_DOG
		watchdog_feedDog();
		#endif
	}
	if (reset_flag1==false)
	{
		reset_count++;
//		if (reset_count==5)
//			NVIC_SystemReset();
	}
	if(reset_flag==false)
	{
		reset_count++;
		if (is_YY_Alive==true)
		{
			//如果存在语音，那么会上电采集电压，需要更多的时间才能够实现排序。
//			if (reset_count==15)
//				NVIC_SystemReset();
		}
		else
		{
			//如果不存在语音，那么不需要采集电压，能够快一点实现排序
//			if (reset_count==10)
//				NVIC_SystemReset();
		}
		
	}
	if (can_sound_flag==true)
	{
		can_sound_time++;
		if (can_sound_time>=CAN_SOUND_TIMEOUT)
		{
			#if NEW_BS
			
				if (mute_flag==true)
				{
					
					Mute_ON();
					Led_OFF();
				}
			
			#endif
			mute_flag=false;
			can_sound_flag=false;
			can_sound_time=0;
			
		}
	}
/*起动ADC转换*/
//	(*((uint32_t *)0x4001C000)) |= (1<<24);//起动AD转换
}

