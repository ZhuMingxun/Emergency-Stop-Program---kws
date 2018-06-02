#include "dataio.h"
#include "can.h"
#include "t16b0.h"

uint8_t flag_18V_count1,flag_18V_count2,flag_18V_return;
uint8_t Data0_State = 0;
uint8_t JDQ_State   = 0;
void JDQ_State_Open()  //继电器打开
{
	Chip_GPIO_SetPinState(LPC_GPIO, JDQ_PORT, JDQ_PORT_BIT, 0);
	JDQ_State = 0;	
}

void JDQ_State_Close() //继电器吸合
{
	Chip_GPIO_SetPinState(LPC_GPIO, JDQ_PORT, JDQ_PORT_BIT, 1);
	JDQ_State = 1;	
}

void Gpio_Init()
{

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, JDQ_PORT, JDQ_PORT_BIT);
	JDQ_State_Open();  //默认继电器处于打开状态	
	#if NEW_BS
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, D18V_PORT, D18V_PORT_BIT);
//		(*((uint32_t *)0x50018000))&=~(1<<D18V_PORT_BIT);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, POWEROFF_PORT, POWEROFF_PORT_BIT);
//	(*((uint32_t *)0x50018000))|=(1<<5);
	//BTPWR_IOCON();
//	POWEROFF_State_Close();  //电池供电关闭
	#else
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, DEBUG_LED_PORT, DEBUG_LED_BIT);
	Chip_GPIO_SetPinState(LPC_GPIO, DEBUG_LED_PORT, DEBUG_LED_BIT, 0);
	#endif


	
	//JT_IOCON();
	//PP1_IOCON();
	//PP2_IOCON();
	
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, JT_PORT, JT_PORT_BIT);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, PP2_PORT, PP2_PORT_BIT);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, PP1_PORT, PP1_PORT_BIT);
	
}
uint8_t BSdata()       //数据处理
{
	if(JDQ_State == 0)Data0_State |= 0x08;      //继电器处于Open状态
	else if(JDQ_State == 1)Data0_State &= 0xf7; //继电器处于Close状态
	return Data0_State;
}
#if NEW_BS
//uint8_t D18V_State(void)
//{
//	if(flag_18V==1)
//	{
//		flag_18V=0;
//		if(Chip_GPIO_GetPinState(LPC_GPIO, D18V_PORT, D18V_PORT_BIT)==1)
//		{
//			if(flag_18V_count2>0)flag_18V_count2--;
//			flag_18V_count1++;
//			if(flag_18V_count1==100)
//			{
//				flag_18V_count1=0;
//				flag_18V_return=1;
//			}
//		}
//		else if(Chip_GPIO_GetPinState(LPC_GPIO, D18V_PORT, D18V_PORT_BIT)==0)
//		{
//			if(flag_18V_count1>0)flag_18V_count1--;
//			flag_18V_count2++;
//			if(flag_18V_count2==100)
//			{
//				flag_18V_count2=0;
//				flag_18V_return=0;
//			}
//		}
//	}
////		return Chip_GPIO_GetPinState(LPC_GPIO, D18V_PORT, D18V_PORT_BIT);
//	return flag_18V_return;
//}

#define STAT_D18V_ON    0
#define STAT_D18V_OFF   1

uint8_t D18V_State(void)
{
    static uint8_t stat = STAT_D18V_OFF;
    static uint8_t d18v_on_cnt;
    static uint8_t d18v_off_cnt;
    
    if(flag_18V==1)
    {
        flag_18V = 0;
        
        switch(stat)
        {
            case STAT_D18V_ON:/* 18V ON State */
            {
                if(Chip_GPIO_GetPinState(LPC_GPIO, D18V_PORT, D18V_PORT_BIT)==1)/* 18V off detect */
                {
                    if(d18v_off_cnt++ > 2)/* 30ms */
                    {
                       stat =  STAT_D18V_OFF;/* Switch to 18V Off State */
                       d18v_on_cnt = 0;
                    }
                }
                else
                {
                    d18v_off_cnt = 0;   
                }
                
                break;
            }
            
            case STAT_D18V_OFF:/* 18V OFF State */
            {
                if(Chip_GPIO_GetPinState(LPC_GPIO, D18V_PORT, D18V_PORT_BIT)==0)
                {
                    if(d18v_on_cnt++ > 50)/* 510ms */
                    {
                        stat =  STAT_D18V_ON;/* Switch to 18V On State */
                        d18v_off_cnt = 0;
                    }
                    
                }
                else
                {
                    d18v_on_cnt = 0;
                }      
                
                break;
            }
            
            default:break;
        }
    }
 
    return stat;
  
}


void POWEROFF_State_Open()  //电池供电接通
{
	Chip_GPIO_SetPinState(LPC_GPIO, POWEROFF_PORT, POWEROFF_PORT_BIT, 1);
}

void POWEROFF_State_Close() //电池供电关闭
{
	Chip_GPIO_SetPinState(LPC_GPIO, POWEROFF_PORT, POWEROFF_PORT_BIT, 0);
}

#endif
uint8_t JT_State(void)
{
	return Chip_GPIO_GetPinState(LPC_GPIO, JT_PORT, JT_PORT_BIT);
}
