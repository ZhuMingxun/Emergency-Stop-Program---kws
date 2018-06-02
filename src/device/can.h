#ifndef __CAN_H_
#define __CAN_H_
#include "board.h"
#include "ccand_11xx.h"

#define ID10_09_PeriodSend  0x00000600    //11
#define ID10_09_ChangeSend  0x00000200    //01
#define ID10_09_OrderToBSM  0x00000000    //00
#define ID10_09_BSMConfirm  0x00000400    //10

#define ID08_07_Equipments  0x00000000    //00:急停开关
#define ID08_07_Soundcodes  0x00000180    //11:语音数据
#define ID05_00_DefaultID   0x0000003f    //默认63
#define CAN_SOUND_BUF_SIZE 8
extern void baudrateCalculate(uint32_t baud_rate, uint32_t *can_api_timing_cfg);
extern void CAN_error(uint32_t error_info);
extern void CAN_tx(uint8_t msg_obj_num);
extern void CAN_rx(uint8_t msg_obj_num);
extern void CAN_init(void);
extern void CAN_reset(void);
extern void can_rcv(CCAN_MSG_OBJ_T can_rev_dat);
extern void can_data_send(uint8_t mod,uint8_t *can_frame);
extern void CANSend_AMBESetSine(uint8_t soudflag);
extern bool can_send(CCAN_MSG_OBJ_T *msg_obj_s);
extern CCAN_MSG_OBJ_T msg_obj_d;
extern uint8_t can_send_len;
											 //建立一个深度为20个包的缓冲，使用can_send_len计数。
extern uint32_t CAN_ID;
extern uint8_t mod_flag[20];
extern bool reset_flag;
extern bool can_send_flag;
extern bool send_px_flag;
extern bool can_rev_monitor_flag;//can接收监视
extern uint16_t can_sound_time;
//extern bool mute_flag;
extern bool can_sound_flag;//收到语音标志
extern uint8_t can_sound_buf[CAN_SOUND_BUF_SIZE];//收到的语音帧缓冲
extern uint8_t can_sound_buf_count;//收到语音缓冲计数器，用于指定数据
extern uint8_t px_flag ;    //0:未排序，1：排序完成
extern uint8_t second,minute,hour,day,month,year;
extern bool heart_flag;//心跳标志，收到can总线命令，允许心跳或者禁心跳
extern uint8_t flag_18ON;//18V状态
	
#endif

