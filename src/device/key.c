
///////////////////////////////////////////////////////////////////////////
/// @file  key.c
/// @brief 按键组件源程序
/// 
/// 按键组件源程序包括按键初始化，按键扫描等功能
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////
/// includes
/////////////////////////////////////////////////
#include "chip.h"
#include "key.h"
#include "Message.h"
#include "rs485.h"

/////////////////////////////////////////////////
/// 变量定义
/////////////////////////////////////////////////
static uint8_t ScanValueSave = Key_Ini_Sta;    //第一次按键默认储存值
static uint8_t ScanCounter = 0;            //按键按下节拍计数器
static uint8_t DoubleHitCounter = 0;       //防止连续双击误动节拍计数器

/////////////////////////////////////////////////////////////////////
/// 函数声明
////////////////////////////////////////////////////////////////////
static uint8_t RemapKey( uint8_t );           //短按键映射
static uint8_t ScanPin( void );               //扫描键值

#ifdef Identify_Long_Click
static uint8_t RemapLongKey( uint8_t );       //长按键映射
#endif

/************************************************************************************************
** Function name:     Key_Scan(void)
** Descriptions:      扫描按键，按键按下后采用消息传递按键信息
**										支持长短按，默认只支持短按，长按在头文件内配置
** input parameters:  无				
** output parameters: 无 
** Returned value:    无
*************************************************************************************************/
 void Key_Scan(void)
{
     uint8_t scanValue;

     scanValue = ScanPin();                     //读取按键值

    if (scanValue == Key_Ini_Sta)              //判断是否释放按键
    {
        if (DoubleHitCounter)                  //防止双击计数器,释放后必须过多久才可以继续检测按键
        {
            DoubleHitCounter--;
            ScanCounter = 0;
            ScanValueSave = Key_Ini_Sta;
            return ;
        }

        if (ScanValueSave == Key_Ini_Sta)           //判断按键值是否有效，无效
        {
            ScanCounter = 0;
            return ;
        }
#ifdef Identify_Long_Click
				
        if (ScanCounter == LongInterval)        //判断为长按键，发送长按键消息
        {           
			PostMessage( MSG_TYPE_KEY, MSG_PRIORITY_LOW, RemapLongKey(ScanValueSave) );      
        }
        else if (ScanCounter > ShortInterval)   //判断为短按键，发送短按键消息
        {
            PostMessage( MSG_TYPE_KEY, MSG_PRIORITY_LOW, RemapKey(ScanValueSave) );
        }
				
#else
				
	      if (ScanCounter > ShortInterval)        //判断为短按键，发送短按键消息
        {
            PostMessage( MSG_TYPE_KEY, MSG_PRIORITY_LOW, RemapKey(ScanValueSave) );			
        }		
				
#endif
				
        ScanCounter = 0;
        ScanValueSave = Key_Ini_Sta;
        DoubleHitCounter = DoubleHitInterval;   //设定防止双击计数器
    }
    else                                        //检测到按键值
    {
        ScanCounter++;

        if (ScanCounter == 1)                   //存储第一次按键值
        {
          ScanValueSave = scanValue;
        }
        else if (ScanCounter == 2)              //判断按键值是否有效
        {
		if (scanValue != ScanValueSave)
            {
                ScanValueSave = Key_Ini_Sta;
                ScanCounter = 0;
            }
        }
				else if (ScanCounter == 4)              //组合按键时2个按键不同时按下，以最后的键值为准
        {
        if (scanValue != ScanValueSave)
            {
                ScanValueSave = scanValue;
            }
        }
        else if (ScanCounter >= LongInterval)   //防止计数值溢出循环
        {
            ScanCounter = LongInterval;
        }
    }
}


/************************************************************************************************
** Function name:     Key_Init
** Descriptions:      按键初始化设置  
**										
** input parameters:  无				
** output parameters: 无 
** Returned value:    无
*************************************************************************************************/
void Key_Init( void )
{	
		 Chip_GPIO_Init(LPC_GPIO);
#ifdef	KEY_A_PIN
		 Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_2, (IOCON_FUNC1 | IOCON_MODE_INACT | MD_DIGMODE));	
		 Chip_GPIO_SetPinDIRInput(LPC_GPIO, KEY_A_PORT, KEY_A_PIN);
#endif
	
#ifdef	KEY_B_PIN
		 Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_1, (IOCON_FUNC1 | IOCON_MODE_INACT | MD_DIGMODE));		  						
		 Chip_GPIO_SetPinDIRInput(LPC_GPIO, KEY_B_PORT, KEY_B_PIN);
#endif	

#ifdef	KEY_C_PIN	  
		 Chip_GPIO_SetPinDIRInput(LPC_GPIO, KEY_C_PORT, KEY_C_PIN);
#endif	

#ifdef  KEY_D_PIN
		 Chip_GPIO_SetPinDIRInput(LPC_GPIO, KEY_D_PORT, KEY_D_PIN);
#endif		 
	
}


/************************************************************************************************
** Function name:     RemapKey(uint8_t scanValue)
** Descriptions:      短按按键映射
**										
** input parameters:  scanValue按键扫描值				
** output parameters: 无 
** Returned value:    短按键映射值
*************************************************************************************************/
static uint8_t RemapKey(uint8_t scanValue)
{
	uint8_t scanValue_temp ;
	
#if (Key_Invalid == 0)							//初始按键状态为低电平
		scanValue_temp = scanValue ;
#elif (Key_Invalid == 1)						//初始按键状态为高电平
		scanValue_temp = ~scanValue ;
#endif
    switch(scanValue_temp)
    {   
        case 0x01:
            return(Key_Short_D);
        case 0x02:
            return(Key_Short_C);
        case 0x03:
            return(Key_Short_CD);
        case 0x04:
            return(Key_Short_B);
        case 0x05:
            return(Key_Short_BD);
        case 0x06:
            return(Key_Short_BC);
        case 0x08:
            return(Key_Short_A);
        case 0x09:
            return(Key_Short_AD);
        case 0x0a:
            return(Key_Short_AC);
        case 0x0c:
            return(Key_Short_AB);      
        default:
            return(Key_Ini_Sta);
    }
}


/************************************************************************************************
** Function name:     RemapLongKey(uint8_t scanValue)
** Descriptions:      长按按键映射
**										
** input parameters:  scanValue按键扫描值				
** output parameters: 无 
** Returned value:    长按键映射值
*************************************************************************************************/
#ifdef Identify_Long_Click
static uint8_t RemapLongKey(uint8_t scanValue)
{
	uint8_t scanValue_temp ;
	
#if (Key_Invalid == 0)					//初始按键状态为低电平
		scanValue_temp = scanValue ;
#elif (Key_Invalid == 1)						//初始按键状态为高电平
		scanValue_temp = ~scanValue ;
#endif
    switch(scanValue_temp)
    {
        case 0x01:
            return(Key_Long_D);
        case 0x02:
            return(Key_Long_C);
        case 0x03:
            return(Key_Long_CD);
        case 0x04:
            return(Key_Long_B);
        case 0x05:
            return(Key_Long_BD);
        case 0x06:
            return(Key_Long_BC);
        case 0x08:
            return(Key_Long_A);
        case 0x09:
            return(Key_Long_AD);
        case 0x0a:
            return(Key_Long_AC);
        case 0x0c:
            return(Key_Long_AB);      
        default:
            return(Key_Ini_Sta);
    }
}
#endif


/************************************************************************************************
** Function name:     ScanPin(void)
** Descriptions:      扫描按键，返回按键扫描值。
**										4个按键高低电平的逻辑值
**           				  按照ABCD的顺序分别存放到返回值的第3位、第2位、第1位和第0位
**            				即存放至返回值的低四位中。
** input parameters:  无 		
** output parameters: 无 
** Returned value:    按键扫描值 scanValue
*************************************************************************************************/
 static uint8_t ScanPin(void)
{
      uint8_t scanValue;                             //扫描到的键值
	  uint8_t read_A_pinValue = 1;
	  uint8_t read_B_pinValue = 1;
	  uint8_t read_C_pinValue = 1;				 //相当于未按键
	  uint8_t read_D_pinValue = 1;				 //相当于未按键
	
#ifdef	KEY_A_PIN
        read_A_pinValue = Chip_GPIO_GetPinState(LPC_GPIO, KEY_A_PORT, KEY_A_PIN);
#endif

#ifdef	KEY_B_PIN
	    read_B_pinValue = Chip_GPIO_GetPinState(LPC_GPIO, KEY_B_PORT, KEY_B_PIN);
#endif


#ifdef	KEY_C_PIN	
	    read_C_pinValue = Chip_GPIO_GetPinState(LPC_GPIO, KEY_C_PORT, KEY_C_PIN);
#endif

#ifdef	KEY_D_PIN		
	    read_D_pinValue = Chip_GPIO_GetPinState(LPC_GPIO, KEY_D_PORT, KEY_D_PIN);
#endif
	  scanValue = (Key_Ini_Sta<<4) | (read_A_pinValue <<3) | (read_B_pinValue <<2) | (read_C_pinValue<<1) | read_D_pinValue;
      return(scanValue);       
}
