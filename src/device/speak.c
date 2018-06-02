#include "speak.h"
#include "can.h"
#include "ambe.h"
#include "adc.h"
#include "board_api.h"
#if NEW_BS

/*����ͨ��IO��ʼ��*/
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

/*LED��*/
void Led_ON(void)
{
  SGPIO_SET(LED_PORT,LED_PINS);
}	

/*LED��*/
void Led_OFF(void)
{
  SGPIO_CLR(LED_PORT,LED_PINS);
}

/*�򿪾���*/
void Mute_ON(void)
{
  //SGPIO_CLR(MUTE_PORT,MUTE_PINS);	
	Chip_GPIO_SetPinState(LPC_GPIO, MUTE_PORT, MUTE_PINS, 0);
}	

/*�رվ���*/
void Mute_OFF(void)
{
  //SGPIO_SET(MUTE_PORT,MUTE_PINS);
	Chip_GPIO_SetPinState(LPC_GPIO, MUTE_PORT, MUTE_PINS, 1);
}

/*��ȡ����״̬(���)*/
char Get_Key1(void)
{
	if(SGPIO_GET(KEY1_PORT,KEY1_PINS) == 0)
	{
		return(1);//�а���
	}
	else
		return(0);
}

/*���*/
char Get_Key2(void)
{
	if(SGPIO_GET(KEY2_PORT,KEY2_PINS)==0)
	{
		return(1);//�а��
		//DEBUGOUT("\r\n key2 ����");
	}
	else
	{
		return(0);//
		//DEBUGOUT("\r\n key2 δ����");
	}
}

/*���ó�絵λ*/
void  Set_Charge(unsigned char level)
{
	switch(level)
	{
		case 1://MAX-���
			SGPIO_CLR(CHG1_PORT,CHG1_PINS);
			SGPIO_SET(CHG2_PORT,CHG2_PINS);
			break;
		case 2://MID-�м�
			SGPIO_SET(CHG1_PORT,CHG1_PINS);
			SGPIO_CLR(CHG2_PORT,CHG2_PINS);
			break;
		case 3://MIN-��С
			SGPIO_CLR(CHG1_PORT,CHG1_PINS);
			SGPIO_CLR(CHG2_PORT,CHG2_PINS);
			break;
	}
}

/*����������λ*/
void  Set_Volume(unsigned char level)
{
	switch(level)
	{
		case 1://MAX-���
			SGPIO_SET(VOL1_PORT,VOL1_PINS);
			SGPIO_SET(VOL2_PORT,VOL2_PINS);
			break;
		case 2://MID-�м�
			SGPIO_CLR(VOL1_PORT,VOL1_PINS);
			SGPIO_SET(VOL2_PORT,VOL2_PINS);
			break;
		case 3://MIN-��С
			SGPIO_SET(VOL1_PORT,VOL1_PINS);
			SGPIO_CLR(VOL2_PORT,VOL2_PINS);
			break;
	}
}

/******��㿪������******/
void Sine_Open()
{
	Mute_OFF();           //���Ŵ�
	Led_ON();         //����������
	//1:��㿪ʼ��2����������3��������ʼ��4����������
	CANSend_AMBESetSine(1);//���ʹ����ʼ����
	//while(!AMBEGetWrStatus());
	AMBESetSine();        //������㷢��	
	
	////////////////////
	
}

/******���رպ���*******/
void Sine_Close()
{
	Mute_ON();           //���Źر�
	Led_OFF();
	//1:��㿪ʼ��2����������3��������ʼ��4����������	
	CANSend_AMBESetSine(2);//���ʹ���������
}
#endif
