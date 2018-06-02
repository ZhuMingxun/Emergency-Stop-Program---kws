#include "watchDog.h"
/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Comment this define to let the watchdog timeout. In this case, the board
   will continuously drop via to the WDT warning. */

bool watchdog_feed_flag;
/**
 * @brief	Watchdog Timer Interrupt Handler
 * @return	Nothing
 * @note	Handles watchdog timer warning and timeout events
 */



void watchdog_INIT(int feedTime)
{
	uint32_t wdtFreq;
/* Initialize WWDT (also enables WWDT clock) */
	Chip_WWDT_Init(LPC_WWDT);

	/* Prior to initializing the watchdog driver, the clocking for the
	   watchdog must be enabled. This example uses the watchdog oscillator
	   set at a 500KHz (1Mhz / 2) clock rate. */
	Chip_SYSCTL_PowerUp(SYSCTL_POWERDOWN_WDTOSC_PD);
	Chip_Clock_SetWDTOSC(WDTLFO_OSC_1_05, 2);

	/* The WDT divides the input frequency into it by 2 */
	wdtFreq = Chip_Clock_GetWDTOSCRate() / 8;	
	Chip_Clock_SetWDTClockSource(SYSCTL_WDTCLKSRC_WDTOSC, 1);

	/* Set watchdog feed time constant to approximately 2s
	   Set watchdog warning time to 512 ticks after feed time constant
	   Set watchdog window time to 3s */
	Chip_WWDT_SetTimeOut(LPC_WWDT, wdtFreq *feedTime);	
	Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET);
	
	/* Clear watchdog warning and timeout interrupts */

	Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF);

//	/* Clear and enable watchdog interrupt */

	/* Start watchdog */
	Chip_WWDT_Start(LPC_WWDT);
}
void watchdog_feedDog(void)
{	
	Chip_WWDT_Feed(LPC_WWDT);
	
}
void watchdog_disable(void)
{
	LPC_WWDT->MOD&=(~WWDT_WDMOD_WDEN);
}
void watchdog_enable(void)
{
	/* Start watchdog */
	Chip_WWDT_Start(LPC_WWDT);
}
