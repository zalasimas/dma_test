#ifndef DELAY_H_
#define DELAY_H_

#ifdef __cpluplus
	extern "C"
	{
#endif

#include <stdint.h>

void _delay_ms(uint32_t del);
//void _delay_ms1(uint32_t del);
//void _delay_ms2(uint32_t del);
void resetTimeoutTicks(void);
uint16_t getTimeoutTicks (void);
int Generate_Response_Delay(int size);

void short_delay(int n) __attribute__((noinline));

#ifdef __cpluplus
	}
#endif

#endif /* DELAY_H_ */
