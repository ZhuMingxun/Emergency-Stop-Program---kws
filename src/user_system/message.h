///////////////////////////////////////////////////////////////////////////
/// @file message.h
/// @brief 消息队列
/// 
/// 先入先出，优先级功能可以配置，默认没有优先级
/// 
/// @author  
/// @version 
/// @date   
/// 
/// <b>Copyright 
///////////////////////////////////////////////////////////////////////////
#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif


/////////////////////////////////////////////////////
/// 包含头文件
/////////////////////////////////////////////////////
#include "lpc_types.h" 

/////////////////////////////////////////////////////
/// 宏定义
/////////////////////////////////////////////////////
/// 用户可配置
#define MESSAGE_QUEUE_SIZE     10         /// 消息队列大小,应小于256
//#define PRIORITY_MESSAGE_QUEUE          /// 是否采用优先级队列，默认不采用，如需采用，则去掉注释"//"

/// 消息类型定义，用户可增加自定义消息类型
#define MSG_TYPE_SOFT_TIMER       1       ///< 软件定时器消息
#define MSG_TYPE_ADC              3       ///< ADC消息
#define MSG_TYPE_KEY              4       ///< 按键消息
#define MSG_TYPE_RS485_RXDone	  5		  ///< RS485接收到一帧消息

/// 系统使用，用户不可更改
#define MSG_PRIORITY_HIGH         1       /// 高优先级消息
#define MSG_PRIORITY_LOW          0       /// 低优先级消息

/////////////////////////////////////////////////////
/// 结构定义
/////////////////////////////////////////////////////
/// 消息结构
typedef struct _MESSAGE
{ 
    uint32_t  value;					  ///< 消息值
    uint8_t   type;                       ///< 消息类型
    uint8_t   priority;                   ///< 优先级，目前支持2级优先级(高、低)
} MSG_TypeDef, *P_MSG_TypeDef;

/// 消息队列结构，采用环形缓冲区
typedef struct _MESSAGE_QUEQUE
{
    uint8_t readIndex;						    ///< 读索引
    uint8_t writeIndex;						    ///< 写索引
    uint8_t numMsgInQueue;					    ///< 队列中消息数量
    MSG_TypeDef messageArray[MESSAGE_QUEUE_SIZE];   ///< 消息数组
} MSG_QUEQUE_TypeDef, *P_MSG_QUEQUE_TypeDef;

/////////////////////////////////////////////////////
/// 变量声明
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// 接口声明
/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/// @brief 	消息队列初始化
///
/// @param[in]  无
/// @reture     无
///
//////////////////////////////////////////////////////////////////////////////////
extern void InitMessageQueue (void);

//////////////////////////////////////////////////////////////////////////////////
/// @brief 	发送新消息到消息队列
///
/// @param[in]  msgType  消息类型
/// @param[in]  msgPriority  消息优先级
/// @param[in]  msgValue  消息传递值
/// @reture     成功返回TRUE,失败返回FALSE(队列已满)
///
//////////////////////////////////////////////////////////////////////////////////
extern uint8_t  PostMessage (uint8_t msgType, uint8_t msgPriority, uint32_t msgValue);

//////////////////////////////////////////////////////////////////////////////////
/// @brief 	从消息队列读取消息
///
/// @param[out]  pNewMsg   指向新消息的指针
/// @reture     成功返回TRUE,失败返回FALSE(队列为空)
///
//////////////////////////////////////////////////////////////////////////////////
extern uint8_t PendMessage (P_MSG_TypeDef pNewMsg);

#ifdef __cplusplus
}
#endif

#endif /* _MESSAGE_H_ */
