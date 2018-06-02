//#include "adc.h"
#include "board.h"
#include "cari_config.h"
//#include "led.h"
//#include "Message.h"

uint16_t ADC_IRQ_Value_final,ADC_add=0;//ADC_IRQ_Value_final：AD最终数值，ADC_add：5次之和
uint8_t  ADC_Comp_flag=0;//用于去除上电第一次和第二次采样值，这两个值不准

/**************************************************************************/
/**函数名：ADCInit()                                                ******/
/**入口参数：无                                                    ******/
/**函数功能：实现ADC采样的初始化                                 ******/
/**调用情况：在主函数中被调用                                   ******/
/*************************************************************************/
void ADCInit( )
{
	
	/*ADC模块上电和时钟允许*/
	(*((uint32_t *)0x40048238)) &= ~(0x1<<4);//ADC模块上电， 寄存器PDRUNCFG，ADC模块默认是掉电的	
  (*((uint32_t *)0x40048080)) |= (1<<13);//允许ADC时钟，寄存器SYSAHBCLKCTRL
	/*IO配置模块时钟允许，将管脚配置为AD7*/
	(*((uint32_t *)0x40048080)) |= (1<<16);//允许IO配置模块的时钟，寄存器SYSAHBCLKCTRL
	(*((uint32_t *)0x40044098)) &= 0x60;//IOCON_PIO1_11寄存器配置，FUNC、MODE、ADMODE与0，其他位不变，注意：保留位bit6不能操作，否则会引起AD不准
	(*((uint32_t *)0x40044098)) |= 0x01;//IOCON_PIO1_11寄存器配置，将FUNC置为1,表示该管脚接入AD7
	/*选择AD7*/
	(*((uint32_t *)0x4001C000)) &= ~0xff;//将AD0CR，bit7-bit0清零
	(*((uint32_t *)0x4001C000)) |= (1<<7);//选择AD7，其他通道均不选，寄存器AD0CR，bit7-bit0,SEL
	/*配置ADC时钟*/
	(*((uint32_t *)0x4001C000)) &= ~(0xff<<8);//将AD0CR，bit15-bit8清零
	(*((uint32_t *)0x4001C000)) |= ((SystemCoreClock/((*((uint32_t *)0x40048078))&0xff))/187500-1)<<8;//将ADC时钟设为187.5k(48MHz时钟下的最小值)，187.5k=Pclk/(CLKDIV+1),CLKDIV(寄存器AD0CR，bit15-bit8)=Pclk/187.5k-1，Pclk=SystemCoreClock/SYSAHBCLKDIV
	/*配置ADC软硬件控制模式*/
	(*((uint32_t *)0x4001C000)) &= ~(1<<16);//将AD0CR，bit16清零，配置为软件控制模式
	/*配置ADC转换精度*/
	(*((uint32_t *)0x4001C000)) &= ~(7<<17);//将AD0CR，bit19-bit17清零，配置为10bit
	/*配置ADC开始模式*/
	(*((uint32_t *)0x4001C000)) &= ~(7<<24);//将AD0CR，bit26-bit24清零
	(*((uint32_t *)0x4001C000)) |= (1<<24);//将AD0CR，bit24置1，配置为now start
	/*配置ADC中断*/
	(*((uint32_t *)0x4001C00C)) &= ~0x1ff;//寄存器AD0INTEN
	(*((uint32_t *)0x4001C00C)) |= 0x080;//允许AD7产生中断
	/*使能ADC中断*/
	NVIC_EnableIRQ(ADC_IRQn);
}
/**************************************************************************/
/**函数名：ADC_IRQHandler()                                         ******/
/**入口参数：无                                                     ******/
/**函数功能：ADC采用中断方式，ADC在systick中起动                     ******/
/**调用情况：                                                       ******/
/**************************************************************************/
void  ADC_IRQHandler( )
{
	uint32_t ADC_d;
	//Chip_ADC_ReadValue(LPC_ADC, ADC_CH7, &ADC_IRQ_Value);
	ADC_Comp_flag ++;//记录读取AD的次数
	/*读取AD7数值*/
	ADC_d=(*((uint32_t *)0x4001C02C))&(0x3ff<<6);//寄存器AD0DR7
//	if(ADC_Comp_flag>2)//第1次和第2次的数值不准，所以不读
//	{
//		ADC_add += ((uint16_t)(ADC_d>>6));
//		if(ADC_Comp_flag==7)//读5次取平均
//		{
//			ADC_Comp_flag = 2;//返回计数为第一次			
//			ADC_IRQ_Value_final = (uint16_t)ADC_add/5;//5次平均
//			ADC_add = 0;//将累加值清零
//			#if DEBUG_UART_C
//			DEBUGOUT("ADC:%x\r\n",ADC_IRQ_Value_final);
//			#endif
//		}
//	}
	ADC_IRQ_Value_final=((uint16_t)(ADC_d>>6));
	(*((uint32_t *)0x4001C000)) |= (1<<24);//起动AD转换
//在Systick中断中起动AD转换
}

