///////////////////////////////////////////////////////////////////////////
/// @file LCD.h
/// @brief LCD12864��ʾ��ͷ�ļ�
/// 
/// LCD��ʾģ���ṩ��һ����12864����Һ���Ľӿ�
/// Ҫʹ���������,����밴�����²��������
/// 1) ���ݴ������޸İ�����ͷ�ļ�(λ��LCD.h)��
/// 2) ����Ӳ����ƽ�������(λ��LCD.h)��
///    LCD_X_PORT-------------��LCD�����ĵ�Ƭ���Ķ˿ڣ�
///    LCD_X------------------��LCD�����ĵ�Ƭ��������źţ�
///	3) �û�ʹ��LCDʱ����Ҫ�ȶ�LCD���г�ʼ��LCD_Init()(λ��LCD.c)��
/// 4) ��ʼ����ɺ���Ҫ������ʾ����

///////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "lcd.h"

void LCD_WriteOrder(uint8_t order);




//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    �����ʱ
/// 
/// @param[in]  delay����ʱʱ��
/// @return     �� 
/// @note       ����LCD.h�ļ��е�LCD_DELAY�����޸�
//////////////////////////////////////////////////////////////////////////////////
void LCD_Delay(uint32_t delay)
{
    uint32_t i;
    for (i = 0; i < delay; i++);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ��LCD���������ų�ʼ��
/// 
/// @param[in]  GPIOx ���˿ں�
/// @param[in]  GPIO_Pin �����ź�
/// @return     �� 
/// @note       ��ʼ����ɺ�������LCD������ž�Ϊ���ٸ����ģʽ����ʼֵΪ0��
//////////////////////////////////////////////////////////////////////////////////
void LCD_Pin_Init()
{
	Chip_GPIO_SetPortDIROutput(LPC_GPIO, 2, 0xfff);//ȫ������Ϊ���

	Chip_GPIO_SetPortOutLow(LPC_GPIO, 2, 0xfff);   //ȫ�����Ϊ0
}



//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ��Һ����ʾ
///  
/// @param[in]  ��
/// @return     ��
/// @note 
//////////////////////////////////////////////////////////////////////////////////
void LCD_DisplayOn(void)
{
  	LCD_WriteOrder(ORDER_BASICI);    //����ָ��
	
	LCD_WriteOrder(ORDER_DISPON);	//������ʾָ��
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    �ر�Һ����ʾ�����ܹر���
///  
/// @param[in]  ��
/// @return     ��
/// @note 
//////////////////////////////////////////////////////////////////////////////////
void LCD_DisplayOff(void)
{
    LCD_WriteOrder(ORDER_BASICI);	 //����ָ��
	
	LCD_WriteOrder(ORDER_DISPOFF);	 //�ر���ʾָ��
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    �ȴ�LCD����
/// 
/// @param[in]  ��
/// @return     �� 
/// @note       �ж�LCD�Ƿ�æ״̬���ȴ�LCD����
//////////////////////////////////////////////////////////////////////////////////
void LCD_ReadBusy(void)
{
	uint8_t busyread;
	
	LCD_EN_PIN_DISENABLE;
	
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, LCD_PORT, LCD_DB7);   //��DB7��Ϊ����
	
	Chip_GPIO_SetPinOutLow(LPC_GPIO, LCD_PORT, LCD_RS);		//RS = 1,ָ�����
	
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, LCD_PORT, LCD_RW);    //RW = 1��������
	
	LCD_EN_PIN_ENABLE;										//EN = 1
	

  	busyread = Chip_GPIO_GetPinState(LPC_GPIO, LCD_PORT, LCD_DB7); //��DB7
	
	while(busyread)                                                //�ȴ�DB7 = 0�����У�
	{
		busyread = Chip_GPIO_GetPinState(LPC_GPIO, LCD_PORT, LCD_DB7);
	}
	
	LCD_EN_PIN_DISENABLE;										//EN = 0
	
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LCD_PORT, LCD_DB7);		//DB7������Ϊ���
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ��ȡLCD�������λ��
/// 
/// @param[in]  ��
/// @return     readac �������λ��
/// @note       
//////////////////////////////////////////////////////////////////////////////////
uint8_t LCD_ReadAc(void)
{
	uint8_t readac=0x00;
	
	Chip_GPIO_SetPortDIRInput(LPC_GPIO,LCD_PORT,0x07f);		//DB0-DB6��Ϊ����
	
	Chip_GPIO_SetPinOutLow(LPC_GPIO, LCD_PORT, LCD_RS);		//RS = 0�����ݲ���
	
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, LCD_PORT, LCD_RW);	//RW = 0,������
	
	LCD_EN_PIN_ENABLE;										//EN = 1,
	
	LCD_Delay(LCD_DELAY);
	
	readac = Chip_GPIO_GetPortValue(LPC_GPIO,LCD_PORT);		//��ȡDB0-DB7
	
	readac &= 0x7f;
	
	LCD_EN_PIN_DISENABLE;

	Chip_GPIO_SetPortDIROutput(LPC_GPIO, 2, 0xfff);//ȫ������Ϊ���
	
	return readac;
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ����LCD������
/// 
/// @param[in]  ��
/// @return     readdata������������
/// @note       
//////////////////////////////////////////////////////////////////////////////////
uint8_t LCD_ReadData(void)
{
	uint8_t readdata=0x00;
	
	Chip_GPIO_SetPortDIRInput(LPC_GPIO, LCD_PORT, 0x0ff);
	
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, LCD_PORT, LCD_RS);
	
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, LCD_PORT, LCD_RW);
	
	LCD_EN_PIN_DISENABLE;
	
	LCD_EN_PIN_ENABLE;
	
	LCD_Delay(LCD_DELAY);
	
    readdata = (uint8_t)(Chip_GPIO_GetPortValue(LPC_GPIO,LCD_PORT));
	
	LCD_EN_PIN_DISENABLE;
	
	Chip_GPIO_SetPortDIROutput(LPC_GPIO, 2, 0xfff);//ȫ������Ϊ���
	
	return readdata;
}

//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ����дָ��
/// 
/// @param[in]  order ��LCDָ��
/// @return     �� 
/// @note       

//////////////////////////////////////////////////////////////////////////////////
void LCD_WriteOrder(uint8_t order)
{
	LCD_ReadBusy();

	Chip_GPIO_SetPinOutLow(LPC_GPIO, LCD_PORT, LCD_RS);
	
	Chip_GPIO_SetPinOutLow(LPC_GPIO, LCD_PORT, LCD_RW);
	
	Chip_GPIO_SetPortValue(LPC_GPIO, LCD_PORT, 0x0ff & order);
	
	LCD_EN_PIN_ENABLE;
	
  	LCD_Delay(LCD_DELAY);
	
	LCD_EN_PIN_DISENABLE;
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ����LCDд����
/// 
/// @param[in]  data������
/// @return     �� 
/// @note       
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_WriteData(uint8_t data)
{
	LCD_ReadBusy();
	
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, LCD_PORT, LCD_RS);
	
	Chip_GPIO_SetPinOutLow(LPC_GPIO, LCD_PORT, LCD_RW);
	
	Chip_GPIO_SetPortValue(LPC_GPIO, LCD_PORT, 0x100 | data);
	
	LCD_EN_PIN_ENABLE;
	
	LCD_Delay(LCD_DELAY);
	
	LCD_EN_PIN_DISENABLE;
}

//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ���LCD������ʾ���ݣ�������
/// 
/// @param[in]  ��
/// @return     �� 
/// @note       
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_Clean(void)
{
	LCD_ClearDGRAM();
	LCD_WriteOrder(ORDER_CLEAR);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ���LCDĳһ�е���ʾ����
/// 
/// @param[in]  line �кţ�0~3
/// @return     �� 
/// @note       
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_CleanLine(uint8_t line)
{
	uint8_t i;
	LCD_SetCursor(line,0);
	for (i=0;i<16;i++)
	{
		LCD_WriteData(0x20U);
	}
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    �����
/// 
/// @param[in]  ��
/// @return     �� 
/// @note       
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_CursorOn(void)
{
	LCD_WriteOrder(ORDER_CURSORON);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    �ع��
/// 
/// @param[in]  ��
/// @return     �� 
/// @note       
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_CursorOff(void)
{
	LCD_WriteOrder(ORDER_CURSOROFF);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ���ù��λ��
/// 
/// @param[in]  line �кţ�0~3
/// @param[in]  column �к� 0~7
///
/// @return     �� 
/// @note       
//////////////////////////////////////////////////////////////////////////////////
void LCD_SetCursor(uint8_t line,uint8_t column)
{
	uint8_t position=0;
	switch (line)
	{
		case 0 : position = 0x80 + column;
		break;
		case 1 : position = 0x90 + column;
		break;
		case 2 : position = 0x88 + column;
		break;
		case 3 : position = 0x98 + column;
		break;
		default : break;
	}
	LCD_WriteOrder(position);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ��ʾһ���ַ�
/// 
/// @param[in]  displaybuff �ַ����룬ͨ��LCD��Ӧ�������ֲ�OCMJ4�8C-14.PDF�в���
/// @return     �� 
/// @note       һ�����λ�ÿ���д�������ַ���
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_DisplayChar(uint8_t displaybuff)
{
	LCD_WriteData(displaybuff);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ��ʾһ����
/// 
/// @param[in]  displaybuff �ִ��룬֧��BIG5�����GB����
/// @return     �� 
/// @note       һ�����λ��дһ����
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_DisplayWord(uint8_t *displaybuff)
{
	LCD_WriteData(displaybuff[0]);
	LCD_WriteData(displaybuff[1]);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ��ĳ��λ������ʾһ����
/// 
/// @param[in]  line �кţ�0~3
/// @param[in]  column �кţ�0~7
/// @param[in]  displaybuff �ִ��룬֧��BIG5�����GB����
/// @return     �� 
/// @note       
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_DisplayWordXY(uint8_t line,uint8_t column,uint8_t *displaybuff)
{
	LCD_SetCursor(line,column);
	LCD_DisplayWord(displaybuff);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ��ʾn����
/// 
/// @param[in]  string�ִ��׵�ַ
/// @param[in]  start ��Ҫ��ʾ�����ִ��е�λ�ã������ִ���һ����ַ����Ϊ0
/// @param[in]  len ��Ҫ��ʾ�ֵĸ���
/// @return     �� 
/// @note       �ִ����ֱ�����˫�ֽڣ����ڵ��ֽ������£�Ӣ����ĸ�����֣�������
///             �����ַ��ȣ�������ż����������ǵ����ַ������ÿո��㡣
///             eg ���£ã���Ϊ˫�ֽڿ���ֱ�ӷ����ִ��е��ã���ABCabc�ǵ��ֽڣ�
///             ��������ż�������������ÿո���
///
//////////////////////////////////////////////////////////////////////////////////
uint8_t acdata;
void LCD_DisplayString(uint8_t *string,uint8_t start,uint8_t len)
{
	uint8_t i;
	
	for(i=0;i<len;i++)
	{

		acdata = LCD_ReadAc();
		switch (acdata)
		{
			case 0x07 : 
			LCD_WriteData(string[2*(start+i)]);
			LCD_WriteData(string[2*(start+i) + 1]);
			LCD_WriteOrder(0x90);
			break;
			
			case 0x0f : 
			LCD_WriteData(string[2*(start+i)]);
			LCD_WriteData(string[2*(start+i)+1]);
			LCD_WriteOrder(0x98);
			break;
			
			case 0x17 : 
			LCD_WriteData(string[2*(start+i)]);
			LCD_WriteData(string[2*(start+i)+1]);
			LCD_WriteOrder(0x88);
			break;
			
			case 0x1f : 
			LCD_WriteData(string[2*(start+i)]);
			LCD_WriteData(string[2*(start+i)+1]);
			LCD_WriteOrder(0x80);
			break;
			
			default : 
			LCD_WriteData(string[2*(start+i)]);
			LCD_WriteData(string[2*(start+i)+1]);
			break;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ��ĳ��λ�ÿ�ʼ��ʾn����
///
/// @param[in]  line �кţ�0~3
/// @param[in]  column �кţ�0~7
/// @param[in]  string�ִ��׵�ַ
/// @param[in]  start ��Ҫ��ʾ�����ִ��е�λ�ã������ִ���һ����ַ����Ϊ0
/// @param[in]  len ��Ҫ��ʾ�ֵĸ���
/// @return     �� 
/// @note       
//////////////////////////////////////////////////////////////////////////////////
void LCD_DisplayStringXY(uint8_t line,uint8_t column,uint8_t *displaybuff,uint8_t start,uint8_t len)
{
	LCD_SetCursor(line,column);
	LCD_DisplayString(displaybuff,start,len);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ���λͼ�洢��
///
/// @param[in]  ��
/// @return     �� 
/// @note       
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_ClearDGRAM(void)
{
	uint8_t  a = 0;
	uint8_t  b = 0;
	uint8_t  c = 0;
	uint8_t  lcd_x = 0x80;
	uint8_t  lcd_y = 0x80;
	
	for(a=0;a<2;a++)
	{
		for(b=0;b<32;b++)
		{			
			for(c=0;c<8;c++)
			{	
				LCD_WriteOrder(ORDER_EXPANDI);
				LCD_WriteOrder(lcd_y+b);
				LCD_WriteOrder(lcd_x+c);	      	
				LCD_WriteData(0x00);
				LCD_WriteData(0x00);
			}
		}
		lcd_x = 0x88;
	}
    LCD_WriteOrder(ORDER_BASICI);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ��ʾһ��ͼƬ
///
/// @param[in]  pPictureArrayλͼȡģ�����׵�ַ
/// @return     �� 
/// @note       
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_LoadPicture(uint8_t *pPictureArray)
{
	uint8_t  a = 0;
	uint8_t  b = 0;
	uint8_t  c = 0;
	uint16_t d = 0;
	uint8_t  lcd_x = 0x80;
	uint8_t  lcd_y = 0x80;
	LCD_ClearDGRAM();
	
	for(a=0;a<2;a++)
	{
		for(b=0;b<32;b++)
		{   		
			for(c=0;c<8;c++)
			{	
				LCD_WriteOrder(ORDER_EXPANDI);				
			    LCD_WriteOrder(lcd_y+b);
			    LCD_WriteOrder(lcd_x+c);
			    LCD_WriteData(pPictureArray[d++]);
			    LCD_WriteData(pPictureArray[d++]);
			}
		}
		lcd_x = 0x88;
	}
    LCD_WriteOrder(ORDER_GDRAMOPEN);
	LCD_WriteOrder(ORDER_BASICI);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ��ʾͼƬ���ִ�
///
/// @param[in]  pPictureArrayλͼȡģ�����׵�ַ
/// @param[in]  x �кţ�0~3
/// @param[in]  y �кţ�0~7
/// @param[in]  n ��ʾ�ֵĸ���
/// @return     �� 
/// @note       ��LCD��ͬʱ��ʾͼƬ���ִ���x��y���ִ�����ʼλ�ã�ע���ֺ�ͼƬ��Ҫ�ص�
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_PictureString(uint8_t *pPictureArray,uint8_t x,uint8_t y,uint8_t *string,uint8_t start,uint8_t len)
{
	LCD_Clean();
	LCD_ClearDGRAM();
	LCD_LoadPicture(pPictureArray);
	LCD_DisplayStringXY(x,y,string,start,len);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ����
///
/// @param[in]  X ���x���꣺0~127
/// @param[in]  Y ���y���꣺0~63
/// @return     �� 
/// @note       
///
//////////////////////////////////////////////////////////////////////////////////
uint8_t LCD_H,LCD_L;
void LCD_DrawPoint(uint8_t x, uint8_t y)
{
	uint8_t x_byte, x_bit;
	uint8_t y_byte, y_bit;
	
	x &= 0x7f;
	y &= 0x3f;
	x_byte = x/16;
	x_bit = x%16;
	y_byte = y/32;
	y_bit = y%32;
	
    LCD_WriteOrder(ORDER_EXPANDI);
	LCD_WriteOrder(0x80+y_bit);
	LCD_WriteOrder(0x80+x_byte+8*y_byte);
	
	LCD_WriteOrder(ORDER_BASICI);
	LCD_ReadData();
	LCD_H = LCD_ReadData();
	LCD_L = LCD_ReadData();
	
	LCD_WriteOrder(ORDER_EXPANDI);
	LCD_WriteOrder(0x80+y_bit);
	LCD_WriteOrder(0x80+x_byte+8*y_byte);
	
	LCD_WriteOrder(ORDER_BASICI);
	if(x_bit < 8)
	{
		LCD_WriteData(LCD_H | (0x01 << (7-x_bit)));
		LCD_WriteData(LCD_L);
	}
	else
	{
		LCD_WriteData(LCD_H);
		LCD_WriteData(LCD_L | (0x01 << (15-x_bit)));
	}
    LCD_WriteOrder(ORDER_GDRAMOPEN);
	LCD_WriteOrder(ORDER_BASICI);
}



// void LCD_DrawPoint(uint8_t x, uint8_t y)
// {
// 	uint8_t x_Dyte,x_byte;//�����е�ַ���ֽ�λ�����ֽ��е���һλ
// 	uint8_t y_Dyte,y_byte;//������������0,1�����е�ַ��0~31)
// 	uint8_t GDRAM_hbit,GDRAM_lbit;
// 	LCD_WriteOrder(0x36);//��չָ�
// 	//---XY ���껥��������ͨ�� X,Y ����---
// 	x_Dyte=x/16;//������ 16 ���ֽ��е���һ��
// 	x_byte=x&0x0f;//�����ڸ��ֽ��е� ��һλ
// 	y_Dyte=y/32;//0 Ϊ�ϰ�����1 Ϊ�°���
// 	y_byte=y&0x1f;//������ 0~31 �е���һ��
// 	LCD_WriteOrder(0x80+y_byte);//�趨�е�ַ��y ���꣩������ֱ����
// 	LCD_WriteOrder(0x80+x_Dyte+8*y_Dyte);// �� �� �� �� ַ �� x �� �� �� �� �� ͨ ��	8*y_Dyte ѡ������������ˮƽ��ַ
// 	LCD_ReadData();//Ԥ��ȡ����
// 	GDRAM_hbit=LCD_ReadData();//��ȡ��ǰ��ʾ�߰�λ����
// 	GDRAM_lbit=LCD_ReadData();//��ȡ��ǰ��ʾ�ڰ�λ����
// 	LCD_Delay(LCD_DELAY);
// 	LCD_WriteOrder(0x80+y_byte);//�趨�е�ַ(y ����)
// 	LCD_WriteOrder(0x80+x_Dyte+8*y_Dyte);// �� �� �� �� ַ �� x �� �� �� �� �� ͨ ��	8*y_Dyte ѡ������������ˮƽ��ַ
// 	LCD_Delay(LCD_DELAY);
// 	if(x_byte<8)//�ж����ڸ߰�λ���ǵͰ�λ
// 	{
// 		
// 		LCD_WriteData(GDRAM_hbit|(0x01<<(7-x_byte)));//��λ GDRAM ����	��λ��������Ӧ�ĵ�
// 		
// 		LCD_WriteData(GDRAM_lbit);//��ʾ GDRAM ���Ͱ�λ����
// 	}
// 	else
// 	{
// 		LCD_WriteData(GDRAM_hbit);
// 		
// 		LCD_WriteData(GDRAM_lbit|(0x01<<(15-x_byte)));//��λ GDRAM ����	��λ��������Ӧ�ĵ�
// 		
// 	}
// 	LCD_WriteOrder(0x30);//�ָ�������ָ�

// }



//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ����
///
/// @param[in]  startX �ߵ���ʼ��x���꣺0~127
/// @param[in]  startY �ߵ���ʼ��y���꣺0~63
/// @param[in]  endX   �ߵ��յ�x���꣺0~127
/// @param[in]  endY   �ߵ��յ�y���꣺0~63
/// @return     �� 
/// @note       
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_DrawLine(uint8_t start_X,uint8_t start_Y,uint8_t end_X,uint8_t end_Y)
{
	uint8_t i,b,temp,k;
	if(start_X == end_X)
	{
		if(start_Y > end_Y)
		{
			temp = start_Y;
			start_Y = end_Y;
			end_Y = temp;
		}
	  for(i=0;i<(end_Y-start_Y+1);i++)
	  {
		  LCD_DrawPoint(start_X, start_Y + i);
	  }
	}
	if(start_Y == end_Y)
	{
		if(start_X > end_X)
		{
			temp = start_X;
			start_X = end_X;
			end_X = temp;
		}
		for(i=0;i<(end_X-start_X+1);i++)
		{
			LCD_DrawPoint(start_X + i, start_Y);
		}
	}
	if((start_X > end_X) && (start_Y > end_Y))
	{
		temp = start_X;
		start_X = end_X;
		end_X = temp;
		temp = start_Y;
		start_Y = end_Y;
		end_Y = temp;
	}
	if((start_X < end_X) && (start_Y < end_Y))
	{
		k = (end_Y - start_Y)*100/(end_X - start_X);
		b = start_Y - k*start_X/100;
		if(k < 100)
		{
			for(i=0;i<(end_X - start_X +1);i++)
			{
				LCD_DrawPoint(start_X + i, k*(start_Y+i)/100+b);
			}
		}
		else
		{
			for(i=0;i<(end_Y - start_Y + 1);i++)
			{
				LCD_DrawPoint((start_Y + i -b)*100/k, start_Y+i);
			}
		}
	}
	if((start_X > end_X) && (start_Y < end_Y))
	{
		temp = start_X;
		start_X = end_X;
		end_X = temp;
		temp = start_Y;
		start_Y = end_Y;
		end_Y = temp;
	}
	if((start_X < end_X) && (start_Y > end_Y))
	{
		k = (start_Y - end_Y)*100/(end_X - start_X);
		b = start_Y + k*start_X/100;
		if(k<100)
		{
			for(i=0;i<(end_X - start_X + 1);i++)
			{
				LCD_DrawPoint(start_X + i , b - k*(start_X+i)/100);
			}
		}
		else
		{
			for(i=0;i<(start_Y - end_Y + 1);i++)
			LCD_DrawPoint((b - start_Y + i)*100/k, start_Y - i);
		}
	}
}

	
//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    ������
///
/// @param[in]  startX �������Ͻ�x���꣺0~127
/// @param[in]  startY �������Ͻ�Y���꣺0~63
/// @param[in]  endX   �������½�x���꣺0~127
/// @param[in]  endY   �������½�y���꣺0~63
/// @return     �� 
/// @note       
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_DrawRect(uint8_t start_X,uint8_t start_Y,uint8_t end_X,uint8_t end_Y)
{
	LCD_DrawLine(start_X,start_Y,end_X,start_Y);
	LCD_DrawLine(end_X,start_Y,end_X,end_Y);
	LCD_DrawLine(start_X,start_Y,start_X,end_Y);
	LCD_DrawLine(start_X,end_Y,end_X,end_Y);
}
uint8_t teststring[]={"��������ѧ"};
uint8_t tab1[]={"��ϵ������ģ��������λ�÷�����ʾ�ö����ֿ⣬����ʹ�ø���������"};
uint8_t tab5[]={
/*--  ������һ��ͼ��F:\��\��ͼ\COCK.bmp  --*/
/*--  ���x�߶�=128x64  --*/
0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,
0x00,0x00,0x00,0x00,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0x00,0x40,
0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0x00,0x40,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x14,0x00,0x00,0x00,0x01,0xC2,0x40,
0x00,0x00,0x00,0x06,0x30,0x01,0xE0,0x00,0x00,0x2A,0x00,0x00,0xD8,0x01,0xC6,0x40,
0x00,0x00,0x00,0x0F,0x7B,0x63,0xE0,0x00,0x00,0x22,0x00,0x01,0x24,0x00,0x04,0x40,
0x00,0x00,0x00,0x0F,0xFB,0x63,0x07,0x34,0x00,0x14,0x00,0x01,0x04,0x00,0x08,0xF8,
0x00,0x00,0x00,0x0D,0xDB,0x63,0x01,0xBC,0x00,0x08,0x00,0x00,0x88,0x00,0x1F,0x40,
0x00,0x04,0x00,0x0C,0x1B,0x63,0x07,0xB0,0x00,0x00,0x00,0x00,0x50,0x00,0x30,0x40,
0x00,0x06,0x00,0x0C,0x1B,0x63,0xED,0xB0,0xDB,0x00,0x01,0x00,0x20,0x00,0x00,0x40,
0x00,0x02,0x00,0x0C,0x19,0xE1,0xE7,0xB0,0xDB,0x00,0x02,0x00,0x00,0x00,0x00,0x40,
0x00,0x02,0x00,0x0C,0x19,0xE1,0xE7,0xB0,0xDB,0x00,0x02,0x00,0x00,0x00,0x00,0x40,
0x00,0x02,0x00,0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x42,
0x00,0x02,0x00,0x00,0x03,0xC0,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x03,0xFF,0xFE,
0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x05,0x00,0x00,0x00,0x40,
0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x0A,0x80,0x00,0x00,0x40,
0x00,0x02,0x20,0x00,0x00,0x0A,0x00,0x00,0xFF,0xF8,0xE0,0x05,0x00,0x00,0x00,0x40,
0x00,0x82,0x20,0x06,0xC0,0x15,0x00,0x1F,0xFF,0xFF,0xC0,0x02,0x00,0x00,0x00,0x40,
0x00,0x82,0x30,0x09,0x20,0x11,0x00,0xFB,0xFF,0xE1,0xF8,0x00,0x00,0x00,0x00,0x40,
0x01,0x82,0x10,0x08,0x20,0x0A,0x03,0x9F,0x00,0x9E,0x3E,0x00,0x00,0x00,0x00,0x40,
0x01,0x02,0x10,0x04,0x40,0x04,0x0E,0x70,0x00,0x81,0xC7,0x80,0x01,0x40,0x00,0x40,
0x01,0x02,0x08,0x02,0x80,0x00,0x1D,0x80,0x00,0xE0,0x61,0xE0,0x02,0xA0,0x00,0x40,
0x02,0x02,0x08,0x01,0x00,0x00,0x77,0x9F,0xFC,0xF0,0x18,0xF8,0x02,0x20,0x00,0x40,
0x06,0x02,0x08,0x00,0x00,0x01,0xDF,0x00,0x00,0xF3,0x0C,0x3C,0x01,0x40,0x00,0x00,
0x0C,0x06,0x0C,0x00,0x00,0x03,0x9E,0x00,0x00,0xF8,0x06,0x1E,0x00,0x80,0x00,0x00,
0x00,0x44,0x04,0x00,0x00,0x07,0x0E,0x30,0x01,0xFC,0x7F,0x07,0x00,0x00,0x00,0x00,
0x00,0x6C,0x06,0x00,0x00,0x07,0x0E,0x30,0x01,0xFC,0x7F,0x07,0x00,0x00,0x00,0x00,
0x00,0x38,0x03,0x00,0x00,0x7E,0xFF,0xFF,0xFF,0xFF,0xFF,0x83,0xC0,0x00,0x00,0x00,
0x00,0x18,0x00,0x00,0x0F,0xFD,0xFF,0xFF,0xFF,0xFF,0xFC,0x01,0xF0,0x00,0x78,0x00,
0x00,0x00,0x00,0x00,0x3F,0x81,0xFF,0x00,0x00,0x00,0x00,0x1F,0xF0,0x00,0x48,0x00,
0x00,0x00,0x00,0x00,0xFE,0xF0,0x00,0x00,0x01,0x00,0x00,0xC0,0x1C,0x00,0x44,0x00,
0x00,0x03,0x00,0x03,0xC0,0x07,0x80,0x00,0x03,0xE2,0x07,0x00,0x1C,0x00,0x46,0x00,
0x00,0x06,0x00,0x03,0x80,0x03,0x40,0x00,0x03,0xE0,0x0C,0x00,0x0E,0x00,0x42,0x00,
0x00,0x1C,0x00,0x07,0x00,0x00,0xC0,0x00,0x00,0x20,0x18,0x00,0x07,0x00,0x42,0x00,
0x00,0xF0,0x00,0x09,0x01,0x80,0x60,0x00,0x00,0x20,0x73,0x9F,0x03,0x80,0x42,0x00,
0x00,0x10,0x00,0x1E,0x0F,0xF2,0x20,0x00,0x00,0x20,0x67,0xFF,0xC1,0xC0,0x46,0x00,
0x00,0x10,0x00,0x1E,0x1F,0xF9,0x30,0x00,0x00,0x20,0xEF,0xFF,0xE0,0xE0,0x7C,0x00,
0x03,0xFC,0x00,0x38,0x3F,0xFC,0x90,0x00,0x00,0x20,0xCB,0xFF,0xF9,0xF0,0xFF,0x00,
0x00,0x10,0x00,0x7F,0x7F,0xFE,0x10,0x00,0x00,0x20,0xDB,0xFF,0xFF,0xF0,0x41,0xC0,
0x00,0x10,0x00,0x6E,0xF9,0xBF,0x10,0x00,0x00,0x20,0xF7,0xED,0xFF,0xF0,0x40,0x60,
0x00,0x11,0x00,0x7C,0xFC,0x3F,0x10,0x00,0x00,0x20,0x67,0xE1,0xFD,0xE0,0x40,0x30,
0x03,0xFF,0x00,0x7C,0xFC,0x3F,0x10,0x00,0x00,0x20,0x67,0xE1,0xFD,0xE0,0xC0,0x10,
0x00,0x10,0x00,0x38,0xFC,0x3F,0x18,0x00,0x00,0x20,0x87,0xE0,0xFD,0xC0,0x80,0x08,
0x00,0x20,0x00,0x38,0xF1,0x8F,0x18,0x00,0x1F,0xE7,0x87,0x9C,0x7D,0xC0,0x80,0x08,
0x00,0x60,0x00,0x1C,0xFE,0x1F,0x81,0xFF,0xFF,0xC0,0x0F,0xE0,0xFF,0x80,0x80,0x04,
0x00,0x40,0x00,0x0C,0xFC,0x3F,0x80,0x00,0x00,0x00,0x0F,0xE1,0xF8,0x00,0x80,0x04,
0x01,0x80,0x00,0x00,0xF9,0xBF,0xE0,0x00,0x00,0x00,0x03,0xEC,0xF6,0x01,0x80,0x0C,
0x01,0xC3,0x00,0x00,0xF9,0xBF,0x00,0x00,0x00,0x00,0x00,0xFF,0xCC,0x01,0x80,0x18,
0x00,0x3F,0x80,0x00,0x7F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x00,0x81,0xF0,
0x00,0x00,0xC0,0x00,0x07,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,
0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    LCD����Һ����ʾģ���ʼ��
/// 
/// @param[in]  ��
/// @return     �� 
/// @note       ��ʼ����ɺ�������LCD������ž�Ϊ���ٸ����ģʽ����ʼֵΪ0��
///             �����Ҫ�ı��ʼʱIO����״̬������LCD.c�ļ�LCD_Pin_Init�������޸�
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_Init(void)
{
	LCD_Pin_Init();
	LCD_Clean();
	LCD_DisplayOn();
	LCD_CursorOff();
	//LCD_LoadPicture(tab5);
	//LCD_PictureString(tab5,3,5,teststring,2,4);
	//LCD_DisplayStringXY(0,0,tab1,0,32);
	//LCD_DrawRect(0,0,127,63);
	//LCD_DrawLine(0,0,127,63);
	//LCD_CleanLine(3);
}

