#include "MKL46Z4.h"
#include "Seg_LCD.h"
void init_I2C0();
void init_MAG3110();
void delayMs(int n);
void write(int reg, int data);
int read(int reg);
void waitACK();

int main() {
	SystemClockConfiguration();
  SegLCD_Init();
	init_I2C0();
	init_MAG3110();

	
	int result;

	while (1) {
		write(0x11, 1);
		//I2C0->C1 |= I2C_C1_MST(1);
		//I2C0->D = (0x0E << 1) | 1;
		//waitACK();
		
		SegLCD_DisplayDecimal(1000);
	}

	return 1;
}

void write(int reg, int data){
	I2C0->C1 |= I2C_C1_MST(1);
	I2C0->D = (0x0E << 1) | 1;
	waitACK();
	//I2C0->D = reg;
	//waitACK();
	//I2C0->D = 1;
	//waitACK();
	//I2C0->C1 &= ~I2C_C1_MST(1);
}

int read(int reg){
	int data;
	I2C0->C1 |= I2C_C1_MST(1);
	I2C0->D = (0x0E << 1) | 1;
	waitACK();
	I2C0->D = reg;
	waitACK();
	I2C0->D = (0x0E << 1);
	waitACK();
	data = I2C0->D;
	I2C0->C1 &= ~I2C_C1_MST(1);
	return data;
}

void waitACK(){
	while ((I2C0->S & ~I2C_S_TCF_MASK) != 1);
}

void init_I2C0() {
	SIM->SCGC4 |= SIM_SCGC4_I2C0(1);
  I2C0->F |= I2C_F_MULT(1) | I2C_F_ICR(0x01);
	I2C0->C1 |= I2C_C1_IICEN(1) | I2C_C1_MST(1) | I2C_C1_TXAK_MASK;
	//I2C0->A1 |= 0x0E;
}

void init_MAG3110() {
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	PORTE->PCR[25] |= PORT_PCR_MUX(0x5);
	PORTE->PCR[24] |= PORT_PCR_MUX(0x5);
}
