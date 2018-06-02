
///////////////////////////////////////////////////////////////////////////
/// @file  key.h
/// @brief �����������ͷ�ļ�
/// 
/// KEY��������ṩ�˰���ɨ��Ľӿ�
/// Ҫʹ���������,����밴�����²��������
/// 
/// 1) ����Ӳ��������Ӧ�Ĺܽ�(λ��key.h)��
///    KEY_X_PORT-------------������Ҫ���ð���X��Ӧ�Ķ˿ڣ���0��1��2......;
///	   KEY_X_PIN--------------������Ҫ���ð���X��Ӧ��PIN�ܽţ���0��1��2.....;
///		 ���������ö˿�2��6������Ϊ����B���������޸ģ�
///					#define KEY_B_PORT   2	        
///					#define KEY_B_PIN    6
/// 2) ѡ���Ƿ�ʶ�𳤰�(λ��key.h);
///    #define Identify_Long_Click--֧�ְ�������������Ҫ����ʱע�͵������;
/// 3) ѡ�񰴼��ĳ�ʼ��״̬(λ��key.h);
///    #define Key_Invalid	    1  //��ʼ�����ͷ�״̬,�͵�ƽʱ����Ϊ0���ߵ�ƽʱ����Ϊ1;
/// 4) ��user_config.h�д򿪰������ʹ�ܿ���;
/// 	 #define ENABLE_KEY  
/// 5) ��Message.h��������Ϣ����;
/// 	 #define MSG_TYPE_KEY          4  
/// 6) ϵͳʱ���жϳ���void SysTick_Handler(void)��Key_Scan()������ÿ��20msִ�У�
/// 	 �����޸Ĳ������ڣ����޸�s_count_keyִ�е�����ֵ��
/// 7) ���������ж�ȡ������Ϣֵ������Ӧ����Ӵ��롣
/// 	 
///////////////////////////////////////////////////////////////////////////

#ifndef _KEY_H_
#define _KEY_H_

#ifdef __cplusplus
 extern "C" {
#endif

/////////////////////////////////////////////////
/// ��������  �û����޸�
/////////////////////////////////////////////////

#define KEY_A_PORT   1          //����A�˿ڣ���������P1.2 S1
#define KEY_A_PIN    2					
	 
#define KEY_B_PORT   1	        //����B�˿ڣ���������P1.1 S2
#define KEY_B_PIN    1
	 
//#define KEY_C_PORT   1	    //����C�˿ڣ���������
//#define KEY_C_PIN    2
//	 
//#define KEY_D_PORT   1        //����D�˿ڣ���������
//#define KEY_D_PIN    1
	 
//#define Identify_Long_Click         //�����Ƿ�֧�ְ�����������Ҫʶ�𳤰�ʱ���壬����Ҫ����ʱע�͵�����䡣 
	 
/////////////////////////////////////////////////
/// ��������   һ���������Ҫ�޸�
/////////////////////////////////////////////////
#define ShortInterval			    3		//�̰� ����ʱ�����
#define LongInterval			    35		//���� ����ʱ�����
#define DoubleHitInterval	        10		//��ֹ����˫�� �� ����

/////////////////////////////////////////////////
/// �������� �û������޸�
/////////////////////////////////////////////////
#define Key_Short_A            0x58  //�̰���A
#define Key_Short_B            0x54  
#define Key_Short_C            0x52  
#define Key_Short_D            0x51

#define Key_Short_AB           0x5c  //�̰� ͬʱ��A��B
#define Key_Short_AC           0x5a
#define Key_Short_AD           0x59
#define Key_Short_BC           0x56
#define Key_Short_BD           0x55
#define Key_Short_CD           0x53

#ifdef Identify_Long_Click
#define Key_Long_A             0xa8  //��������A
#define Key_Long_B             0xa4  //��������B
#define Key_Long_C             0xa2
#define Key_Long_D             0xa1

#define Key_Long_AB            0xac  //������ͬʱ����A��B
#define Key_Long_AC            0xaa  //������ͬʱ����A��C
#define Key_Long_AD            0xa9
#define Key_Long_BC            0xa6
#define Key_Long_BD            0xa5
#define Key_Long_CD            0xa3
#endif

#define Key_Valid	              0     //��������״̬
#define Key_Invalid	              1     //��ʼ�����ͷ�״̬,0Ϊ�͵�ƽ��1Ϊ�ߵ�ƽ

#if (Key_Invalid == 1)
#define Key_Ini_Sta  0xff
#else 
#define Key_Ini_Sta  0x00
#endif


/********************************�����ӿں������� **********************************************/

/************************************************************************************************
** Function name:     Key_Scan(void)
** Descriptions:      ɨ�谴�����������º������Ϣ���ݰ�����Ϣ
**										֧�ֳ��̰���Ĭ��ֻ֧�ֶ̰���������ͷ�ļ�������
** input parameters:  ��				
** output parameters: �� 
** Returned value:    ��
*************************************************************************************************/
extern void Key_Scan( void );   //����ɨ�躯��


/************************************************************************************************
** Function name:     Key_Init
** Descriptions:      ������ʼ������  
**										
** input parameters:  ��				
** output parameters: �� 
** Returned value:    ��
*************************************************************************************************/
extern void Key_Init( void );   //������ʼ��

#ifdef __cplusplus
}
#endif

#endif  /* _KEY_H_ */
