#include "MKL46Z4.h" // Device header
#include "cmsis_os.h" // RTOS:Keil RTX header
void blink_LED (void const *argument); // Prototype function
osThreadDef (blink_LED, osPriorityNormal, 1, 0); // Define blinky thread

void LED_Initialize (void) {
//This function enables the red LED on the FRDM-KL46Z development board
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[5] |= (1ul<<8);
	PTD->PDDR |= (1ul << 5);
}

/* Turn LED OFF */
void LED_Off (void) {
	PTD->PSOR = (1u<<5); //Set PTD5 = 1, turns LED OFF (Cathode connected to PTD5)
}

/* Turn LED On */
void LED_On (void) {
	PTD->PCOR = (1u<<5); //Clear PTD5 = 0, turns LED ON
}

// Blink LED function
void blink_LED(void const *argument) {
for (;;) {
LED_On (); // Switch LED on
osDelay (500); // Delay 500 ms
LED_Off (); // Switch off
osDelay (500); // Delay 500 ms
}
}
void Init_BlinkyThread (void) {
osThreadCreate (osThread(blink_LED), NULL); // Create thread
}