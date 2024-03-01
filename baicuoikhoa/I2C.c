#include "I2C.h"

void InitI2C(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
    PORTE->PCR[24] = PORT_PCR_MUX(5);
		PORTE->PCR[25] = PORT_PCR_MUX(5);
    I2C0->F = 0x14;
    I2C0->C1 = 0x80;
}

void WriteToI2C(uint8_t data)
{
    I2C0->D = data;
}

void I2CWait(void){
	while((I2C0->S & I2C_S_IICIF_MASK)==0);
	I2C0->S |= I2C_S_IICIF_MASK;
}

uint16_t I2C_Get_Ack(void){
	if((I2C0->S & I2C_S_RXAK_MASK) == 0)
        return 1; //true=1
    else
        return 0;//false=0
}
