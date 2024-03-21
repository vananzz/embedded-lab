#include "MKL46Z4.h"
#include "stdio.h"
#include "Seg_LCD.h"
#include "I2C.h"
#include "mma.h"
#include "math.h"
#include "stdbool.h"
#include "stdlib.h"

#define IDLE 1
#define COUNTING 2

void mma_calib(int* xoff, int* yoff, int* zoff);

int state = IDLE;

int count = 0;

int x_off, y_off, z_off;

void initGreenLed();

void initRedLed();

void initSwitch1();

void initSwitch3();

void displayNewValue() {
	SegLCD_DisplayDecimal(count);
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


uint32_t SysTick_Configuration (uint32_t ticks){
	SysTick->LOAD  = ticks -1;                                  /* set reload register */
	NVIC_SetPriority(SysTick_IRQn, (1<<4) -1);  /* set Priority for SystickInterrupt */
	SysTick->VAL   = 0;                                          /* Load theSysTickCounter Value */
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk|
	SysTick_CTRL_TICKINT_Msk |
	SysTick_CTRL_ENABLE_Msk;                 /* Enable SysTickIRQ and SysTickTimer */
	return (0);                                                  /* Function successful */
}

void init_SysTick_interrupt(){
	SysTick->LOAD = SystemCoreClock/ 1000; //configured the SysTickto count in 1ms
	/* Select Core Clock & Enable SysTick& Enable Interrupt */
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk|SysTick_CTRL_TICKINT_Msk|SysTick_CTRL_ENABLE_Msk;
}

int32_t volatile msTicks = 0; // Interval counter in ms

unsigned long long tinhthoigian = 0;	//	Increasing second
long long SysTick_Handler(void) { // SysTickinterrupt Handler
	msTicks++; // Increment counter 
	if(msTicks == 1000){
		tinhthoigian++;
		if (state == COUNTING) toggleGreenLed();
		msTicks = 0;
	}
	return tinhthoigian;
}

int read_acc();

void PORTC_PORTD_IRQHandler(void) {
	/* Put a proper name of PORTC_PORTD Interrupt service routine ISR. See
	startup_MKL46Z4.s file for function name */

	/*do anything you want here*/
	uint32_t i = 0;
	for(i = 0; i < 1000; i++);
	if ((PTC->PDIR & (1<<3)) == 0) {
		if (state == IDLE) {
			state = COUNTING;
			turnOffRedLed();
		} else {
			state = IDLE;
			turnOnRedLed();
			turnOffGreenLed();
		}
	}
	if ((PTC->PDIR & (1<<12)) == 0) {
		count = 0;
		displayNewValue();
	}
	PORTC->PCR[3] |= PORT_PCR_ISF_MASK;
	PORTC->PCR[12] |= PORT_PCR_ISF_MASK;
	/* Clear interrupt service flag in port control register otherwise int.
	remains active */
}

void init() {
	SystemClockConfiguration();
  SegLCD_Init();
	mma_init();
	initGreenLed();
	initRedLed();
	initSwitch1();
	initSwitch3();
	init_SysTick_interrupt();
}

int main() {		
	init();

	mma_calib(&x_off, &y_off, &z_off);
	

	int off = sqrt(pow(x_off/3.0, 2)+pow(y_off/ 3.0, 2)+pow(z_off / 3.0, 2));
	
	displayNewValue();
	
	int upper = 4000, lower = 1400;

	bool reach_upper = false;
	bool reach_lower = false;
	
	int acc, min_acc = 999999, max_acc = 0;

	turnOffGreenLed();
	while (1) {
		switch(state) {
			case IDLE:
				turnOffGreenLed();
				break;
			case COUNTING:
				if((mma_read(STATUS_REG)&0xf) == 0xf) {
					acc = abs(read_acc()) - abs(off);
					
					if (!(acc > 0 && acc < 9999)) continue;
					
					min_acc = acc < min_acc && acc > 0 && acc < 9999 ? acc : min_acc;
					max_acc = acc > max_acc && acc > 0 && acc < 9999 ? acc : max_acc;
				}
				
				if (acc > upper) reach_upper = true;
				if (acc < lower) reach_lower = true;
				
				if (reach_lower && reach_upper) {
					count++;
					reach_lower = false;
					reach_upper = false;
				}
				//sqrt(pow(x_off/3.0, 2)+pow(y_off/ 3.0, 2)+pow(z_off / 3.0, 2));
				//SegLCD_DisplayDecimal(max_acc);
				displayNewValue();
				
				break;
		}
	}

	return 1;
}

int read_acc() {
	int result = 0;
	int x, y, z;
	x = mma_read(OUT_X_MSB)<<6;
	x |= mma_read(OUT_X_LSB)>>2;

	y = mma_read(OUT_Y_MSB)<<6;
	y |= mma_read(OUT_Y_LSB)>>2;

	z = mma_read(OUT_Z_MSB)<<6;
	z |= mma_read(OUT_Z_LSB)>>2;
	
	result = (sqrt(pow(x/3.0, 2)+pow(y/ 3.0, 2)+pow(z / 3.0, 2)));
	//result = (sqrt(pow(x/3.0, 2)+pow(y/ 3.0, 2)));
	return result;
}


void mma_calib(int* xoff, int* yoff, int* zoff){
	short Xout_16_bit_avg, Yout_16_bit_avg, Zout_16_bit_avg;  
	short Xout_16_bit_max, Yout_16_bit_max, Zout_16_bit_max;  
	short Xout_16_bit_min, Yout_16_bit_min, Zout_16_bit_min;  
	signed short xread, yread, zread;
	short j=0;  
	  
	while (j < 200){ // Calibration process ~10s (200 samples * 1/20Hz)
		SegLCD_DisplayDecimal(j);   
		if ((mma_read(STATUS_REG)&0xf) == 0xf){   
	  
				xread = mma_read(OUT_X_MSB)<<6;
				xread |= mma_read(OUT_X_LSB)>>2;

				yread = mma_read(OUT_Y_MSB)<<6;
				yread |= mma_read(OUT_Y_LSB)>>2;

				zread = mma_read(OUT_Z_MSB)<<6;
				zread |= mma_read(OUT_Z_LSB)>>2;
	  
				if (j == 0)  
				{  
					Xout_16_bit_max = xread;  
					Xout_16_bit_min = xread;  
	  
					Yout_16_bit_max = yread;  
					Yout_16_bit_min = yread;  
	  
					Zout_16_bit_max = zread;  
					Zout_16_bit_min = zread;  
				}  
	  
				// Check to see if current sample is the maximum or minimum X-axis value  
				if (xread > Xout_16_bit_max) {Xout_16_bit_max = xread;}  
				if (xread < Xout_16_bit_min) {Xout_16_bit_min = xread;}  
	  
				// Check to see if current sample is the maximum or minimum X-axis value  
				if (yread > Yout_16_bit_max) {Yout_16_bit_max = yread;}  
				if (yread < Yout_16_bit_min) {Yout_16_bit_min = yread;}  
	  
				// Check to see if current sample is the maximum or minimum X-axis value  
				if (zread > Zout_16_bit_max) {Zout_16_bit_max = zread;}  
				if (zread < Zout_16_bit_min) {Zout_16_bit_min = zread;}  
	  
				j++;  
			}  
	}  
	  
	  
	Xout_16_bit_avg = (Xout_16_bit_max + Xout_16_bit_min) / 2;    // X-axis hard-iron offset  
	Yout_16_bit_avg = (Yout_16_bit_max + Yout_16_bit_min) / 2;    // Y-axis hard-iron offset  
	Zout_16_bit_avg = (Zout_16_bit_max + Zout_16_bit_min) / 2;    // Z-axis hard-iron offset  
	
	(*xoff) = Xout_16_bit_avg;
	(*yoff) = Yout_16_bit_avg;
	(*zoff) = Zout_16_bit_avg;
	  
	mma_set(MMA_CTRL_REG1, 0x00);  // Standby mode  
	  
	// Set Offset  
	mma_set(OFF_X, (char)(Xout_16_bit_avg & 0xFF));
	mma_set(OFF_Y, (char)(Yout_16_bit_avg & 0xFF));
	mma_set(OFF_Z, (char)(Zout_16_bit_avg & 0xFF));
	  
	mma_set(MMA_CTRL_REG1, 0x01);  //  Active mode again  *****
}

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

void initSwitch1() {
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	PORTC->PCR[3] = PORT_PCR_IRQC(0xA);
	PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PTC->PDDR &= ~((uint32_t)(1u<<3));
	NVIC_ClearPendingIRQ(31);
	NVIC_EnableIRQ(31);
}

void initSwitch3() {
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTC->PCR[12] |= PORT_PCR_IRQC(0xA);
	PTC->PDDR &= ~((uint32_t)(1u<<12));
	NVIC_ClearPendingIRQ(30);
	NVIC_EnableIRQ(30);
}