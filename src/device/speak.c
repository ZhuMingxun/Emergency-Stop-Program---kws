#include "speak.h"
#include "can.h"
#include "ambe.h"
#include "adc.h"
#include "board_api.h"
#if NEW_BS

/*板内通用IO初始化*/
void  SBoard_Init(void)
{
	SPEAK_CHG1_IOCON();
	SPEAK_CHG2_IOCON();
	SPEAK_VOL1_IOCON();
	SPEAK_VOL2_IOCON();
	SPEAK_KEY1_IOCON();
	SPEAK_KEY2_IOCON();
	SPEAK_MUTE_IOCON();
	SPEAK_LED_IOCON();
	SGPIO_OUT(CHG1_PORT,CHG1_PINS);
	SGPIO_OUT(CHG2_PORT,CHG2_PINS);
	SGPIO_OUT(VOL1_PORT,VOL1_PINS);
	SGPIO_OUT(VOL2_PORT,VOL2_PINS);
	SGPIO_IN(KEY1_PORT,KEY1_PINS);
	SGPIO_IN(KEY2_PORT,KEY2_PINS);
	SGPIO_OUT(MUTE_PORT,MUTE_PINS);
	SGPIO_OUT(LED_PORT,LED_PINS);
	Set_Volume(1);
	Set_Charge(3);	
}

/*LED亮*/
void Led_ON(void)
{
  SGPIO_SET(LED_PORT,LED_PINS);
}	

/*LED灭*/
void Led_OFF(void)
{
  SGPIO_CLR(LED_PORT,LED_PINS);
}

/*打开静音*/
void Mute_ON(void)
{
  //SGPIO_CLR(MUTE_PORT,MUTE_PINS);	
	Chip_GPIO_SetPinState(LPC_GPIO, MUTE_PORT, MUTE_PINS, 0);
}	

/*关闭静音*/
void Mute_OFF(void)
{
  //SGPIO_SET(MUTE_PORT,MUTE_PINS);
	Chip_GPIO_SetPinState(LPC_GPIO, MUTE_PORT, MUTE_PINS, 1);
}

/*获取按键状态(打点)*/
char Get_Key1(void)
{
	if(SGPIO_GET(KEY1_PORT,KEY1_PINS) == 0)
	{
		return(1);//有按键
	}
	else
		return(0);
}

/*打点*/
char Get_Key2(void)
{
	if(SGPIO_GET(KEY2_PORT,KEY2_PINS)==0)
	{
		return(1);//有按�
		//DEBUGOUT("\r\n key2 按下");
	}
	else
	{
		return(0);//
		//DEBUGOUT("\r\n key2 未按下");
	}
}

/*设置充电档位*/
void  Set_Charge(unsigned char level)
{
	switch(level)
	{
		case 1://MAX-最大
			SGPIO_CLR(CHG1_PORT,CHG1_PINS);
			SGPIO_SET(CHG2_PORT,CHG2_PINS);
			break;
		case 2://MID-中间
			SGPIO_SET(CHG1_PORT,CHG1_PINS);
			SGPIO_CLR(CHG2_PORT,CHG2_PINS);
			break;
		case 3://MIN-最小
			SGPIO_CLR(CHG1_PORT,CHG1_PINS);
			SGPIO_CLR(CHG2_PORT,CHG2_PINS);
			break;
	}
}

/*设置音量档位*/
void  Set_Volume(unsigned char level)
{
	switch(level)
	{
		case 1://MAX-最大
			SGPIO_SET(VOL1_PORT,VOL1_PINS);
			SGPIO_SET(VOL2_PORT,VOL2_PINS);
			break;
		case 2://MID-中间
			SGPIO_CLR(VOL1_PORT,VOL1_PINS);
			SGPIO_SET(VOL2_PORT,VOL2_PINS);
			break;
		case 3://MIN-最小
			SGPIO_SET(VOL1_PORT,VOL1_PINS);
			SGPIO_CLR(VOL2_PORT,VOL2_PINS);
			break;
	}
}

/******打点开启函数******/
void Sine_Open()
{
	Mute_OFF();           //功放打开
	Led_ON();         //点亮语音灯
	//1:打点开始，2：打点结束，3：语音开始，4：语音结束
	CANSend_AMBESetSine(1);//发送打点起始数据
	//while(!AMBEGetWrStatus());
	AMBESetSine();        //本机打点发声	
	
	////////////////////
	
}

/******打点关闭函数*******/
void Sine_Close()
{
	Mute_ON();           //功放关闭
	Led_OFF();
	//1:打点开始，2：打点结束，3：语音开始，4：语音结束	
	CANSend_AMBESetSine(2);//发送打点结束数据
}
#endif
