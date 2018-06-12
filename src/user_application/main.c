
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
#define D18V_LOST_TIMEOUT  28800  //��λ���룬18V��������߿��ܻ���Ҫ���������Խ��������绰����ʱ��8Сʱ
//#define DEBUG_ENABLE

#define ENABLE_WATCH_DOG 0
#define ENABLE_AT24C16 1
#define DEBUG_CAN 0
#define CAN_SOUND_TIMEOUT 10


////��������
void SysTick_Handler(void);								//ϵͳ�����жϴ�����
void Device_Init(void);										//�����ʼ��
void HardFault_Handler(void )
{
	NVIC_SystemReset();
}
//void Key_process(uint8_t Scanvalue);


/////ϵͳ��Ϣ�ṹ�����
MSG_TypeDef  msg;


#define QUITIME  5000	 //�����˳�ʱ�䣬��λms

#if DEBUG_UART_C
uint8_t flag_18off=0;//���ڴ��ڴ�ӡ����20ms���֣���һ�ν���18V�������ʱ����ӡ�����1��18V�ָ�������
#endif


uint16_t error=0;
extern bool can_sound_flag;//�յ�������־
bool mute_flag;//���Ŵ򿪱�־�� 
///////////////////////////////////////////////////////////
CCAN_MSG_OBJ_T msg_obj;	
///////////////////////////////////////////////////////////

int reset_count;
bool is_YY_Alive;// �����绰�Ƿ���ڱ�־�����ڲ������еı������������绰.
bool reset_flag1;
uint16_t ADC_value;
uint8_t flag_18ON=1;
bool D18V_IN_flag;//18V��Դ��⵽
uint8_t D18V_IN_count;//18V��⵽�����������ڼ�⵽18V  2s֮����������
//bool watchdog_reset_times;  δʹ��
bool key1_flag;//�Խ��������±�־
bool key2_flag;//��㰴�����±�־
bool is_YY_Init;//�����绰��ʼ����־���ϵ���ʱ���ʼ����ϣ�����ڽ��ambe1000�ϵ��ʼ�������õ����⡣
	
/*********************���Ӷ���ı��� - 2018.5.25 - zhumx ***************************/
uint8_t g_d18_onlinetime_cnt;//���ߵõ�ʱ���������1Hz����
bool g_battsupply_enable = false;//��ع���ʹ��
#define D18V_STAT_ON    1
#define D18V_STAT_OFF   2
uint8_t d18v_stat=D18V_STAT_ON;

/***********************************************************************************/
	
	
	
int main(void)
{	
	//char *p;
	

	uint8_t key_count;		 //��������ͳ�Ʊ���,���ڷ����ǲ��������������������һ��ʵ���ڵ�������������½��Ͳ��Ź���

	bool jt_flag; //��ͣ������־
	bool pp1_flag;//һ����ƫ��־
	bool pp2_flag;//������ƫ��־
	
	
	bool is_AT24C16_Alive;
	//at24c16��2048���ֽ�������
	//���У���ַ��0�����ڲ��ԣ�����0��==0x88ʱ����ʾ���ڡ�
	//��1������2�����ڴ�ż�¼��������[1]Ϊ��λ��ַ����2��Ϊ��λ��ַ
	//ÿ�μ�¼�¡��ա��֡�ʱ���롢�¼�  6���ֽ�
	//�¼�1 ��ʾ��������
	//�¼�2 ��ʾ�ͷű���
	//�¼�3 ��ʾ�������
	//�¼�4 ��ʾ�����Խ�
	//�¼�5 ��ʾһ����ƫ
	//�¼�6 ��ʾ������ƫ
	//�¼�7 ��ʾһ����ƫ����
	//�¼�8 ��ʾ������ƫ����
	uint16_t record_address;//�¼���¼��ַ�������¼��ĵ�ַ������at24c16��2��3���ֽ��С�����ַ==2000ʱ����3��ʼ��


	uint8_t jt_count; //��ͣ�������¼�����������������
	uint8_t pp1_count;//һ����ƫ����������������������
	uint8_t pp2_count;//������ƫ����������������������
	uint32_t D18V_Lost_count;//18V����ʱ�������
	uint8_t D18V_LOST_FAST_count;//���ڽ�����ݵ��粻�ܷ�����λ���ݵ����⡣

	extern unsigned int chip_ID;//оƬΨһ��
	uint32_t reset_id;//��λԭ��
	CCAN_MSG_OBJ_T msg;//������Χ�ڵ�can֡ȫ�ֱ���
	reset_flag=false;//����ϵ緢����ͨ���ϵ�ʱ��resetid.0==1,��ô��λ�����reset_count==15��1.5�룩ʱ����û��ȡ��CAN_ID����ô�ٸ�λһ�Ρ�
	reset_flag1=false;
	reset_count=0;
	D18V_LOST_FAST_count=0;
	//ϵͳ��ʼ��
	SystemCoreClockUpdate();
	//��ȡоƬΨһ�룬ֻ�������λ�ö�
	IAP_Entry = (void(*)())IAP_ENTER_ADR;
	chip_ID=ReadUID();
	reset_id=Chip_SYSCTL_GetSystemRSTStatus();//��ȡ��λԭ��
	Chip_SYSCTL_ClearSystemRSTStatus(reset_id);
	Board_Init();
	//���ڴ˺��������DEBUGOUT()�������������

//	DEBUGOUT("reset reason:0x%x\r\n",reset_id);//��ӡ��λԭ��
	Chip_GPIO_Init(LPC_GPIO);	
  Gpio_Init();//IO��ʼ״̬���̵�������ƫ����ͣ��
//	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 3, 2);//���ڲ��Զ�ʱ��
	
	#if NEW_BS
	//��ʼ��δ��ɣ�����ص�Ͽ�
//	POWEROFF_State_Close();
//	if((reset_id&0x03)!=0x0)
//	{
//		//�õ�Ƭ����1.8V��3.6V���������������ü�ط�Ժ����������Դ���磬�ϵ���С���ϵ��ٶ�Ҳ�Ƚ�������ˣ���Ҫ��һ�θ�λ��Ϊ�˱��ϵ�Դ������ģ���Ѿ�����������
//		uint32_t tmpi;
//		for (tmpi=0;tmpi<0xff;tmpi++);//��ʱ
////		NVIC_SystemReset();	//������ϵ縴λ����ô���¸�λһ��,Ϊ�˽��Ŀǰ���ڵ���������
//	}//�ڵ�·���ѽ�������⣬���ע������ά�磬20170725
	#endif
	Device_Init();

		
	#if ENABLE_WATCH_DOG
	watchdog_INIT(1);//��ʼ�����Ź�
	#endif

//	watchdog_reset_times=false;
	#if NEW_BS
	#if ENABLE_AT24C16
	
	if ((reset_id&0x04)==0x04)
	{
		//���Ź���λ
		uint8_t wd_reset_times;//���Ź�������λ����
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
		AT24C16_WriteOneByte(record_address,9);//9Ϊ���Ź���λ�¼�
		record_address++;
		if (record_address>=1980+3)//�ӵ�ַ3��ʼ��¼,�����г���¼�ǣ�ÿ����¼330*6=1980  330�����ݺ󣬾ʹ�ͷ�ٿ�ʼ
			record_address=3;
//		//�����ַ
		AT24C16_WriteOneByte(1,record_address%256);
		AT24C16_WriteOneByte(2,record_address/256);
		
		//�����Ź�������λ����
		//BYTE 2003 ���Ź���λ������������Ź�������λ����10�ζ��޷���ñ���������ţ���ô���ټ����������򡣽�ͨ��һ������һλ�������������������ôִ�����㡣������Ź���λ����ô+1.
		wd_reset_times=AT24C16_ReadOneByte(2003);
		if (wd_reset_times==10)
		{
//			watchdog_reset_times=true;
			px_flag=1;
			CAN_ID=0;
			JDQ_State_Close();//�̵������ϣ�����һ������
		}
		else
		{
			//��1д��ȥ
			AT24C16_WriteOneByte(2003,wd_reset_times+1);
		}
	}
	else  //if ((reset_id&0x04)==0x04)
	{
		//��Ϊ���Ź���λ����ô��0
			AT24C16_WriteOneByte(2003,0);
	}
	#endif
	#endif
	//Chip_GPIO_Init(LPC_GPIO);
	is_YY_Init=false;
	#if NEW_BS
	SBoard_Init();//����������ų�ʼ��
//	Mute_OFF();//�򿪹���
	#endif
	
	//������ʼ��
	key1_flag=false; 
	key2_flag=false;//��㰴�����±�־
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
//	while(flag_1000ms==0){}//��ʱ500ms
	
	#if NEW_BS
Mute_ON();    //���Źرգ�����			
//		if ((key2_flag==false)||(can_sound_flag==false))//key2_flag����㰴��
//	{
//			msg.data[1]=ADC_IRQ_Value_final%256;
//			msg.data[0]&=0x3f;//���msg.data[0]�������λ
//			msg.data[0]|=(((ADC_IRQ_Value_final-msg.data[1])>>2)&0xc0);
//			if ((ADC_IRQ_Value_final/1024)*3.3*4>2.0)
//			{
//				is_YY_Alive=true;		
//				Mute_ON();    //���Źرգ�����			
//			}
//			else
//				is_YY_Alive=false;
//	}//�˴����ж������绰�Ƿ���ڵı�־�ŵ���ѭ���У���ά�磬2017.7.30
	
	#if ENABLE_AT24C16
	if (AT24C16_Check()==0)
		is_AT24C16_Alive=true;
	#endif
	#endif
	
	
	TIM16B0_INT_init(5); //5ms
	flag_1000ms=0;
	flag_1000ms_count=0;
	#if ENABLE_WATCH_DOG
	watchdog_INIT(200);//��ʼ�����Ź�
	#endif 
	
	#if NEW_BS
	//��ʼ��δ��ɣ�����ص�Ͽ�
//	POWEROFF_State_Close();
	#endif
	
	reset_count=0;
//	DEBUGOUT("start!");
	/*��ʼ����ɺ�ɼ�һ�ε�ص�ѹ���Ժ��ڴ�����������ʱ�ɼ�*/
	msg.data[1]=ADC_IRQ_Value_final%256;
	msg.data[0]&=0x3f;//���msg.data[0]�������λ
	msg.data[0]|=(((ADC_IRQ_Value_final-msg.data[1])>>2)&0xc0);
	ADC_value=ADC_IRQ_Value_final;
	
	while (1) 
	{	
		watchdog_feed_flag=true;
		flag_18ON=D18V_State();	
	//Chip_WWDT_Feed(LPC_WWDT);
	//	while (1);
	/*�ж��Ƿ�������*/
	if ((key2_flag==true)||(can_sound_flag==true))//��㰴�»�����������ʱ�ɼ���ص�ѹ
	{
		msg.data[1]=ADC_IRQ_Value_final%256;
		msg.data[0]&=0x3f;//���msg.data[0]�������λ
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

		//����ͣ����
	if (flag_10ms==1)
	{
		flag_10ms=0;
		if (Chip_GPIO_GetPinState(LPC_GPIO, JT_PORT, JT_PORT_BIT)==1)
		{			 
			jt_flag=true;				
			if(jt_count<15)jt_count++;//ÿ10ms��һ�α���״̬			
		}
		else if ((jt_flag==true)&&(Chip_GPIO_GetPinState(LPC_GPIO, JT_PORT, JT_PORT_BIT)==0))
		{
			 
			if (jt_count>0)
				jt_count--;//���ֶ��������Ѿ��ɿ�����
			if (jt_count==0)
			{
				if ((msg.data[0]&0x01)==0x01)
				{//��������
					#if NEW_BS
					if (is_AT24C16_Alive==true)
					{
						//at24c16����
						//at24c16��2048���ֽ�������
						//���У���ַ��0�����ڲ��ԣ�����0��==0x88ʱ����ʾ���ڡ�
						//��1������2�����ڴ�ż�¼��������[1]Ϊ��λ��ַ����2��Ϊ��λ��ַ
						//ÿ�μ�¼�¡��ա��֡�ʱ���롢�¼�  6���ֽ�
						//�¼�1 ��ʾ��������
						//�¼�2 ��ʾ�ͷű���
						//�¼�3 ��ʾ�������
						//�¼�4 ��ʾ�����Խ�
						//�¼�5 ��ʾһ����ƫ
						//�¼�6 ��ʾ������ƫ
						//�¼�7 ��ʾһ����ƫ����
						//�¼�8 ��ʾ������ƫ����
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
						AT24C16_WriteOneByte(record_address,2);//2Ϊ�����ͷ��¼�
						record_address++;
						if (record_address>=1980+3)//�ӵ�ַ3��ʼ��¼,�����г���¼�ǣ�ÿ����¼330*6=1980  330�����ݺ󣬾ʹ�ͷ�ٿ�ʼ
							record_address=3;
						//�����ַ
						AT24C16_WriteOneByte(1,record_address%256);
						AT24C16_WriteOneByte(2,record_address/256);
					}//if (is_AT24C16_Alive==true) at24c16����
					#endif
					msg.mode_id=ID10_09_ChangeSend|ID08_07_Equipments|CAN_ID;//��Ҫ����canid
					msg.data[0]&=0xfe;
					msg.dlc=2;
					can_send(&msg);//�仯���ͣ���������
					flag_1500ms_count = 0;
					flag_1500ms = 0;//����ʱ���ͱ�־����
				}//if ((msg.data[0]&0x01)==0x01)
				
				jt_flag=false;//�����ͣ��־λ
				
			}
		}
		if (((jt_count>14)||(jt_count==0))&&(jt_flag==true))
		{
			msg.mode_id=ID10_09_ChangeSend|ID08_07_Equipments|CAN_ID;//��Ҫ����canid
			
			if (jt_count>0)
			{
				//���ͱ����ź�
				//jt_count=0;
				if ((msg.data[0]&0x01)==0x00)//ʵ�ֲ��䲻��������ڴ�֮ǰ�Ѿ����ڱ���״̬����ômsg.data[0].0λӦ���Ѿ���1��
				{//ʵ�ֱ仯����
					msg.data[0]|=0x01;//��data[0].0��λΪ1����ʾ�б����������������ݻ��߼�ͣ״̬���͡�
					msg.dlc=2;
					can_send(&msg);//��ͣ״̬����
					flag_1500ms_count = 0;
					flag_1500ms = 0;//����ʱ���ͱ�־����
					#if NEW_BS
					//����i2c��¼
					if (is_AT24C16_Alive==true)
					{
						//at24c16����
						//at24c16��2048���ֽ�������
						//���У���ַ��0�����ڲ��ԣ�����0��==0x88ʱ����ʾ���ڡ�
						//��1������2�����ڴ�ż�¼��������[1]Ϊ��λ��ַ����2��Ϊ��λ��ַ
						//ÿ�μ�¼�¡��ա��֡�ʱ���롢�¼�  6���ֽ�
						//�¼�1 ��ʾ��������
						//�¼�2 ��ʾ�ͷű���
						//�¼�3 ��ʾ�������
						//�¼�4 ��ʾ�����Խ�
						//�¼�5 ��ʾһ����ƫ
						//�¼�6 ��ʾ������ƫ
						//�¼�7 ��ʾһ����ƫ����
						//�¼�8 ��ʾ������ƫ����
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
						AT24C16_WriteOneByte(record_address,1);//1Ϊ�����¼�
						record_address++;
						if (record_address>=1980+3)//�ӵ�ַ3��ʼ��¼,�����г���¼�ǣ�ÿ����¼330*6=1980  330�����ݺ󣬾ʹ�ͷ�ٿ�ʼ
							record_address=3;
						//�����ַ
						AT24C16_WriteOneByte(1,record_address%256);
						AT24C16_WriteOneByte(2,record_address/256);
					}//if (is_AT24C16_Alive==true) ��������
					#endif
				}//if ((msg.data[0]&0x01)==0x00)//ʵ�ֲ��䲻��������ڴ�֮ǰ�Ѿ����ڱ���״̬����ômsg.data[0].0λӦ���Ѿ���1��
			}//
		}// ����ͣ����
	}	
				
	//����һ����ƫ
	if(flag_10ms_p1==1)
	{	
		flag_10ms_p1=0;
		if (Chip_GPIO_GetPinState(LPC_GPIO, PP1_PORT, PP1_PORT_BIT)==false)
		{
			pp1_flag=true;
			if(pp1_count<15)pp1_count++;//ÿ10ms��һ��һ����ƫ״̬				 
		}
		else if ((pp1_flag==true)&&(Chip_GPIO_GetPinState(LPC_GPIO, PP1_PORT, PP1_PORT_BIT)==true))
		{
			if (pp1_count>0)
				pp1_count--;//���ֶ��������Ѿ��ɿ�һ����ƫ
			if (pp1_count==0)
			{
				if ((msg.data[0]&0x02)==0x02)
				{
					#if NEW_BS
					if (is_AT24C16_Alive==true)
					{
						//at24c16����
						//at24c16��2048���ֽ�������
						//���У���ַ��0�����ڲ��ԣ�����0��==0x88ʱ����ʾ���ڡ�
						//��1������2�����ڴ�ż�¼��������[1]Ϊ��λ��ַ����2��Ϊ��λ��ַ
						//ÿ�μ�¼�¡��ա��֡�ʱ���롢�¼�  6���ֽ�
						//�¼�1 ��ʾ��������
						//�¼�2 ��ʾ�ͷű���
						//�¼�3 ��ʾ�������
						//�¼�4 ��ʾ�����Խ�
						//�¼�5 ��ʾһ����ƫ
						//�¼�6 ��ʾ������ƫ
						//�¼�7 ��ʾһ����ƫ����
						//�¼�8 ��ʾ������ƫ����
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
						AT24C16_WriteOneByte(record_address,7);//7Ϊһ����ƫ�ͷ��¼�
						record_address++;
						if (record_address>=1980+3)//�ӵ�ַ3��ʼ��¼,�����г���¼�ǣ�ÿ����¼330*6=1980  330�����ݺ󣬾ʹ�ͷ�ٿ�ʼ
							record_address=3;
						//�����ַ
						AT24C16_WriteOneByte(1,record_address%256);
						AT24C16_WriteOneByte(2,record_address/256);
					}//if (is_AT24C16_Alive==true) ��������
					#endif
					msg.mode_id=ID10_09_ChangeSend|ID08_07_Equipments|CAN_ID;//��Ҫ����canid
					msg.data[0]&=0xfd;
					msg.dlc=2;
					can_send(&msg);//�仯���ͣ�һ����ƫ����
					flag_1500ms_count = 0;
					flag_1500ms = 0;//����ʱ���ͱ�־����
				}//if msg.data[0]&0x02)==0x02
				
				pp1_flag=false;//���������ƫ��־λ
				//����i2c��¼
				 
			}
		}
		if (((pp1_count>14)||(pp1_count==0))&&(pp1_flag==true))
		{
			msg.mode_id=ID10_09_ChangeSend|ID08_07_Equipments|CAN_ID;//��Ҫ����canid
			if (pp1_count>0)
			{
				//����һ����ƫ�ź�
//				pp1_count=0;
				if ((msg.data[0]&0x02)==0x00)//ʵ�ֲ��䲻��������ڴ�֮ǰ�Ѿ����ڱ���״̬����ômsg.data[0].1λӦ���Ѿ���1��
				{//ʵ�ֱ仯����
					msg.data[0]|=0x02;//��data[0].1��λΪ1����ʾ��һ����ƫ�������������ݻ��߼�ͣ״̬���͡�
					msg.dlc=2;
					can_send(&msg);//һ����ƫ״̬����
					flag_1500ms_count = 0;
					flag_1500ms = 0;//����ʱ���ͱ�־����
					//����i2c��¼
					#if NEW_BS
					if (is_AT24C16_Alive==true)
					{
						//at24c16����
						//at24c16��2048���ֽ�������
						//���У���ַ��0�����ڲ��ԣ�����0��==0x88ʱ����ʾ���ڡ�
						//��1������2�����ڴ�ż�¼��������[1]Ϊ��λ��ַ����2��Ϊ��λ��ַ
						//ÿ�μ�¼�¡��ա��֡�ʱ���롢�¼�  6���ֽ�
						//�¼�1 ��ʾ��������
						//�¼�2 ��ʾ�ͷű���
						//�¼�3 ��ʾ�������
						//�¼�4 ��ʾ�����Խ�
						//�¼�5 ��ʾһ����ƫ
						//�¼�6 ��ʾ������ƫ
						//�¼�7 ��ʾһ����ƫ����
						//�¼�8 ��ʾ������ƫ����
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
						AT24C16_WriteOneByte(record_address,5);//5Ϊһ����ƫ�¼�
						record_address++;
						if (record_address>=1980+3)//�ӵ�ַ3��ʼ��¼,�����г���¼�ǣ�ÿ����¼330*6=1980  330�����ݺ󣬾ʹ�ͷ�ٿ�ʼ
							record_address=3;
						//�����ַ
						AT24C16_WriteOneByte(1,record_address%256);
						AT24C16_WriteOneByte(2,record_address/256);
					}//if (is_AT24C16_Alive==true) ��������
					#endif
					
				}
				 
			}
		}
	}//����һ����ƫ
		
		//���������ƫ	
		if(flag_10ms_p2==1)
		{
			flag_10ms_p2=0;
		if (Chip_GPIO_GetPinState(LPC_GPIO, PP2_PORT, PP2_PORT_BIT)==0)
		{
			pp2_flag=true;
			if (pp2_count<15)
				pp2_count++;//ÿ10ms��һ�ζ�����ƫ״̬
				 
			
		}
		else if ((pp2_flag==true)&&(Chip_GPIO_GetPinState(LPC_GPIO, PP2_PORT, PP2_PORT_BIT)==1))
		{
			if (pp2_count>0)
				pp2_count--;//���ֶ��������Ѿ��ɿ�������ƫ
			if (pp2_count==0)
			{
				if ((msg.data[0]&0x04)==0x04)
				{
					#if NEW_BS
					if (is_AT24C16_Alive==true)
					{
						//at24c16����
						//at24c16��2048���ֽ�������
						//���У���ַ��0�����ڲ��ԣ�����0��==0x88ʱ����ʾ���ڡ�
						//��1������2�����ڴ�ż�¼��������[1]Ϊ��λ��ַ����2��Ϊ��λ��ַ
						//ÿ�μ�¼�¡��ա��֡�ʱ���롢�¼�  6���ֽ�
						//�¼�1 ��ʾ��������
						//�¼�2 ��ʾ�ͷű���
						//�¼�3 ��ʾ�������
						//�¼�4 ��ʾ�����Խ�
						//�¼�5 ��ʾһ����ƫ
						//�¼�6 ��ʾ������ƫ
						//�¼�7 ��ʾһ����ƫ����
						//�¼�8 ��ʾ������ƫ����
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
						AT24C16_WriteOneByte(record_address,8);//8Ϊ������ƫ�ͷ��¼�
						record_address++;
						if (record_address>=1980+3)//�ӵ�ַ3��ʼ��¼,�����г���¼�ǣ�ÿ����¼330*6=1980  330�����ݺ󣬾ʹ�ͷ�ٿ�ʼ
							record_address=3;
						//�����ַ
						AT24C16_WriteOneByte(1,record_address%256);
						AT24C16_WriteOneByte(2,record_address/256);
					}//if (is_AT24C16_Alive==true) ��������
					#endif
					msg.mode_id=ID10_09_ChangeSend|ID08_07_Equipments|CAN_ID;//��Ҫ����canid	
					msg.data[0]&=0xfb;
					msg.dlc=2;
					can_send(&msg);//�仯���ͣ�������ƫ����
					flag_1500ms_count = 0;
					flag_1500ms = 0;//����ʱ���ͱ�־����
				}//if ((msg.data[0]&0xfb)==0x04)
				
				pp2_flag=false;//���������ƫ��־λ
				
				}
				 
		}
		if (((pp2_count>14)||(pp2_count==0))&&(pp2_flag==true))
		{
			
			msg.mode_id=ID10_09_ChangeSend|ID08_07_Equipments|CAN_ID;//��Ҫ����canid			
			if (pp2_count>0)
			{
				//���Ͷ�����ƫ�ź�
//				pp2_count=0;
				if ((msg.data[0]&0x04)==0x00)//ʵ�ֲ��䲻��������ڴ�֮ǰ�Ѿ����ڶ�����ƫ״̬����ômsg.data[0].2λӦ���Ѿ���1��
				{//ʵ�ֱ仯����
					msg.data[0]|=0x04;//��data[0].2��λΪ1����ʾ�ж�����ƫ�������������ݻ��߼�ͣ״̬���͡�
					msg.dlc=2;
					can_send(&msg);//������ƫ״̬����
					flag_1500ms_count = 0;
					flag_1500ms = 0;//����ʱ���ͱ�־����
					//����i2c��¼
					#if NEW_BS
					if (is_AT24C16_Alive==true)
					{
						//at24c16����
						//at24c16��2048���ֽ�������
						//���У���ַ��0�����ڲ��ԣ�����0��==0x88ʱ����ʾ���ڡ�
						//��1������2�����ڴ�ż�¼��������[1]Ϊ��λ��ַ����2��Ϊ��λ��ַ
						//ÿ�μ�¼�¡��ա��֡�ʱ���롢�¼�  6���ֽ�
						//�¼�1 ��ʾ��������
						//�¼�2 ��ʾ�ͷű���
						//�¼�3 ��ʾ�������
						//�¼�4 ��ʾ�����Խ�
						//�¼�5 ��ʾһ����ƫ
						//�¼�6 ��ʾ������ƫ
						//�¼�7 ��ʾһ����ƫ����
						//�¼�8 ��ʾ������ƫ����
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
						AT24C16_WriteOneByte(record_address,6);//6Ϊ������ƫ�¼�
						record_address++;
						if (record_address>=1980+3)//�ӵ�ַ3��ʼ��¼,�����г���¼�ǣ�ÿ����¼330*6=1980  330�����ݺ󣬾ʹ�ͷ�ٿ�ʼ
							record_address=3;
						//�����ַ
						AT24C16_WriteOneByte(1,record_address%256);
						AT24C16_WriteOneByte(2,record_address/256);
					}//if (is_AT24C16_Alive==true) ��������
					#endif
				}//if ((msg.data[0]&0x04)==0x00)//ʵ�ֲ��䲻��������ڴ�֮ǰ�Ѿ����ڶ�����ƫ״̬����ômsg.data[0].2λӦ���Ѿ���1��
			}
			 
		}
	}////���������ƫ
		#if NEW_BS
		if (is_YY_Init==true)
		{

			if(Get_Key1() == 1  )     //�Խ��¼���ʼ
			{
				if ((can_sound_flag==false)&&(key2_flag==false))//ֻ���ڲ������Լ�û�д���ʱ��ſ��Խ�����//�л������ȫ˫������
					key1_flag=true;			
				 
			}
			else if (key1_flag==true)
			{
				key1_flag=false;
				//�������������¶�ȡ��ص�ѹ����ͣ����ƫ����
				msg.data[0]=0;
				if(jt_flag==false)msg.data[0]&=0xfe;
				else if(jt_flag==true)msg.data[0]|=0x01;
				if(pp1_flag==true)msg.data[0]|=0x02;
				else if(pp1_flag==false)msg.data[0]&=0xfd;
				if(pp2_flag==true)msg.data[0]|=0x04;
				else if(pp2_flag==false)msg.data[0]&=0xfb;
				msg.data[1]=ADC_value%256;
				msg.data[0]&=0x3f;//���msg.data[0]�������λ
				msg.data[0]|=(((ADC_value-msg.data[1])>>2)&0xc0);
				
//				Sine_Close();//���ر�
				 
			}
			if((Get_Key2() == 1))       //����¼���ʼ
			{
				if ((can_sound_flag==false)&&(key1_flag==false))//ֻ���ڲ�������ʱ���Լ�û�н�����ʱ��ſ��Դ��
				{
					key2_flag=true;
//				Sine_Open();//��㿪��
				}
				
			}
			else if ((key2_flag==true))
			{
				key2_flag=false;
				Sine_Close();//���ر�
				 
			}
		}
		#endif
		if (can_send_len>0)
		{
			bool send_status;


			switch (mod_flag[0])
			{
				//Ϊ�˲�Ӱ�쵽����������У����ﵥ������һ���ֲ��������ڷ���can����
				CCAN_MSG_OBJ_T mod_can_msg;
				//mod_flag[x]=1��Ӧ�𱾻�ID DLC=0��
				case 0x01:{
					mod_can_msg.mode_id = (CAN_MSGOBJ_STD|ID10_09_BSMConfirm|ID08_07_Equipments|CAN_ID); 
					mod_can_msg.dlc     = 0;
					mod_can_msg.msgobj=0;
					send_status=can_send(&mod_can_msg);
				};break;
				//mod_flag[x]=2����ȡchipID��¼ DLC=4��
				case 0x02:{
					mod_can_msg.mode_id = (CAN_MSGOBJ_STD|ID10_09_BSMConfirm|ID08_07_Equipments|CAN_ID); 
					mod_can_msg.dlc     = 4;
					mod_can_msg.data[0] = chip_ID%256;
					mod_can_msg.data[1] = (chip_ID-mod_can_msg.data[0])>>8%256;
					mod_can_msg.data[2] = (chip_ID-(unsigned int)mod_can_msg.data[1]*256-mod_can_msg.data[0])>>16%256;
					mod_can_msg.data[3] = (chip_ID-(unsigned int)mod_can_msg.data[2]*65536-(unsigned int)mod_can_msg.data[1]*256-mod_can_msg.data[0])>>24%256;;
					send_status=can_send(&mod_can_msg);
				};break;
				//mod_flag[x]=3�����flash���� DLC=0��
				#if NEW_BS
				case 0x03:{
					if (AT24C16_Check()==0)
					{
						AT24C16_WriteOneByte(1,3);
						AT24C16_WriteOneByte(2,0);
					}
				};break;
				//mod_flag[x]=4����ȡ��ʷ��¼���� DLC=0��
				case 0x04:{
					//					CAN_RD_Message.data[0] = 0x00;
					if (AT24C16_Check()==0)
					{
						int len,tmpi;
						len=AT24C16_ReadOneByte(1)+((uint16_t)AT24C16_ReadOneByte(2)<<8);
						len=(len-3)/6;
						//�ȷ������ݰ���ʼ������
						mod_can_msg.mode_id = (CAN_MSGOBJ_STD|ID10_09_BSMConfirm|ID08_07_Equipments|CAN_ID); 
						mod_can_msg.dlc     = 4;
						mod_can_msg.data[0]=0x44;
						mod_can_msg.data[1]=0x44;
						mod_can_msg.data[2]=len%256;//�������ݰ������ĵ�λ
						mod_can_msg.data[3]=len/256;//�������ݰ������ĸ�λ
						can_send(&mod_can_msg);
						for (tmpi=0;tmpi<len;tmpi++)
						{
							
							mod_can_msg.dlc     = 8;
							mod_can_msg.data[0]=0x55;
							mod_can_msg.data[1]=0x55;
							mod_can_msg.data[2]=AT24C16_ReadOneByte(tmpi*6+3);//��
							mod_can_msg.data[3]=AT24C16_ReadOneByte(tmpi*6+4);//��
							mod_can_msg.data[4]=AT24C16_ReadOneByte(tmpi*6+5);//ʱ
							mod_can_msg.data[5]=AT24C16_ReadOneByte(tmpi*6+6);//��
							mod_can_msg.data[6]=AT24C16_ReadOneByte(tmpi*6+7);//��
							mod_can_msg.data[7]=AT24C16_ReadOneByte(tmpi*6+8);//�¼�							
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
					//ʵ��fifo����
					int tmpi_can;
					for (tmpi_can=0;tmpi_can<can_send_len-1;tmpi_can++)
						mod_flag[tmpi_can]=mod_flag[tmpi_can+1];
					
				}
				can_send_len--;
			}
		}
		
		if (flag_20ms==1)
		{//ÿ20ms����һ��
			flag_20ms=0;
			
			//�����can���������ڣ�ÿ20ms����һ��
			#if NEW_BS
			if(can_sound_flag==true)//���������ӿڲ����Ѿ���ʼ����			
			{//�����߽��յ�����������Ҫ����
				
				if (mute_flag==true)//�жϹ����Ƿ��
				{	
					//�����Ѿ���
					if (((can_sound_buf[0]==0x11)&&(can_sound_buf[1]==0x11)&&(is_YY_Init==true)))//���Ŵ��
					{					
						//���Ŵ�� 
						//while (!AMBEGetWrStatus());//�ȴ�ambe1000���Ա�д����
						AMBESetSine();        //������㷢��	
						DEBUGOUT("\r\nAMBESetSine()finished!\r\n");		
						
						
					}//���Ŵ��
					else if ((can_sound_buf[0]==0x22)&&(can_sound_buf[1]==0x22))//���Ž���
					{
						//�Ѿ�������ɣ����Թرչ���
						can_sound_buf_count=0;
						can_sound_flag=false;
						mute_flag=false;	
						can_sound_flag=false;
						
					
						Mute_ON();//�ر������绰����					
						Led_OFF();//�ر������绰����ʾ��
						
					}//���Ž��� 
					else if (((can_sound_buf[0]==0x33)&&(can_sound_buf[1]==0x33)&&(is_YY_Init==true)))//��������
					{
						char *p;						

						p=&can_sound_buf[2];//������������ָ��ֱ��ָ������������
						//while (!AMBEGetWrStatus());
						AMBEWrData(p);//��������		
						
						
					}//��������			
					

					
				}//if mute_flag==true
				else
				{//mute_flag==false;
					//����������Ͻ��յ���һ֡��������ʱ����ʱӦ���ȴ򿪹���
					mute_flag=true;
					Mute_OFF();//�򿪹���
					Led_ON();//����������
					
					flag_1000ms = 0;//��������ʱ����0�����ڲ�������ʱ�������ϲ���������������ˣ���������ʱ�����ڲ��ſ�ʼ��ʱ�ļ�ʱ����ʱһ�������󣬿�ʼ�ɼ���ص�ѹ
				}//mute_flag==false;
				 
			}//if(can_sound_flag==true) �����Ҫ���ŵ��������Ȳ��ţ���˫������ͨ��
			else if (is_YY_Init==true)
			{//û��Ҫ���ŵ�����ʱ���ɼ��Խ����ߴ��
				if (key1_flag==true)
				{//�Խ���ÿ20ms��һ������
					char *p;
					uint16_t canid;
					uint8_t tmpi;
					
					key1_flag=true;
					/*======���Ͱ壺��������======*/
				
					//���жϵ�AMBEGetRdStatus()��������ֵ
					//Ϊ����ʱ�ſ��Խ��б����ȡ�Ĳ���
					while(!AMBEGetRdStatus());   //�ж��Ƿ�ɶ��������Ƿ����
				
					//�������뱻���������FrameRdData��
					//��FrameRdData[10]~FrameRdData[15]
					//����6���ֽ�����ͨ��CAN���߷��ͳ�ȥ
					//����ָ��pָ��FrameRdData[10]��ַ
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
				}//ÿ20ms��һ֡��������  if (key1_flag==true)
				if (key2_flag==true)
				{
					//��㣬ÿ20ms��һ�����ݣ�ͬʱ���Լ�Ҳ����
					Sine_Open();//��㿪��
					flag_1000ms = 0;
				}
				
			}
			#endif
			#if NEW_BS
			//���18V���ߡ�
			if (flag_18ON==1)
			{
                //d18v_stat = D18V_STAT_OFF;
                JDQ_State_Open();//�̵������ϣ��Ͽ���һ������
                px_flag=0;
                D18V_LOST_FAST_count=50;//20ms����һ�Σ���18V�����ָ�����ʱ1000ms����������
                
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
		if(flag_1500ms == 1)   //flag_1500ms��1s��ʱ��־����ʱ����
		{
			//ÿ�뷢��һ������
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
					//����18V��ѹ�ڣ�û�жϵ�
					D18V_Lost_count=0;//����ضϵ����������
					
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
                        JDQ_State_Close();//�̵������ϣ�����һ������							
                        msg.data[0]|=0x08;					
                        
                    }
                    if (heart_flag==true)
                    {
                        can_data_send(px_flag,&(msg.data[0]));
                    }
                    
                    if (flag_18ON==1)//18V ����
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
                    /*************************************����18V���ߴ��� start *************************************************/ 
                    
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
                    if(D18V_Lost_count>D18V_LOST_TIMEOUT)//�������ʱ�䳬ʱ���Զ��ػ�
                    {
                        //POWEROFF_State_Close();
                    }
                    
                    if (flag_18ON==0)//18V �ϵ�
                    {
                        AMBEReset();
                        AMBESetGain();
                        d18v_stat =  D18V_STAT_ON;  
                    }
                    /*************************************����18V���ߴ���end*************************************************/
                    break;
                }
                
                default:
                {
                    d18v_stat = D18V_STAT_ON;
                    break;
                }
            }
			#endif

		}//if(flag_1500ms == 1)   //1500ms��ʱ������ʱ����

	}//while(1)
	
}//main()

/************************************************************************************************
** Function name:     Device_Init(void)
** Descriptions:      ϵͳ���ĳ�ʼ������Ϣ���г�ʼ���������ʱ����ʼ���������ʼ��
**										
** input parameters:  ��				
** output parameters: �� 
** Returned value:    ��
*************************************************************************************************/
void Device_Init(void)		
{
	SysTick_Config(((float)SystemCoreClock/1000000UL)*SYSTEM_TIMER_INT_PERIOD);	

//	InitMessageQueue();
//	InitSoftTimers();
	#ifdef 	ENABLE_CAN				//CAN�����ʼ��
	CAN_reset();	//��boot��ת��App������£�����Ӵ˲��֣�����CAN���߻���ֹ���������������
	CAN_init();
	msg_obj.msgobj = 0;
	msg_obj.mode_id = 0x00000000;
	msg_obj.mask = 0x0;
	LPC_CCAN_API->config_rxmsgobj(&msg_obj);
	#endif
	
// 	#ifdef 	ENABLE_KEY				//���������ʼ��
// 	Key_Init();
// 	#endif

// 	#ifdef 	ENABLE_LCD				//LCD�����ʼ��
// 	LCD_Init();
// 	#endif	
// 	
	#if NEW_BS
 	#ifdef 	ENABLE_ADC
 	ADCInit();
// 	ADC_Start();			        //����ADת��
 	#endif	
	
	#endif
	#if NEW_BS
	#if ENABLE_AT24C16
 	#ifdef	ENABLE_EEPROM			//I2c���߳�ʼ��
 	i2c_app_init(I2C0, 100000);
 	#endif
	#endif
	#endif
 	
// 	#ifdef	ENABLE_LED				//I2c���߳�ʼ��
// 	led_Init();
// 	#endif


// 	
// 	#ifdef 	ENABLE_RS485			//���������ʼ��
// 	RS485_Init();
// 	#endif
	

}

/************************************************************************************************
** Function name:     SysTick_Handler(void)
** Descriptions:      ϵͳ�����жϴ�����
**										
** input parameters:  ��				
** output parameters: �� 
** Returned value:    ��
*************************************************************************************************/

extern bool can_sound_flag;//�յ�������־
void SysTick_Handler(void)
{
 	volatile uint32_t RStau;
// 	static uint16_t s_counter_key = 0;
// 	static uint16_t s_counter_led = 0;
// 	static uint16_t s_counter_adc = 0;
// 	static uint8_t  channel_num = 1;
	
	RStau = SysTick->CTRL ;			//���ʱ��
//	DEBUGOUT("1+\r\n");
	DEBUGOUT("\r\n px_flag==%d, flag_18ON==%d, CAN_ID==%x, can_send_flag==%d, is_YY_Init==%d, is_YY_Alive==%d, ",px_flag,flag_18ON,CAN_ID,can_send_flag,is_YY_Init,is_YY_Alive);//��ӡpx_flag��CAN_ID��flag_18ON
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
			//���������������ô���ϵ�ɼ���ѹ����Ҫ�����ʱ����ܹ�ʵ������
//			if (reset_count==15)
//				NVIC_SystemReset();
		}
		else
		{
			//�����������������ô����Ҫ�ɼ���ѹ���ܹ���һ��ʵ������
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
/*��ADCת��*/
//	(*((uint32_t *)0x4001C000)) |= (1<<24);//��ADת��
}

