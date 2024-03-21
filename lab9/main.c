#include "MKL46Z4.h"
#include "Seg_LCD.h"

#define IDLE 0
#define SEATED 1
#define BELTED 2
#define BUZZER 3

void init();

int state = IDLE;

int main() {
	SystemClockConfiguration();
  SegLCD_Init();
	init();
	SegLCD_DisplayDecimal(8888);	
	
	PTD->PCOR = (1u<<5);
	
	PTE->PSOR |= 1u<<29;
	
	while (1) {
		switch (state) {
			case IDLE:
				PTD->PCOR = (1u<<5);
				break;
			case SEATED:
				break;
			case BELTED:
				break;
			case BUZZER:
				break;
		}
	}
	
	return 1;
}

void init() {
	// led xanh
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[5] |= (1ul<<8);
	PTD->PDDR |= (1ul << 5);
	// sw1
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	PORTC->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PTC->PDDR &= ~((uint32_t)(1u<<3));
	// led red
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	PORTE->PCR[29] |= (1ul<<8);
	PTE->PDDR |= (1ul << 29);
	// sw3
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PTC->PDDR &= ~((uint32_t)(1u<<12));
}	