#ifndef __DATAIO_H__
#define __DATAIO_H__

#include "chip.h"
#include "cari_config.h"
#if NEW_BS
#define D18V_PORT 1
#define D18V_PORT_BIT 2  //����18V��ѹ������Ϊp1.2

#define JDQ_PORT 3
#define JDQ_PORT_BIT 0  //�̵���p3.0


#define POWEROFF_PORT 1
#define POWEROFF_PORT_BIT 5 //���Ƶ�ع���ض�����

#define JT_PORT 2
#define JT_PORT_BIT 0	//��ͣ����
#define PP1_PORT 1    //һ����ƫ
#define PP1_PORT_BIT 4
#define PP2_PORT 2		//������ƫ  //����ʵ�����ߣ���һ���Ͷ����Ķ˿ڶԵ� 2017.12.19 ��ά��
#define PP2_PORT_BIT 3
#define DEBUG_LED_PORT 2		//AMBE1
#define DEBUG_LED_BIT 5
#define	BTPWR_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_5, (IOCON_FUNC0 | IOCON_MODE_INACT | MD_DIGMODE|MD_PUP));

#else

#define JDQ_PORT 3
#define JDQ_PORT_BIT 0  //�̵���p3.0
#define JT_PORT 0
#define JT_PORT_BIT 8	//��ͣ����
#define PP1_PORT 2    //һ����ƫ
#define PP1_PORT_BIT 9
#define PP2_PORT 2		//������ƫ
#define PP2_PORT_BIT 10
#define DEBUG_LED_PORT 2		//������ƫ
#define DEBUG_LED_BIT 5
#endif


#define	JT_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_0, (IOCON_FUNC0 | IOCON_MODE_INACT ));
#define	PP1_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_3, (IOCON_FUNC0 | IOCON_MODE_INACT ));
#define	PP2_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_4, (IOCON_FUNC0 | IOCON_MODE_INACT ));
extern void Gpio_Init(void);
extern void JDQ_State_Open(void);
extern void JDQ_State_Close(void);
extern uint8_t BSdata(void);
#if NEW_BS
extern uint8_t D18V_State(void);
extern void POWEROFF_State_Open(void);
extern void POWEROFF_State_Close(void);
#endif
extern 	uint8_t Data0_State ;
extern uint8_t JT_State(void);
extern uint8_t JDQ_State;
#endif

