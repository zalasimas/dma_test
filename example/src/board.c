
#include <math.h>

#include "board.h"
#include "cdc_main.h"
#include "chip.h"

static volatile uint32_t adc_timer_matchval;

/**
 * Initializing board
 */
void BOARD_Init(void)
{
	uint16_t rawTxAdc[] = { 0xc000, 0 }; //set channel 0

	Chip_GPIO_SetPinState(LPC_GPIO, 0, 7, false);
}


/**
 * Eables interrupts on VUSB Pins
 * */
void BOARD_VUSB_Interupt_Init(void)
{
	Chip_SYSCTL_SetPinInterrupt(0, 0, 3);
	Chip_SYSCTL_SetPinInterrupt(1, 0, 3);

	Chip_Clock_EnablePeriphClock (SYSCTL_CLOCK_PINT);

	Chip_PININT_SetPinModeLevel(LPC_PININT, PININTCH0);
	Chip_PININT_EnableIntHigh(LPC_PININT, PININTCH0);

	Chip_PININT_SetPinModeLevel(LPC_PININT, PININTCH1);
	Chip_PININT_EnableIntLow(LPC_PININT, PININTCH1);

	NVIC_SetPriority(PIN_INT0_IRQn, 3);
	NVIC_EnableIRQ (PIN_INT0_IRQn);
	NVIC_SetPriority(PIN_INT1_IRQn, 2);
	NVIC_DisableIRQ (PIN_INT1_IRQn);

}


/**
 * Sets Timer match for input debouncing. set this timer with addition to input_filter_count.
 * When timer waits longer, filter count must be added bigger. Now: timer(1), filter_count(+10)
 * @param delay_milis
 */
void Timer_Init_Debounce(void)//LPC_TIMER16_0
{
	Chip_TIMER_Init (LPC_TIMER16_0);
	Chip_TIMER_Reset(LPC_TIMER16_0);
	Chip_TIMER_MatchEnableInt(LPC_TIMER16_0, 1);
	Chip_TIMER_PrescaleSet(LPC_TIMER16_0, (Chip_Clock_GetSystemClockRate() / 1000000));
	Chip_TIMER_SetMatch(LPC_TIMER16_0, 1, 1000);
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER16_0, 1);
	Chip_TIMER_Enable(LPC_TIMER16_0);
}

void Timer_Init_Output_Pulse(void)//LPC_TIMER32_1
{
	Chip_TIMER_Init(LPC_TIMER32_1);
	Chip_TIMER_Reset(LPC_TIMER32_1);
    Chip_TIMER_MatchEnableInt(LPC_TIMER32_1, 1);
    Chip_TIMER_PrescaleSet(LPC_TIMER32_1, (Chip_Clock_GetSystemClockRate() / 1000000));
    Chip_TIMER_SetMatch(LPC_TIMER32_1, 1, 1000);
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER32_1, 1);
	Chip_TIMER_Enable(LPC_TIMER32_1);
}

void Timer_For_RTC_Init(void)//LPC_TIMER32_0
{
	Chip_TIMER_Init (LPC_TIMER32_0);
	Chip_TIMER_Reset(LPC_TIMER32_0);
	Chip_TIMER_PrescaleSet(LPC_TIMER32_0, 48000000);
	Chip_TIMER_Enable(LPC_TIMER32_0);
}

void Timer_For_ADC_Init(void)//LPC_TIMER16_1
{
	Chip_TIMER_Init (LPC_TIMER16_1);
	Chip_TIMER_Reset(LPC_TIMER16_1);
	//Chip_TIMER_SetMatch(LPC_TIMER16_1, 0, 4000); //Sampling 6000Hz (50Hz * 120 SAMPLES)
	//Chip_TIMER_SetMatch(LPC_TIMER16_1, 0, 3750); //Sampling 6400Hz (50Hz * 128 SAMPLES)
	//Chip_TIMER_SetMatch(LPC_TIMER16_1, 0, 3125); //Sampling 7680Hz (60Hz * 128 SAMPLES)
	//Chip_TIMER_SetMatch(LPC_TIMER16_1, 0, 6250); //Sampling 3840Hz (60Hz * 64 SAMPLES)
	Chip_TIMER_SetMatch(LPC_TIMER16_1, 0, adc_timer_matchval);
	Chip_TIMER_PrescaleSet(LPC_TIMER16_1, 1);// 1
	Chip_TIMER_MatchEnableInt(LPC_TIMER16_1, 0);
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER16_1, 0);
	Chip_TIMER_Enable(LPC_TIMER16_1);
	NVIC_EnableIRQ (TIMER_16_1_IRQn);
	NVIC_SetPriority(TIMER_16_1_IRQn, 2);
}

/**
 * Deinitializing interrupts
 */
void Interrupt_DeInit(void)
{
	NVIC_DisableIRQ(TIMER_32_0_IRQn);
	NVIC_DisableIRQ(TIMER_32_1_IRQn);
	NVIC_DisableIRQ(PIN_INT1_IRQn);
	NVIC_DisableIRQ(PIN_INT0_IRQn);
	NVIC_DisableIRQ(USART0_IRQn);
	NVIC_DisableIRQ(TIMER_16_0_IRQn);
	NVIC_DisableIRQ(TIMER_16_1_IRQn);
	NVIC_EnableIRQ(SysTick_IRQn);
}

/**
 * Timer for timeouts (might be deprecated)
 */
void Timer_Init_Timeout(void)
{
	__disable_irq();
	Chip_TIMER_Init (LPC_TIMER16_0);
	Chip_TIMER_SetMatch(LPC_TIMER16_0, 0, 0xffff);
	Chip_TIMER_PrescaleSet(LPC_TIMER16_0, (SystemCoreClock / 1000) - 1);
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER16_0, 0);
	Chip_TIMER_Reset(LPC_TIMER16_0);
	Chip_TIMER_Enable(LPC_TIMER16_0);
	__enable_irq();
}

/**
 * Function to swap two values between them in memory
 * @param a - first value
 * @param b - second value
 */
void swapTwoValues(volatile int *a, volatile int *b)
{
	int16_t temp = *a;
	*a = *b;
	*b = temp;
}



