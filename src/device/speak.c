#include "speak.h"
#include "can.h"
#include "ambe.h"
#include "adc.h"
#include "board_api.h"
#if NEW_BS

/*°åÄÚÍ¨ÓÃIO³õÊ¼»¯*/
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

/*LEDÁÁ*/
void Led_ON(void)
{
  SGPIO_SET(LED_PORT,LED_PINS);
}	

/*LEDÃð*/
void Led_OFF(void)
{
  SGPIO_CLR(LED_PORT,LED_PINS);
}

/*´ò¿ª¾²Òô*/
void Mute_ON(void)
{
  //SGPIO_CLR(MUTE_PORT,MUTE_PINS);	
	Chip_GPIO_SetPinState(LPC_GPIO, MUTE_PORT, MUTE_PINS, 0);
}	

/*¹Ø±Õ¾²Òô*/
void Mute_OFF(void)
{
  //SGPIO_SET(MUTE_PORT,MUTE_PINS);
	Chip_GPIO_SetPinState(LPC_GPIO, MUTE_PORT, MUTE_PINS, 1);
}

/*»ñÈ¡°´¼ü×´Ì¬(´òµã)*/
char Get_Key1(void)
{
	if(SGPIO_GET(KEY1_PORT,KEY1_PINS) == 0)
	{
		return(1);//ÓÐ°´¼ü
	}
	else
		return(0);
}

/*´òµã*/
char Get_Key2(void)
{
	if(SGPIO_GET(KEY2_PORT,KEY2_PINS)==0)
	{
		return(1);//ÓÐ°´¼
		//DEBUGOUT("\r\n key2 °´ÏÂ");
	}
	else
	{
		return(0);//
		//DEBUGOUT("\r\n key2 Î´°´ÏÂ");
	}
}

/*ÉèÖÃ³äµçµµÎ»*/
void  Set_Charge(unsigned char level)
{
	switch(level)
	{
		case 1://MAX-×î´ó
			SGPIO_CLR(CHG1_PORT,CHG1_PINS);
			SGPIO_SET(CHG2_PORT,CHG2_PINS);
			break;
		case 2://MID-ÖÐ¼ä
			SGPIO_SET(CHG1_PORT,CHG1_PINS);
			SGPIO_CLR(CHG2_PORT,CHG2_PINS);
			break;
		case 3://MIN-×îÐ¡
			SGPIO_CLR(CHG1_PORT,CHG1_PINS);
			SGPIO_CLR(CHG2_PORT,CHG2_PINS);
			break;
	}
}

/*ÉèÖÃÒôÁ¿µµÎ»*/
void  Set_Volume(unsigned char level)
{
	switch(level)
	{
		case 1://MAX-×î´ó
			SGPIO_SET(VOL1_PORT,VOL1_PINS);
			SGPIO_SET(VOL2_PORT,VOL2_PINS);
			break;
		case 2://MID-ÖÐ¼ä
			SGPIO_CLR(VOL1_PORT,VOL1_PINS);
			SGPIO_SET(VOL2_PORT,VOL2_PINS);
			break;
		case 3://MIN-×îÐ¡
			SGPIO_SET(VOL1_PORT,VOL1_PINS);
			SGPIO_CLR(VOL2_PORT,VOL2_PINS);
			break;
	}
}

/******´òµã¿ªÆôº¯Êý******/
void Sine_Open()
{
	Mute_OFF();           //¹¦·Å´ò¿ª
	Led_ON();         //µãÁÁÓïÒôµÆ
	//1:´òµã¿ªÊ¼£¬2£º´òµã½áÊø£¬3£ºÓïÒô¿ªÊ¼£¬4£ºÓïÒô½áÊø
	CANSend_AMBESetSine(1);//·¢ËÍ´òµãÆðÊ¼Êý¾Ý
	//while(!AMBEGetWrStatus());
	AMBESetSine();        //±¾»ú´òµã·¢Éù	
	
	////////////////////
	
}

/******´òµã¹Ø±Õº¯Êý*******/
void Sine_Close()
{
	Mute_ON();           //¹¦·Å¹Ø±Õ
	Led_OFF();
	//1:´òµã¿ªÊ¼£¬2£º´òµã½áÊø£¬3£ºÓïÒô¿ªÊ¼£¬4£ºÓïÒô½áÊø	
	CANSend_AMBESetSine(2);//·¢ËÍ´òµã½áÊøÊý¾Ý
}
#endif
