
///////////////////////////////////////////////////////////////////////////
/// @file  key.c
/// @brief �������Դ����
/// 
/// �������Դ�������������ʼ��������ɨ��ȹ���
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////
/// includes
/////////////////////////////////////////////////
#include "chip.h"
#include "key.h"
#include "Message.h"
#include "rs485.h"

/////////////////////////////////////////////////
/// ��������
/////////////////////////////////////////////////
static uint8_t ScanValueSave = Key_Ini_Sta;    //��һ�ΰ���Ĭ�ϴ���ֵ
static uint8_t ScanCounter = 0;            //�������½��ļ�����
static uint8_t DoubleHitCounter = 0;       //��ֹ����˫���󶯽��ļ�����

/////////////////////////////////////////////////////////////////////
/// ��������
////////////////////////////////////////////////////////////////////
static uint8_t RemapKey( uint8_t );           //�̰���ӳ��
static uint8_t ScanPin( void );               //ɨ���ֵ

#ifdef Identify_Long_Click
static uint8_t RemapLongKey( uint8_t );       //������ӳ��
#endif

/************************************************************************************************
** Function name:     Key_Scan(void)
** Descriptions:      ɨ�谴�����������º������Ϣ���ݰ�����Ϣ
**										֧�ֳ��̰���Ĭ��ֻ֧�ֶ̰���������ͷ�ļ�������
** input parameters:  ��				
** output parameters: �� 
** Returned value:    ��
*************************************************************************************************/
 void Key_Scan(void)
{
     uint8_t scanValue;

     scanValue = ScanPin();                     //��ȡ����ֵ

    if (scanValue == Key_Ini_Sta)              //�ж��Ƿ��ͷŰ���
    {
        if (DoubleHitCounter)                  //��ֹ˫��������,�ͷź�������òſ��Լ�����ⰴ��
        {
            DoubleHitCounter--;
            ScanCounter = 0;
            ScanValueSave = Key_Ini_Sta;
            return ;
        }

        if (ScanValueSave == Key_Ini_Sta)           //�жϰ���ֵ�Ƿ���Ч����Ч
        {
            ScanCounter = 0;
            return ;
        }
#ifdef Identify_Long_Click
				
        if (ScanCounter == LongInterval)        //�ж�Ϊ�����������ͳ�������Ϣ
        {           
			PostMessage( MSG_TYPE_KEY, MSG_PRIORITY_LOW, RemapLongKey(ScanValueSave) );      
        }
        else if (ScanCounter > ShortInterval)   //�ж�Ϊ�̰��������Ͷ̰�����Ϣ
        {
            PostMessage( MSG_TYPE_KEY, MSG_PRIORITY_LOW, RemapKey(ScanValueSave) );
        }
				
#else
				
	      if (ScanCounter > ShortInterval)        //�ж�Ϊ�̰��������Ͷ̰�����Ϣ
        {
            PostMessage( MSG_TYPE_KEY, MSG_PRIORITY_LOW, RemapKey(ScanValueSave) );			
        }		
				
#endif
				
        ScanCounter = 0;
        ScanValueSave = Key_Ini_Sta;
        DoubleHitCounter = DoubleHitInterval;   //�趨��ֹ˫��������
    }
    else                                        //��⵽����ֵ
    {
        ScanCounter++;

        if (ScanCounter == 1)                   //�洢��һ�ΰ���ֵ
        {
          ScanValueSave = scanValue;
        }
        else if (ScanCounter == 2)              //�жϰ���ֵ�Ƿ���Ч
        {
		if (scanValue != ScanValueSave)
            {
                ScanValueSave = Key_Ini_Sta;
                ScanCounter = 0;
            }
        }
				else if (ScanCounter == 4)              //��ϰ���ʱ2��������ͬʱ���£������ļ�ֵΪ׼
        {
        if (scanValue != ScanValueSave)
            {
                ScanValueSave = scanValue;
            }
        }
        else if (ScanCounter >= LongInterval)   //��ֹ����ֵ���ѭ��
        {
            ScanCounter = LongInterval;
        }
    }
}


/************************************************************************************************
** Function name:     Key_Init
** Descriptions:      ������ʼ������  
**										
** input parameters:  ��				
** output parameters: �� 
** Returned value:    ��
*************************************************************************************************/
void Key_Init( void )
{	
		 Chip_GPIO_Init(LPC_GPIO);
#ifdef	KEY_A_PIN
		 Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_2, (IOCON_FUNC1 | IOCON_MODE_INACT | MD_DIGMODE));	
		 Chip_GPIO_SetPinDIRInput(LPC_GPIO, KEY_A_PORT, KEY_A_PIN);
#endif
	
#ifdef	KEY_B_PIN
		 Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_1, (IOCON_FUNC1 | IOCON_MODE_INACT | MD_DIGMODE));		  						
		 Chip_GPIO_SetPinDIRInput(LPC_GPIO, KEY_B_PORT, KEY_B_PIN);
#endif	

#ifdef	KEY_C_PIN	  
		 Chip_GPIO_SetPinDIRInput(LPC_GPIO, KEY_C_PORT, KEY_C_PIN);
#endif	

#ifdef  KEY_D_PIN
		 Chip_GPIO_SetPinDIRInput(LPC_GPIO, KEY_D_PORT, KEY_D_PIN);
#endif		 
	
}


/************************************************************************************************
** Function name:     RemapKey(uint8_t scanValue)
** Descriptions:      �̰�����ӳ��
**										
** input parameters:  scanValue����ɨ��ֵ				
** output parameters: �� 
** Returned value:    �̰���ӳ��ֵ
*************************************************************************************************/
static uint8_t RemapKey(uint8_t scanValue)
{
	uint8_t scanValue_temp ;
	
#if (Key_Invalid == 0)							//��ʼ����״̬Ϊ�͵�ƽ
		scanValue_temp = scanValue ;
#elif (Key_Invalid == 1)						//��ʼ����״̬Ϊ�ߵ�ƽ
		scanValue_temp = ~scanValue ;
#endif
    switch(scanValue_temp)
    {   
        case 0x01:
            return(Key_Short_D);
        case 0x02:
            return(Key_Short_C);
        case 0x03:
            return(Key_Short_CD);
        case 0x04:
            return(Key_Short_B);
        case 0x05:
            return(Key_Short_BD);
        case 0x06:
            return(Key_Short_BC);
        case 0x08:
            return(Key_Short_A);
        case 0x09:
            return(Key_Short_AD);
        case 0x0a:
            return(Key_Short_AC);
        case 0x0c:
            return(Key_Short_AB);      
        default:
            return(Key_Ini_Sta);
    }
}


/************************************************************************************************
** Function name:     RemapLongKey(uint8_t scanValue)
** Descriptions:      ��������ӳ��
**										
** input parameters:  scanValue����ɨ��ֵ				
** output parameters: �� 
** Returned value:    ������ӳ��ֵ
*************************************************************************************************/
#ifdef Identify_Long_Click
static uint8_t RemapLongKey(uint8_t scanValue)
{
	uint8_t scanValue_temp ;
	
#if (Key_Invalid == 0)					//��ʼ����״̬Ϊ�͵�ƽ
		scanValue_temp = scanValue ;
#elif (Key_Invalid == 1)						//��ʼ����״̬Ϊ�ߵ�ƽ
		scanValue_temp = ~scanValue ;
#endif
    switch(scanValue_temp)
    {
        case 0x01:
            return(Key_Long_D);
        case 0x02:
            return(Key_Long_C);
        case 0x03:
            return(Key_Long_CD);
        case 0x04:
            return(Key_Long_B);
        case 0x05:
            return(Key_Long_BD);
        case 0x06:
            return(Key_Long_BC);
        case 0x08:
            return(Key_Long_A);
        case 0x09:
            return(Key_Long_AD);
        case 0x0a:
            return(Key_Long_AC);
        case 0x0c:
            return(Key_Long_AB);      
        default:
            return(Key_Ini_Sta);
    }
}
#endif


/************************************************************************************************
** Function name:     ScanPin(void)
** Descriptions:      ɨ�谴�������ذ���ɨ��ֵ��
**										4�������ߵ͵�ƽ���߼�ֵ
**           				  ����ABCD��˳��ֱ��ŵ�����ֵ�ĵ�3λ����2λ����1λ�͵�0λ
**            				�����������ֵ�ĵ���λ�С�
** input parameters:  �� 		
** output parameters: �� 
** Returned value:    ����ɨ��ֵ scanValue
*************************************************************************************************/
 static uint8_t ScanPin(void)
{
      uint8_t scanValue;                             //ɨ�赽�ļ�ֵ
	  uint8_t read_A_pinValue = 1;
	  uint8_t read_B_pinValue = 1;
	  uint8_t read_C_pinValue = 1;				 //�൱��δ����
	  uint8_t read_D_pinValue = 1;				 //�൱��δ����
	
#ifdef	KEY_A_PIN
        read_A_pinValue = Chip_GPIO_GetPinState(LPC_GPIO, KEY_A_PORT, KEY_A_PIN);
#endif

#ifdef	KEY_B_PIN
	    read_B_pinValue = Chip_GPIO_GetPinState(LPC_GPIO, KEY_B_PORT, KEY_B_PIN);
#endif


#ifdef	KEY_C_PIN	
	    read_C_pinValue = Chip_GPIO_GetPinState(LPC_GPIO, KEY_C_PORT, KEY_C_PIN);
#endif

#ifdef	KEY_D_PIN		
	    read_D_pinValue = Chip_GPIO_GetPinState(LPC_GPIO, KEY_D_PORT, KEY_D_PIN);
#endif
	  scanValue = (Key_Ini_Sta<<4) | (read_A_pinValue <<3) | (read_B_pinValue <<2) | (read_C_pinValue<<1) | read_D_pinValue;
      return(scanValue);       
}
