#ifndef __T16B0_H__
#define __T16B0_H__

extern void T16B0_init(void);
extern void TIM16B0_INT_init(uint16_t ms);
extern void TIMER16_0_IRQHandler(void);	

extern uint16_t flag_1000ms ;
extern uint16_t flag_1000ms_count ;

extern uint8_t flag_1500ms ;
extern uint8_t flag_1500ms_count ;

extern uint8_t flag_10ms;
extern uint8_t flag_10ms_p1;
extern uint8_t flag_10ms_p2;
extern uint8_t flag_10ms_count;
extern uint8_t flag_CAN_Send;//用于判断CAN发送成功定时，10ms

extern uint8_t flag_5s ;
extern uint16_t flag_5s_count ;

extern uint8_t flag_20ms;
extern uint8_t flag_20ms_count;

extern uint8_t second,minute,hour,day,month,year;

extern uint8_t flag_18V;


#endif





