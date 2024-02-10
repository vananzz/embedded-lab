#include "MKL46Z4.h"

#define MASK(x) 1ul << x

uint32_t SysTick_Configuration (uint32_t ticks)
{
	SysTick->LOAD = ticks * (SystemCoreClock/1000); //ticks - 1; /* set reload register */
	NVIC_SetPriority (SysTick_IRQn, (1 << 4) - 1); /* set Priority for Systick Interrupt */
	SysTick->VAL = 0; /* Load the SysTick Counter Value */
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
	SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */

	return (0); /* Function successful */
}

void initLed() {
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[5] |= (1ul<<8);
	PTD->PDDR |= (1ul << 5);
}

void toggleLed () {
	PTD->PTOR |= MASK(5);
}

int main()
{
	SIM->SCGC6 |= MASK(24);
	SIM->SOPT2 |= MASK(24);
	// tat
	TPM0->SC &= ~(MASK(3) | MASK(4));
	// chia 128
	TPM0->SC |= MASK(0) | MASK(1) | MASK(2);
	// dem tien
	TPM0->SC &= ~MASK(5);
	// ngat khi tran
	TPM0->SC |= MASK(6);
	// xoa TOF
	TPM0->SC &= ~MASK(7);
	// set bo dem
	TPM0->MOD |= 0x0000;
	// bat bo dinh thoi
	TPM0->SC |= MASK(3);
	initLed();
	while (1)
	{
		//toggleLed();
		if (TPM0->SC & MASK(7)) {
			toggleLed();
			TPM0->SC &= ~MASK(7);
			SysTick_Configuration(SystemCoreClock);
			SysTick->VAL = 0;
		} /* if COUNT flag is set */
	}
}