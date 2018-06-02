//#include "adc.h"
#include "board.h"
#include "cari_config.h"
//#include "led.h"
//#include "Message.h"

uint16_t ADC_IRQ_Value_final,ADC_add=0;//ADC_IRQ_Value_final��AD������ֵ��ADC_add��5��֮��
uint8_t  ADC_Comp_flag=0;//����ȥ���ϵ��һ�κ͵ڶ��β���ֵ��������ֵ��׼

/**************************************************************************/
/**��������ADCInit()                                                ******/
/**��ڲ�������                                                    ******/
/**�������ܣ�ʵ��ADC�����ĳ�ʼ��                                 ******/
/**������������������б�����                                   ******/
/*************************************************************************/
void ADCInit( )
{
	
	/*ADCģ���ϵ��ʱ������*/
	(*((uint32_t *)0x40048238)) &= ~(0x1<<4);//ADCģ���ϵ磬 �Ĵ���PDRUNCFG��ADCģ��Ĭ���ǵ����	
  (*((uint32_t *)0x40048080)) |= (1<<13);//����ADCʱ�ӣ��Ĵ���SYSAHBCLKCTRL
	/*IO����ģ��ʱ���������ܽ�����ΪAD7*/
	(*((uint32_t *)0x40048080)) |= (1<<16);//����IO����ģ���ʱ�ӣ��Ĵ���SYSAHBCLKCTRL
	(*((uint32_t *)0x40044098)) &= 0x60;//IOCON_PIO1_11�Ĵ������ã�FUNC��MODE��ADMODE��0������λ���䣬ע�⣺����λbit6���ܲ��������������AD��׼
	(*((uint32_t *)0x40044098)) |= 0x01;//IOCON_PIO1_11�Ĵ������ã���FUNC��Ϊ1,��ʾ�ùܽŽ���AD7
	/*ѡ��AD7*/
	(*((uint32_t *)0x4001C000)) &= ~0xff;//��AD0CR��bit7-bit0����
	(*((uint32_t *)0x4001C000)) |= (1<<7);//ѡ��AD7������ͨ������ѡ���Ĵ���AD0CR��bit7-bit0,SEL
	/*����ADCʱ��*/
	(*((uint32_t *)0x4001C000)) &= ~(0xff<<8);//��AD0CR��bit15-bit8����
	(*((uint32_t *)0x4001C000)) |= ((SystemCoreClock/((*((uint32_t *)0x40048078))&0xff))/187500-1)<<8;//��ADCʱ����Ϊ187.5k(48MHzʱ���µ���Сֵ)��187.5k=Pclk/(CLKDIV+1),CLKDIV(�Ĵ���AD0CR��bit15-bit8)=Pclk/187.5k-1��Pclk=SystemCoreClock/SYSAHBCLKDIV
	/*����ADC��Ӳ������ģʽ*/
	(*((uint32_t *)0x4001C000)) &= ~(1<<16);//��AD0CR��bit16���㣬����Ϊ�������ģʽ
	/*����ADCת������*/
	(*((uint32_t *)0x4001C000)) &= ~(7<<17);//��AD0CR��bit19-bit17���㣬����Ϊ10bit
	/*����ADC��ʼģʽ*/
	(*((uint32_t *)0x4001C000)) &= ~(7<<24);//��AD0CR��bit26-bit24����
	(*((uint32_t *)0x4001C000)) |= (1<<24);//��AD0CR��bit24��1������Ϊnow start
	/*����ADC�ж�*/
	(*((uint32_t *)0x4001C00C)) &= ~0x1ff;//�Ĵ���AD0INTEN
	(*((uint32_t *)0x4001C00C)) |= 0x080;//����AD7�����ж�
	/*ʹ��ADC�ж�*/
	NVIC_EnableIRQ(ADC_IRQn);
}
/**************************************************************************/
/**��������ADC_IRQHandler()                                         ******/
/**��ڲ�������                                                     ******/
/**�������ܣ�ADC�����жϷ�ʽ��ADC��systick����                     ******/
/**���������                                                       ******/
/**************************************************************************/
void  ADC_IRQHandler( )
{
	uint32_t ADC_d;
	//Chip_ADC_ReadValue(LPC_ADC, ADC_CH7, &ADC_IRQ_Value);
	ADC_Comp_flag ++;//��¼��ȡAD�Ĵ���
	/*��ȡAD7��ֵ*/
	ADC_d=(*((uint32_t *)0x4001C02C))&(0x3ff<<6);//�Ĵ���AD0DR7
//	if(ADC_Comp_flag>2)//��1�κ͵�2�ε���ֵ��׼�����Բ���
//	{
//		ADC_add += ((uint16_t)(ADC_d>>6));
//		if(ADC_Comp_flag==7)//��5��ȡƽ��
//		{
//			ADC_Comp_flag = 2;//���ؼ���Ϊ��һ��			
//			ADC_IRQ_Value_final = (uint16_t)ADC_add/5;//5��ƽ��
//			ADC_add = 0;//���ۼ�ֵ����
//			#if DEBUG_UART_C
//			DEBUGOUT("ADC:%x\r\n",ADC_IRQ_Value_final);
//			#endif
//		}
//	}
	ADC_IRQ_Value_final=((uint16_t)(ADC_d>>6));
	(*((uint32_t *)0x4001C000)) |= (1<<24);//��ADת��
//��Systick�ж�����ADת��
}

