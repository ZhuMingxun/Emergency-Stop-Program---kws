///////////////////////////////////////////////////////////////////////////
/// @file config.h
/// @brief 配置头文件
/// 
/// 用户配置使用哪些组件，使用则去掉相应组件使能开关前的注释，不使用则保持
/// 组件使能开关前的注释

///////////////////////////////////////////////////////////////////////////
#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif


/////////////////////////////////////////////////////
/// 必须包含的头文件
/////////////////////////////////////////////////////
#include "board.h"
#include "lpc_types.h" 
#include "soft_timer.h"
#include "message.h"

/////////////////////////////////////////////////////
/// 用户配置使用的组件使能开关
/////////////////////////////////////////////////////

//#define ENABLE_KEY              ///< 按键组件使能开关
//#define ENABLE_LCD            ///< LCD组件使能开关 
//#define ENABLE_RS485            ///< RS485组件使能开关
//#define ENABLE_SSD            ///< 数码管组件使能开关
//#define ENABLE_AO             ///< 模拟输出组件使能开关
//#define ENABLE_FO             ///< 频率输出组件使能开关
//#define ENABLE_FI             ///< 频率测量组件使能开关
//#define ENABLE_COUNTER        ///< 计数量测量组件使能开关
#define ENABLE_EEPROM           ///< EEPROM存储组件使能开关
#define ENABLE_ADC				///< ADC组件使能开关
//#define ENABLE_LED              ///< 按键组件使能开关
#define ENABLE_CAN              ///< CAN组件使能开关
#define ENABLE_DATAIO              ///< 
#define ENBALE_YYDrive


/////////////////////////////////////////////////////
/// 系统配置
/////////////////////////////////////////////////////
#define SYSTEM_TIMER_INT_PERIOD  100000    ///< 系统时钟中断周期，单位为us,有上限，不能超过

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
