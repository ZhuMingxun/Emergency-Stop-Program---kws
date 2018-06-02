///////////////////////////////////////////////////////////////////////////
/// @file  timer_count.h
/// @brief 计数量采集头文件
/// 
/// 计数量采集组件提供了计数采集功能，采用设匹配值，中断处理的方式
/// 要使用这个驱动,你必须按照如下步骤操作：
/// 
/// 1) 在user_config.h中打开按键组件使能开关;
/// 	 #define ENABLE_COUNTER 
/// 2) 选择需要使用的定时器，注释掉不需要使用的，定义需要使用的：
///			//#define TIMER16_0_COUNT
///				#define TIMER16_1_COUNT
/// 3) 根据定义的定时器，选择需要使用的匹配寄存器，设置计数中断值（MATCH寄存器）
///			#define 	TIMER16_0_COUNT_MATCH_VALUE		0x0000000a			//MATCH值，用于计数达到上限中断
///			#define 	TIMER16_0_COUNT_MATCH_NUM			0								//匹配寄存器号，0~3
/// 4) 在中断函数TIMERxx_x_IRQHandler()（位于timer_count.c）中根据需要添加应用，
///		 中断函数中设有中断次数变量
/// 5) 在主函数中根据全局变量传递出的计数值做相应处理，也可读取当前计数值
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
	#define 	TIMER16_0_COUNT_MATCH_VALUE		0x0000000a			//MATCH值，用于计数达到上限中断
	#define 	TIMER16_0_COUNT_MATCH_NUM			0								//匹配寄存器号，0~3
#endif

#ifdef TIMER16_1_COUNT
	#define 	TIMER16_1_COUNT_MATCH_VALUE		0x0000000a			//MATCH值，用于计数达到上限中断
	#define 	TIMER16_1_COUNT_MATCH_NUM			0								//匹配寄存器号，0~3
#endif

#ifdef TIMER32_0_COUNT
	#define 	TIMER32_0_COUNT_MATCH_VALUE		0x0000000a			//MATCH值，用于计数达到上限中断
	#define 	TIMER32_0_COUNT_MATCH_NUM			0								//匹配寄存器号，0~3
#endif	

#ifdef TIMER32_1_COUNT
	#define 	TIMER32_1_COUNT_MATCH_VALUE		0x0000000a			//MATCH值，用于计数达到上限中断
	#define 	TIMER32_1_COUNT_MATCH_NUM			0								//匹配寄存器号，0~3
#endif

	
void Timer_Count_Init(void);
	
#endif /*__TIMER_COUNT_H_ */


/***************************End file**************************************/	

