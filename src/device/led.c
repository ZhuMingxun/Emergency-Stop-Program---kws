
///////////////////////////////////////////////////////////////////////////
/// @file  led.c
/// @brief 组件源程序
/// 
/// 组件源程序包括功能：节拍中断中采集PIO3_3,通过消息传递函数传出
/// 在message.h中定义消息类型，#define MSG_TYPE_XXX             2
/// <b>Copyright （c） 
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////
/// includes
/////////////////////////////////////////////////
#include "chip.h"
#include "led.h"
#include "Message.h"

uint32_t dis_data = 0xff;
uint8_t dis_chan  = 1;

/////////////////////////////////////////////////////////////////////
/// 函数声明
////////////////////////////////////////////////////////////////////

/******数码管字段定义数组**********/
uint8_t Disp_DX[]=
{//	0-0  1-1  2-2  3-3  4-4  5-5  6-6  7-7  8-8  9-9
	0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90, 
//  10-E 11-r 12-y 13-s	14-空 15-- 16-r	17-L  18-H
    0x86,0xAF,0x91,0x92,0xff,0xbf, 0xce,0xc7, 0x89
};
/******数码管片选定义数组**********/
uint8_t Disp_PX[]=
{	
	0xfe,	//0-1
	0xfd,	//1-2
	0xfb,	//2-3
	0xf7,	//3-4 
};
/******74HC595端口定义**********/
#define HC595_DAT(x) (x)?( Chip_GPIO_SetPinState(LPC_GPIO, 1, 4, 1) ) : ( Chip_GPIO_SetPinState(LPC_GPIO, 1, 4, 0) )	 
#define HC595_CLK(x) (x)?( Chip_GPIO_SetPinState(LPC_GPIO, 3, 1, 1) ) : ( Chip_GPIO_SetPinState(LPC_GPIO, 3, 1, 0) )
#define HC595_RCK(x) (x)?( Chip_GPIO_SetPinState(LPC_GPIO, 2, 3, 1) ) : ( Chip_GPIO_SetPinState(LPC_GPIO, 2, 3, 0) )
/*************************************************************/
/**函数名称：HC595_Init                                     */
/**输入参数：无                                             */
/**函数功能：将控制74HC595的引脚设置为输出               */
/*************************************************************/
void HC595_Init(void)
{	
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 4);	 //P1.4 = DAT,OUTPUT
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 3, 1);	 //P3.1 = CLK,OUTPUT
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 3);	 //P2.3 = RCK,OUTPUT
}

/*************************************************************/
/**函数名称：HC595_Write_Data                               */
/**输入参数：data:要输入的数据                            */
/**函数功能：将数据写进74HC595                            */
/*************************************************************/
void HC595_Write_Data( uint8_t data )
{
	uint8_t i;
	uint8_t temp;
	temp = data;
	for(i = 0;i < 8;i++)
	{
		HC595_CLK(0);	        //CLK = 0
		if( temp & 0x80 ) HC595_DAT(1); 
		else  HC595_DAT(0);	 
		temp = temp<<1;  		//移位
		HC595_CLK(0);           //CLK = 0;
		HC595_CLK(1);           //CLK = 1;
		HC595_CLK(0);           //CLK = 0;	
	}
}
/*************************************************************/
/**函数名称：Disp_Out                                       */
/**输入参数：无                                             */
/**函数功能：将数据显示                                    */
/*************************************************************/
void Disp_Out()
{
	HC595_RCK(0);
	HC595_RCK(1);	
}
/*****显示延时函数***********/
void Disp_Delay(void)
{
	uint16_t i;
	for(i = 8000;i > 0;i--);
}
/******数码管显示函数*****位数：3 0 1 2 ***/
void Disp_Data( uint32_t data,uint8_t chanl )
{
	uint8_t i;
	uint32_t Temp_Data;
	uint32_t Temp_Data_FUN;
	uint8_t  Temp_Addr;
	uint8_t Wd_buff[4];
	static	uint8_t Cnt_PX = 0;

	Temp_Data = data;
	Temp_Addr = chanl;
	Temp_Data_FUN = data;
	for(i = 0;i < 3;i++)
	{
		Wd_buff[2-i] = Temp_Data%10;	  //0:百；1：十；2：个
		Temp_Data = Temp_Data/10;
	}

// 	if(Wd_buff[0] == 0)
// 	{
// 		Wd_buff[0] = 14;
// 		if(Wd_buff[1] == 0)
// 			Wd_buff[1] = 14;	
// 	}

	if(Temp_Data_FUN == _XS_LLL)
	{
		for(i = 0;i < 3;i++)
			Wd_buff[i] = 17;
	}

	if(Temp_Data_FUN == _XS_HHH)
	{
		for(i = 0;i < 3;i++)
			Wd_buff[i] = 18;
	}

	if(Temp_Data_FUN == _XS_CUOWU)
	{
		Wd_buff[0] = 10;
		Wd_buff[1] = 11;
		Wd_buff[2] = 11;
	}

	Wd_buff[3] = Temp_Addr;
	if(Cnt_PX >= 3 )Cnt_PX = 0;
	else Cnt_PX++;
	HC595_Write_Data( Disp_PX[Cnt_PX] );	
	HC595_Write_Data( Disp_DX[Wd_buff[Cnt_PX]] );
	Disp_Out();
	Disp_Delay();
}

 void led_Init(void)
{
	HC595_Init();
}
 void led_Scan(void)
{
	Disp_Data(dis_data,dis_chan);		
}	

