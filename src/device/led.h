
///////////////////////////////////////////////////////////////////////////
/// @file  xxx.h
/// @brief 组件程序头文件
/// 
/// 
/// 要使用这个驱动,你必须按照如下步骤操作：
/// 
/// 1) ：
/// 
/// 2) ;
///    
/// 2) ;
/// 3) ;
///////////////////////////////////////////////////////////////////////////

#ifndef _LED_H_
#define _LED_H_

#ifdef __cplusplus
 extern "C" {
#endif

	 
/////////////////////////////////////////////////
/// 常量定义   一般情况不需要修改
/////////////////////////////////////////////////
#define ShortInterval			    3		//短按 按键时间节拍
#define LongInterval			    35		//长按 按键时间节拍
#define DoubleHitInterval	        10		//防止连续双击 误动 节拍

void led_Scan(void);
void led_Init(void);

#define	_XS_LLL  		255		//上传数据，在数码管上显示空。
#define	_XS_HHH		  254		//不上传，在数码管上显示－－－，标识进入某路调校。
#define	_XS_CUOWU		253		//不上传，在数码管上显示Eгг，标识出错了。

extern void Disp_Data(uint32_t data,uint8_t chanl);
extern uint32_t dis_data;
extern uint8_t dis_chan;


#ifdef __cplusplus
}
#endif

#endif  /* _XXX_H_ */
