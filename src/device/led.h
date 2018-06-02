
///////////////////////////////////////////////////////////////////////////
/// @file  xxx.h
/// @brief �������ͷ�ļ�
/// 
/// 
/// Ҫʹ���������,����밴�����²��������
/// 
/// 1) ��
/// 
/// 2) ;
///    
/// 2) ;
/// 3) ;
///////////////////////////////////////////////////////////////////////////

#ifndef _LED_H_
#define _LED_H_

#ifdef __cplusplus
 extern "C" {
#endif

	 
/////////////////////////////////////////////////
/// ��������   һ���������Ҫ�޸�
/////////////////////////////////////////////////
#define ShortInterval			    3		//�̰� ����ʱ�����
#define LongInterval			    35		//���� ����ʱ�����
#define DoubleHitInterval	        10		//��ֹ����˫�� �� ����

void led_Scan(void);
void led_Init(void);

#define	_XS_LLL  		255		//�ϴ����ݣ������������ʾ�ա�
#define	_XS_HHH		  254		//���ϴ��������������ʾ����������ʶ����ĳ·��У��
#define	_XS_CUOWU		253		//���ϴ��������������ʾE�ԧԣ���ʶ�����ˡ�

extern void Disp_Data(uint32_t data,uint8_t chanl);
extern uint32_t dis_data;
extern uint8_t dis_chan;


#ifdef __cplusplus
}
#endif

#endif  /* _XXX_H_ */
