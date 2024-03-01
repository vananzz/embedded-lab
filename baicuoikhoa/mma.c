#include "mma.h"

static void pause(void)
{
    int n;
    for(n=0; n<40; n++)
        __asm("nop");
//    for(n=0; n<100; n++);
}

void mma_set(uint8_t addr, uint8_t data)
{
		// Master & TX mode
		I2C0->C1 |=  I2C_C1_MST_MASK;
		I2C0->C1 |= I2C_C1_TX_MASK;
		
    WriteToI2C(MMA8451Q_I2C_ADDRESS|I2C_WRITE); //device ID
		I2CWait();
		I2C_Get_Ack();
	
    WriteToI2C(addr); //register address - C2
		I2CWait();
		I2C_Get_Ack();
	
    WriteToI2C(data); //register address - C1
		I2CWait();
		I2C_Get_Ack();
	
		// Slave & RX mode
		I2C0->C1 &= ~I2C_C1_MST_MASK;
		I2C0->C1 &= ~I2C_C1_TX_MASK;
		pause();
}

void mma_init(void)
{
		InitI2C();      //Initialize I2C modules done in read/write functions itself
		
    mma_set(MMA_CTRL_REG1,0x00); //Standby mode
    mma_set(MMA_CTRL_REG2,0x40); //Auto reset
    mma_set(MMA_CTRL_REG1,0x01); //Active
}

signed short mma_read(uint8_t addr)
{
		uint8_t result;
			// Master & TX mode
		I2C0->C1  |=  I2C_C1_MST_MASK;
		I2C0->C1 |= I2C_C1_TX_MASK;
    WriteToI2C(MMA8451Q_I2C_ADDRESS|I2C_WRITE); //Device ID
	
		I2CWait();
		I2C_Get_Ack();
	
	  WriteToI2C(addr); //Device ID
		I2CWait();
		I2C_Get_Ack();
	
		I2C0->C1 |= 0x04;
		WriteToI2C(MMA8451Q_I2C_ADDRESS|I2C_READ);
		I2CWait();
		I2C_Get_Ack();
	
		I2C0->C1 &= ~I2C_C1_TX_MASK;
		I2C0->C1 |= I2C_C1_TXAK_MASK;
		result = I2C0->D;
		I2CWait();
		
		// Slave & RX mode
		I2C0->C1  &= ~I2C_C1_MST_MASK;
		I2C0->C1 &= ~I2C_C1_TX_MASK;
		result = I2C0->D;
		pause();
    return result;
}