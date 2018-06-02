///////////////////////////////////////////////////////////////////////////
/// @file  timer_count.h
/// @brief �������ɼ�ͷ�ļ�
/// 
/// �������ɼ�����ṩ�˼����ɼ����ܣ�������ƥ��ֵ���жϴ���ķ�ʽ
/// Ҫʹ���������,����밴�����²��������
/// 
/// 1) ��user_config.h�д򿪰������ʹ�ܿ���;
/// 	 #define ENABLE_COUNTER 
/// 2) ѡ����Ҫʹ�õĶ�ʱ����ע�͵�����Ҫʹ�õģ�������Ҫʹ�õģ�
///			//#define TIMER16_0_COUNT
///				#define TIMER16_1_COUNT
/// 3) ���ݶ���Ķ�ʱ����ѡ����Ҫʹ�õ�ƥ��Ĵ��������ü����ж�ֵ��MATCH�Ĵ�����
///			#define 	TIMER16_0_COUNT_MATCH_VALUE		0x0000000a			//MATCHֵ�����ڼ����ﵽ�����ж�
///			#define 	TIMER16_0_COUNT_MATCH_NUM			0								//ƥ��Ĵ����ţ�0~3
/// 4) ���жϺ���TIMERxx_x_IRQHandler()��λ��timer_count.c���и�����Ҫ���Ӧ�ã�
///		 �жϺ����������жϴ�������
/// 5) ���������и���ȫ�ֱ������ݳ��ļ���ֵ����Ӧ����Ҳ�ɶ�ȡ��ǰ����ֵ
/// 	 
///////////////////////////////////////////////////////////////////////////


#ifndef __TIMER_COUNT_H_
#define __TIMER_COUNT_H_

#ifdef __cplusplus
extern "C" {
#endif

	
//#define TIMER16_0_COUNT
#define TIMER16_1_COUNT
//#define TIMER32_0_COUNT
//#define TIMER32_1_COUNT


#ifdef TIMER16_0_COUNT
	#define 	TIMER16_0_COUNT_MATCH_VALUE		0x0000000a			//MATCHֵ�����ڼ����ﵽ�����ж�
	#define 	TIMER16_0_COUNT_MATCH_NUM			0								//ƥ��Ĵ����ţ�0~3
#endif

#ifdef TIMER16_1_COUNT
	#define 	TIMER16_1_COUNT_MATCH_VALUE		0x0000000a			//MATCHֵ�����ڼ����ﵽ�����ж�
	#define 	TIMER16_1_COUNT_MATCH_NUM			0								//ƥ��Ĵ����ţ�0~3
#endif

#ifdef TIMER32_0_COUNT
	#define 	TIMER32_0_COUNT_MATCH_VALUE		0x0000000a			//MATCHֵ�����ڼ����ﵽ�����ж�
	#define 	TIMER32_0_COUNT_MATCH_NUM			0								//ƥ��Ĵ����ţ�0~3
#endif	

#ifdef TIMER32_1_COUNT
	#define 	TIMER32_1_COUNT_MATCH_VALUE		0x0000000a			//MATCHֵ�����ڼ����ﵽ�����ж�
	#define 	TIMER32_1_COUNT_MATCH_NUM			0								//ƥ��Ĵ����ţ�0~3
#endif

	
void Timer_Count_Init(void);
	
#endif /*__TIMER_COUNT_H_ */


/***************************End file**************************************/	

