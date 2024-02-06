#include "MKL46Z4.h"

#define MASK(x) 1ul << x

uint32_t SysTick_Configuration (uint32_t ticks)
{
	SysTick->LOAD = ticks - 1; /* set reload register */
	NVIC_SetPriority (SysTick_IRQn, (1 << 4) - 1); /* set Priority for Systick Interrupt */
	SysTick->VAL = 0; /* Load the SysTick Counter Value */
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |

	SysTick_CTRL_TICKINT_Msk |
	SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */

	return (0); /* Function successful */
}

void initRedLed() {
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	PORTE->PCR[29] |= (1ul<<8);
	PTE->PDDR |= (1ul << 29);
}

void initSw1() {
	// sw1
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	PORTC->PCR[3] = PORT_PCR_IRQC(0xA);
	PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PTC->PDDR &= ~((uint32_t)(1u<<3));
	NVIC_ClearPendingIRQ(31);
	NVIC_EnableIRQ(31);
}

void initSysTick() {
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
}

void toggleRedLed() {
	PTE->PTOR |= 1ul<<29;
}

void PORTC_PORTD_IRQHandler(void) {
	/* Put a proper name of PORTC_PORTD Interrupt service routine ISR. See
	startup_MKL46Z4.s file for function name */

	/*do anything you want here*/
	uint32_t i = 0;
	for(i = 0; i < 1000; i++);
	if ((PTC->PDIR & (1<<3)) == 0) toggleRedLed();
	PORTC->PCR[3] |= PORT_PCR_ISF_MASK;
	/* Clear interrupt service flag in port control register otherwise int.
	remains active */
}

void SysTick_Handler() {
	toggleRedLed();
	TPM0->SC &= ~MASK(7);
	SysTick->VAL = 0;
}

int main() {
	initSw1();
	initRedLed();
	initSysTick();
	
	while (1) {
		SysTick_Configuration(SystemCoreClock);
	};
	return 0;
}