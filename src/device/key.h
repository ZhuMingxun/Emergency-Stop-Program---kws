
///////////////////////////////////////////////////////////////////////////
/// @file  key.h
/// @brief 按键组件程序头文件
/// 
/// KEY按键组件提供了按键扫描的接口
/// 要使用这个驱动,你必须按照如下步骤操作：
/// 
/// 1) 根据硬件配置相应的管脚(位于key.h)：
///    KEY_X_PORT-------------根据需要设置按键X对应的端口，如0、1、2......;
///	   KEY_X_PIN--------------根据需要设置按键X对应的PIN管脚，如0、1、2.....;
///		 例：如设置端口2第6个引脚为按键B，按如下修改：
///					#define KEY_B_PORT   2	        
///					#define KEY_B_PIN    6
/// 2) 选择是否识别长按(位于key.h);
///    #define Identify_Long_Click--支持按键长按，不需要长按时注释掉该语句;
/// 3) 选择按键的初始化状态(位于key.h);
///    #define Key_Invalid	    1  //初始按键释放状态,低电平时设置为0，高电平时设置为1;
/// 4) 在user_config.h中打开按键组件使能开关;
/// 	 #define ENABLE_KEY  
/// 5) 在Message.h中设置消息类型;
/// 	 #define MSG_TYPE_KEY          4  
/// 6) 系统时钟中断程序void SysTick_Handler(void)，Key_Scan()函数，每隔20ms执行；
/// 	 若需修改采样周期，可修改s_count_key执行的上限值；
/// 7) 在主程序中读取按键消息值，根据应用添加代码。
/// 	 
///////////////////////////////////////////////////////////////////////////

#ifndef _KEY_H_
#define _KEY_H_

#ifdef __cplusplus
 extern "C" {
#endif

/////////////////////////////////////////////////
/// 常量配置  用户可修改
/////////////////////////////////////////////////

#define KEY_A_PORT   1          //按键A端口，引脚配置P1.2 S1
#define KEY_A_PIN    2					
	 
#define KEY_B_PORT   1	        //按键B端口，引脚配置P1.1 S2
#define KEY_B_PIN    1
	 
//#define KEY_C_PORT   1	    //按键C端口，引脚配置
//#define KEY_C_PIN    2
//	 
//#define KEY_D_PORT   1        //按键D端口，引脚配置
//#define KEY_D_PIN    1
	 
//#define Identify_Long_Click         //控制是否支持按键长按，需要识别长按时定义，不需要长按时注释掉该语句。 
	 
/////////////////////////////////////////////////
/// 常量定义   一般情况不需要修改
/////////////////////////////////////////////////
#define ShortInterval			    3		//短按 按键时间节拍
#define LongInterval			    35		//长按 按键时间节拍
#define DoubleHitInterval	        10		//防止连续双击 误动 节拍

/////////////////////////////////////////////////
/// 常量定义 用户不可修改
/////////////////////////////////////////////////
#define Key_Short_A            0x58  //短按，A
#define Key_Short_B            0x54  
#define Key_Short_C            0x52  
#define Key_Short_D            0x51

#define Key_Short_AB           0x5c  //短按 同时按A和B
#define Key_Short_AC           0x5a
#define Key_Short_AD           0x59
#define Key_Short_BC           0x56
#define Key_Short_BD           0x55
#define Key_Short_CD           0x53

#ifdef Identify_Long_Click
#define Key_Long_A             0xa8  //长按按键A
#define Key_Long_B             0xa4  //长按按键B
#define Key_Long_C             0xa2
#define Key_Long_D             0xa1

#define Key_Long_AB            0xac  //长按，同时按下A和B
#define Key_Long_AC            0xaa  //长按，同时按下A和C
#define Key_Long_AD            0xa9
#define Key_Long_BC            0xa6
#define Key_Long_BD            0xa5
#define Key_Long_CD            0xa3
#endif

#define Key_Valid	              0     //按键按下状态
#define Key_Invalid	              1     //初始按键释放状态,0为低电平，1为高电平

#if (Key_Invalid == 1)
#define Key_Ini_Sta  0xff
#else 
#define Key_Ini_Sta  0x00
#endif


/********************************操作接口函数声明 **********************************************/

/************************************************************************************************
** Function name:     Key_Scan(void)
** Descriptions:      扫描按键，按键按下后采用消息传递按键信息
**										支持长短按，默认只支持短按，长按在头文件内配置
** input parameters:  无				
** output parameters: 无 
** Returned value:    无
*************************************************************************************************/
extern void Key_Scan( void );   //按键扫描函数


/************************************************************************************************
** Function name:     Key_Init
** Descriptions:      按键初始化设置  
**										
** input parameters:  无				
** output parameters: 无 
** Returned value:    无
*************************************************************************************************/
extern void Key_Init( void );   //按键初始化

#ifdef __cplusplus
}
#endif

#endif  /* _KEY_H_ */
