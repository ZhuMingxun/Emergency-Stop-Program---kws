///////////////////////////////////////////////////////////////////////////
/// @file config.h
/// @brief ����ͷ�ļ�
/// 
/// �û�����ʹ����Щ�����ʹ����ȥ����Ӧ���ʹ�ܿ���ǰ��ע�ͣ���ʹ���򱣳�
/// ���ʹ�ܿ���ǰ��ע��

///////////////////////////////////////////////////////////////////////////
#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif


/////////////////////////////////////////////////////
/// ���������ͷ�ļ�
/////////////////////////////////////////////////////
#include "board.h"
#include "lpc_types.h" 
#include "soft_timer.h"
#include "message.h"

/////////////////////////////////////////////////////
/// �û�����ʹ�õ����ʹ�ܿ���
/////////////////////////////////////////////////////

//#define ENABLE_KEY              ///< �������ʹ�ܿ���
//#define ENABLE_LCD            ///< LCD���ʹ�ܿ��� 
//#define ENABLE_RS485            ///< RS485���ʹ�ܿ���
//#define ENABLE_SSD            ///< ��������ʹ�ܿ���
//#define ENABLE_AO             ///< ģ��������ʹ�ܿ���
//#define ENABLE_FO             ///< Ƶ��������ʹ�ܿ���
//#define ENABLE_FI             ///< Ƶ�ʲ������ʹ�ܿ���
//#define ENABLE_COUNTER        ///< �������������ʹ�ܿ���
#define ENABLE_EEPROM           ///< EEPROM�洢���ʹ�ܿ���
#define ENABLE_ADC				///< ADC���ʹ�ܿ���
//#define ENABLE_LED              ///< �������ʹ�ܿ���
#define ENABLE_CAN              ///< CAN���ʹ�ܿ���
#define ENABLE_DATAIO              ///< 
#define ENBALE_YYDrive


/////////////////////////////////////////////////////
/// ϵͳ����
/////////////////////////////////////////////////////
#define SYSTEM_TIMER_INT_PERIOD  100000    ///< ϵͳʱ���ж����ڣ���λΪus,�����ޣ����ܳ���

#ifdef ENABLE_SSD
#include "ssd.h"
#endif

#ifdef ENABLE_KEY
#include "key.h"
#endif

#ifdef ENABLE_LCD
#include "lcd.h"
#endif

#ifdef ENABLE_AO
#include "Current.h"
#include "spi.h"
#endif

#ifdef ENABLE_FO
#include "timer_pwm.h"
#include "frequency.h"
#endif

#ifdef ENABLE_RS485
#include "rs485.h"
#endif

#ifdef ENABLE_CAN
#include "can.h"
#endif

#ifdef ENABLE_FI
#include "timer_cap.h"
#endif

#ifdef ENABLE_COUNTER
#include "timer_count.h"
#endif

#ifdef ENABLE_EEPROM
#include "AT24C16.h"
#endif

#ifdef ENABLE_ADC
#include "adc.h"
#endif

#ifdef ENABLE_LED
#include "led.h"
#endif

#ifdef ENABLE_DATAIO
#include "dataio.h"
#endif

#ifdef ENABLE_YYDRIVE
#include "yydrive.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* _CONFIG_H_ */
