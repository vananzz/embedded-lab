#include "MKL46Z4.h"
#define MOD 4

void Delay ();

void init () {
	#if MOD == 1
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[5] |= (1ul<<8);
	PTD->PDDR |= (1ul << 5);
	#elif MOD == 2	
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	PORTE->PCR[29] |= (1ul<<8);
	PTE->PDDR |= (1ul << 29);
	#elif MOD == 3	
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[5] |= (1ul<<8);
	PTD->PDDR |= (1ul << 5);

	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	PORTE->PCR[29] |= (1ul<<8);
	PTE->PDDR |= (1ul << 29);
	#elif MOD == 4
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
	#endif
}

void main() {
	init();
	while(1) {
		PTD->PTOR |= 1ul<<5;
		#if MOD == 1
		PTD->PTOR |= (1u<<5); //Change PTD5 to 1, turns LED OFF
		Delay();
		//for (int i; i < 3000000; i++){}; //Burn some time
		PTD->PTOR |= (1u<<5); //~((uint32_t)(1u<<5)); //Change PTD5 to 0, turns LED ON. .
		//for (int i; i < 3000000; i++){}; //Burn some time
		Delay();
		#elif MOD == 2
		PTE->PDOR |= (uint32_t)(0xFFF00000);
		for (int i; i < 3000000; i++){}; 
		PTE->PDOR &= ~((uint32_t)(0xFFF00000));
		for (int i; i < 3000000; i++){};
		#elif MOD == 3
		PTD->PDOR |= (1u<<5); //Change PTD5 to 1, turns LED OFF
		PTE->PDOR &= ~((uint32_t)(0xFFF00000));
		for (int i; i < 3000000; i++){}; //Burn some time
		PTD->PDOR &= ~((uint32_t)(1ul<<5)); //~((uint32_t)(1u<<5)); //Change PTD5 to 0, turns LED ON. .
		PTE->PDOR |= (uint32_t)(0xFFF00000);
		for (int i; i < 3000000; i++){}; //Burn some time
		#elif MOD == 4
		if ((PTC -> PDIR & (1<<3))==0) {
			PTD->PTOR |= 1ul<<5;
		}
		if ((PTC->PDIR & (1<<12)) == 0) {
			PTE->PTOR |= 0xFFF00000;
		}
		for (int i; i < 1000000; i++){};
		#endif
	}
}