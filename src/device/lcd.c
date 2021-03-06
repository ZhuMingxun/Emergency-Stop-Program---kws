///////////////////////////////////////////////////////////////////////////
/// @file LCD.h
/// @brief LCD12864显示的头文件
/// 
/// LCD显示模块提供了一个与12864点阵液晶的接口
/// 要使用这个驱动,你必须按照如下步骤操作：
/// 1) 根据处理器修改包含的头文件(位于LCD.h)；
/// 2) 根据硬件设计进行配置(位于LCD.h)：
///    LCD_X_PORT-------------与LCD相连的单片机的端口；
///    LCD_X------------------与LCD相连的单片具体的引脚号；
///	3) 用户使用LCD时，需要先对LCD进行初始化LCD_Init()(位于LCD.c)；
/// 4) 初始化完成后需要开启显示功能

///////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "lcd.h"

void LCD_WriteOrder(uint8_t order);




//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    软件延时
/// 
/// @param[in]  delay：延时时间
/// @return     无 
/// @note       可在LCD.h文件中的LCD_DELAY进行修改
//////////////////////////////////////////////////////////////////////////////////
void LCD_Delay(uint32_t delay)
{
    uint32_t i;
    for (i = 0; i < delay; i++);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    与LCD相连的引脚初始化
/// 
/// @param[in]  GPIOx ：端口号
/// @param[in]  GPIO_Pin ：引脚号
/// @return     无 
/// @note       初始化完成后，所有与LCD相关引脚均为低速高输出模式，初始值为0，
//////////////////////////////////////////////////////////////////////////////////
void LCD_Pin_Init()
{
	Chip_GPIO_SetPortDIROutput(LPC_GPIO, 2, 0xfff);//全部配置为输出

	Chip_GPIO_SetPortOutLow(LPC_GPIO, 2, 0xfff);   //全部输出为0
}



//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    打开液晶显示
///  
/// @param[in]  无
/// @return     无
/// @note 
//////////////////////////////////////////////////////////////////////////////////
void LCD_DisplayOn(void)
{
  	LCD_WriteOrder(ORDER_BASICI);    //基本指令
	
	LCD_WriteOrder(ORDER_DISPON);	//开启显示指令
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    关闭液晶显示，不能关背光
///  
/// @param[in]  无
/// @return     无
/// @note 
//////////////////////////////////////////////////////////////////////////////////
void LCD_DisplayOff(void)
{
    LCD_WriteOrder(ORDER_BASICI);	 //基本指令
	
	LCD_WriteOrder(ORDER_DISPOFF);	 //关闭显示指令
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    等待LCD空闲
/// 
/// @param[in]  无
/// @return     无 
/// @note       判断LCD是否忙状态，等待LCD空闲
//////////////////////////////////////////////////////////////////////////////////
void LCD_ReadBusy(void)
{
	uint8_t busyread;
	
	LCD_EN_PIN_DISENABLE;
	
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, LCD_PORT, LCD_DB7);   //将DB7设为输入
	
	Chip_GPIO_SetPinOutLow(LPC_GPIO, LCD_PORT, LCD_RS);		//RS = 1,指令操作
	
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, LCD_PORT, LCD_RW);    //RW = 1，读操作
	
	LCD_EN_PIN_ENABLE;										//EN = 1
	

  	busyread = Chip_GPIO_GetPinState(LPC_GPIO, LCD_PORT, LCD_DB7); //读DB7
	
	while(busyread)                                                //等待DB7 = 0（空闲）
	{
		busyread = Chip_GPIO_GetPinState(LPC_GPIO, LCD_PORT, LCD_DB7);
	}
	
	LCD_EN_PIN_DISENABLE;										//EN = 0
	
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LCD_PORT, LCD_DB7);		//DB7重新设为输出
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    读取LCD光标所在位置
/// 
/// @param[in]  无
/// @return     readac 光标所在位置
/// @note       
//////////////////////////////////////////////////////////////////////////////////
uint8_t LCD_ReadAc(void)
{
	uint8_t readac=0x00;
	
	Chip_GPIO_SetPortDIRInput(LPC_GPIO,LCD_PORT,0x07f);		//DB0-DB6设为输入
	
	Chip_GPIO_SetPinOutLow(LPC_GPIO, LCD_PORT, LCD_RS);		//RS = 0，数据操作
	
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, LCD_PORT, LCD_RW);	//RW = 0,读操作
	
	LCD_EN_PIN_ENABLE;										//EN = 1,
	
	LCD_Delay(LCD_DELAY);
	
	readac = Chip_GPIO_GetPortValue(LPC_GPIO,LCD_PORT);		//读取DB0-DB7
	
	readac &= 0x7f;
	
	LCD_EN_PIN_DISENABLE;

	Chip_GPIO_SetPortDIROutput(LPC_GPIO, 2, 0xfff);//全部配置为输出
	
	return readac;
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    并行LCD读数据
/// 
/// @param[in]  无
/// @return     readdata：读出的数据
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
	
	Chip_GPIO_SetPortDIROutput(LPC_GPIO, 2, 0xfff);//全部配置为输出
	
	return readdata;
}

//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    并行写指令
/// 
/// @param[in]  order ：LCD指令
/// @return     无 
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
/// @brief 	    并行LCD写数据
/// 
/// @param[in]  data：数据
/// @return     无 
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
/// @brief 	    清除LCD所有显示内容（清屏）
/// 
/// @param[in]  无
/// @return     无 
/// @note       
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_Clean(void)
{
	LCD_ClearDGRAM();
	LCD_WriteOrder(ORDER_CLEAR);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    清除LCD某一行的显示内容
/// 
/// @param[in]  line 行号：0~3
/// @return     无 
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
/// @brief 	    开光标
/// 
/// @param[in]  无
/// @return     无 
/// @note       
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_CursorOn(void)
{
	LCD_WriteOrder(ORDER_CURSORON);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    关光标
/// 
/// @param[in]  无
/// @return     无 
/// @note       
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_CursorOff(void)
{
	LCD_WriteOrder(ORDER_CURSOROFF);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    设置光标位置
/// 
/// @param[in]  line 行号：0~3
/// @param[in]  column 列号 0~7
///
/// @return     无 
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
/// @brief 	    显示一个字符
/// 
/// @param[in]  displaybuff 字符代码，通过LCD对应的数据手册OCMJ4�8C-14.PDF中查找
/// @return     无 
/// @note       一个光标位置可以写入两个字符；
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_DisplayChar(uint8_t displaybuff)
{
	LCD_WriteData(displaybuff);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    显示一个字
/// 
/// @param[in]  displaybuff 字代码，支持BIG5编码和GB编码
/// @return     无 
/// @note       一个光标位置写一个字
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_DisplayWord(uint8_t *displaybuff)
{
	LCD_WriteData(displaybuff[0]);
	LCD_WriteData(displaybuff[1]);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    在某个位置上显示一个字
/// 
/// @param[in]  line 行号：0~3
/// @param[in]  column 列号：0~7
/// @param[in]  displaybuff 字代码，支持BIG5编码和GB编码
/// @return     无 
/// @note       
///
//////////////////////////////////////////////////////////////////////////////////
void LCD_DisplayWordXY(uint8_t line,uint8_t column,uint8_t *displaybuff)
{
	LCD_SetCursor(line,column);
	LCD_DisplayWord(displaybuff);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    显示n个字
/// 
/// @param[in]  string字串首地址
/// @param[in]  start 需要显示字在字串中的位置，整个字串第一个地址定义为0
/// @param[in]  len 需要显示字的个数
/// @return     无 
/// @note       字串中字必须是双字节，如在单字节输入下，英文字母，数字，标点符号
///             特殊字符等，必须是偶数个，如果是单个字符，需用空格补足。
///             eg ＡＢＣａｂｃ为双字节可以直接放入字串中调用，如ABCabc是单字节，
///             必须连续偶数个，不足需用空格补足
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
/// @brief 	    从某个位置开始显示n个字
///
/// @param[in]  line 行号：0~3
/// @param[in]  column 列号：0~7
/// @param[in]  string字串首地址
/// @param[in]  start 需要显示字在字串中的位置，整个字串第一个地址定义为0
/// @param[in]  len 需要显示字的个数
/// @return     无 
/// @note       
//////////////////////////////////////////////////////////////////////////////////
void LCD_DisplayStringXY(uint8_t line,uint8_t column,uint8_t *displaybuff,uint8_t start,uint8_t len)
{
	LCD_SetCursor(line,column);
	LCD_DisplayString(displaybuff,start,len);
}


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    清除位图存储器
///
/// @param[in]  无
/// @return     无 
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
/// @brief 	    显示一幅图片
///
/// @param[in]  pPictureArray位图取模数组首地址
/// @return     无 
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
/// @brief 	    显示图片和字串
///
/// @param[in]  pPictureArray位图取模数组首地址
/// @param[in]  x 行号：0~3
/// @param[in]  y 列号：0~7
/// @param[in]  n 显示字的个数
/// @return     无 
/// @note       在LCD中同时显示图片和字串，x，y是字串的起始位置，注意字和图片不要重叠
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
/// @brief 	    画点
///
/// @param[in]  X 点的x坐标：0~127
/// @param[in]  Y 点点y坐标：0~63
/// @return     无 
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
// 	uint8_t x_Dyte,x_byte;//定义列地址的字节位及在字节中的哪一位
// 	uint8_t y_Dyte,y_byte;//上下两个屏（0,1），行地址（0~31)
// 	uint8_t GDRAM_hbit,GDRAM_lbit;
// 	LCD_WriteOrder(0x36);//扩展指令集
// 	//---XY 坐标互换，即普通的 X,Y 坐标---
// 	x_Dyte=x/16;//计算在 16 个字节中的哪一个
// 	x_byte=x&0x0f;//计算在该字节中的 哪一位
// 	y_Dyte=y/32;//0 为上半屏，1 为下半屏
// 	y_byte=y&0x1f;//计算在 0~31 中的哪一行
// 	LCD_WriteOrder(0x80+y_byte);//设定行地址（y 坐标），即垂直坐标
// 	LCD_WriteOrder(0x80+x_Dyte+8*y_Dyte);// 设 定 列 地 址 （ x 坐 标 ） ， 并 通 过	8*y_Dyte 选定上下屏，即水平地址
// 	LCD_ReadData();//预读取数据
// 	GDRAM_hbit=LCD_ReadData();//读取当前显示高八位数据
// 	GDRAM_lbit=LCD_ReadData();//读取当前显示第八位数据
// 	LCD_Delay(LCD_DELAY);
// 	LCD_WriteOrder(0x80+y_byte);//设定行地址(y 坐标)
// 	LCD_WriteOrder(0x80+x_Dyte+8*y_Dyte);// 设 定 列 地 址 （ x 坐 标 ） ， 并 通 过	8*y_Dyte 选定上下屏，即水平地址
// 	LCD_Delay(LCD_DELAY);
// 	if(x_byte<8)//判断其在高八位还是低八位
// 	{
// 		
// 		LCD_WriteData(GDRAM_hbit|(0x01<<(7-x_byte)));//置位 GDRAM 区高	八位数据中相应的点
// 		
// 		LCD_WriteData(GDRAM_lbit);//显示 GDRAM 区低八位数据
// 	}
// 	else
// 	{
// 		LCD_WriteData(GDRAM_hbit);
// 		
// 		LCD_WriteData(GDRAM_lbit|(0x01<<(15-x_byte)));//置位 GDRAM 区高	八位数据中相应的点
// 		
// 	}
// 	LCD_WriteOrder(0x30);//恢复到基本指令集

// }



//////////////////////////////////////////////////////////////////////////////////
/// @brief 	    画线
///
/// @param[in]  startX 线的起始点x坐标：0~127
/// @param[in]  startY 线的起始点y坐标：0~63
/// @param[in]  endX   线的终点x坐标：0~127
/// @param[in]  endY   线的终点y坐标：0~63
/// @return     无 
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
/// @brief 	    画矩形
///
/// @param[in]  startX 矩形左上角x坐标：0~127
/// @param[in]  startY 矩形左上角Y坐标：0~63
/// @param[in]  endX   矩形右下角x坐标：0~127
/// @param[in]  endY   矩形右下角y坐标：0~63
/// @return     无 
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
uint8_t teststring[]={"安徽理工大学"};
uint8_t tab1[]={"本系列中文模块内任意位置反白显示置二级字库，可在使用更方便更灵活"};
uint8_t tab5[]={
/*--  调入了一幅图像：F:\梁\画图\COCK.bmp  --*/
/*--  宽度x高度=128x64  --*/
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
/// @brief 	    LCD点阵液晶显示模块初始化
/// 
/// @param[in]  无
/// @return     无 
/// @note       初始化完成后，所有与LCD相关引脚均为低速高输出模式，初始值为0，
///             如果需要改变初始时IO引脚状态，需在LCD.c文件LCD_Pin_Init函数中修改
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

