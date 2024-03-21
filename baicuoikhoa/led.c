#include "led.h"

void initGreenLed() {
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[5] |= (1ul<<8);
	PTD->PDDR |= (1ul << 5);
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

void turnOffRedLed() {
	PTE->PDOR |= (uint32_t)(0xFFF00000);
}

void turnOnGreenLed() {
	PTD->PDOR |= (1u<<5);
}


void turnOnRedLed() {
	PTE->PDOR &= ~((uint32_t)(0xFFF00000));
}


void turnOffGreenLed() {
	//PTD->PDOR &= ~((uint32_t)(1ul<<5));
	PTD->PDOR |= (1u<<5);
}
