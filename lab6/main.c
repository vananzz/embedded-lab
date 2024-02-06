#include "MKL46Z4.h"

void initSw1() {
	// sw1
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	PORTC->PCR[3] = PORT_PCR_IRQC(0xA);
	PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PTC->PDDR &= ~((uint32_t)(1u<<3));
	NVIC_ClearPendingIRQ(31);
	NVIC_EnableIRQ(31);
}

void initSw3() {
	// sw3
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTC->PCR[12] |= PORT_PCR_IRQC(0xA);
	PTC->PDDR &= ~((uint32_t)(1u<<12));
	NVIC_ClearPendingIRQ(30);
	NVIC_EnableIRQ(30);
}

void initGreenLed() {
	// led xanh
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[5] |= (1ul<<8);
	PTD->PDDR |= (1ul << 5);
	//PTD->PTOR |= 1ul<<5;
}

void initRedLed() {
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	PORTE->PCR[29] |= (1ul<<8);
	PTE->PDDR |= (1ul << 29);
}

void toggleGreenLed() {
	PTD->PTOR |= 1ul<<5;
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

int main() {
	initSw1();
	initRedLed();
	while (1) {};
	return 0;
}

