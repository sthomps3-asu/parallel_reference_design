/*
 * msdelay.c
 *
 *  Created on: Feb 23, 2016
 *      Author: badal.nilawar
 */
#include "sys_config_mss_clocks.h"
#include "mss_timer/mss_timer.h"

static uint32_t onemscnt;
static uint32_t timerdone;

void msdelay_init()
{
	uint32_t nsval;

	nsval = 1000000000/MSS_SYS_APB_0_CLK_FREQ;
	onemscnt = 1000000/nsval;

	MSS_TIM1_init(MSS_TIMER_ONE_SHOT_MODE);
	MSS_TIM1_clear_irq();
}
void msdelay(uint32_t tms)
{
#if 1
	uint32_t mscnt;

	timerdone = 0;
	//mscnt = msval * onemscnt;
	mscnt = tms * onemscnt;
	MSS_TIM1_load_immediate(mscnt);
	MSS_TIM1_enable_irq();
	MSS_TIM1_start();

	while(timerdone == 0)
	{
		//busy wait loop
	}

	MSS_TIM1_disable_irq();
#else
	//usleep(tms*1000);
	long count = 0;
	long delayInMilliSeconds = MSS_SYS_M3_CLK_FREQ*tms / 1000;

    for(count = 0; count < delayInMilliSeconds; count++) {
    	asm("NOP\n");
    }
#endif
//    return SSL_STATUS_OK;
}

void Timer1_IRQHandler( void )
{
	timerdone = 1;
	MSS_TIM1_clear_irq();
}
