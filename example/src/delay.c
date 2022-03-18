#include "delay.h"
#include "chip.h"
#include "main.h"

static volatile uint32_t msTicks;
/*static volatile uint32_t msTicks1;
static volatile uint32_t msTicks2;*/
static volatile uint16_t timeoutTicks;
//static uint32_t curTicks;
/**
 * SysTick Handler increments both delay counters
 */
void SysTick_Handler(void)
{
	msTicks++;
	timeoutTicks++;
}

/**
 * Non-blocking delay function
 * @param del - delay in milliseconds
 */
void _delay_ms(uint32_t del)
{
	uint32_t curTicks;
	curTicks = msTicks = 0;

	while ((msTicks - curTicks) < del)
	{
		__WFI();
	}
}

/**
 * Resetting counter which is used to control timeouts
 */
void resetTimeoutTicks(void)
{
	timeoutTicks = 0;
}

/**
 * Returning how much ticks passed from resetting counter
 * @return - counted tick value
 */
uint16_t getTimeoutTicks (void)
{
	return timeoutTicks;
}

void short_delay(int n)
{
	volatile uint64_t d;
 	for (d = 0; d < n * 3000; d++)
	{
__NOP();
	}
}
