#ifndef __ADC_H_ 
#define __ADC_H_

#include "chip.h"

extern void ADC_IRQHandler( void );//AD中断处理函数
extern void ADCInit( void );//AD初始化函数
extern uint16_t ADC_IRQ_Value_final;//AD最终数值

#endif 

