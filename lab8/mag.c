#include "mag.h"

static void pause(void)
{
    int n;
    for(n=0; n<40; n++)
        __asm("nop");
//    for(n=0; n<100; n++);
}

void mag_set(uint8_t addr, uint8_t data)
{
		// Master & TX mode
		I2C0->C1 |=  I2C_C1_MST_MASK;
		I2C0->C1 |= I2C_C1_TX_MASK;
	
    WriteToI2C(MAG3110_I2C_ADDRESS|I2C_WRITE); //device ID
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

void mag_init(void)
{
		InitI2C();      //Initialize I2C modules done in read/write functions itself
		
    mag_set(MAG_CTRL_REG1,0x00); //Standby mode
    mag_set(MAG_CTRL_REG2,0x80); //Auto reset			5th bit: raw
    mag_set(MAG_CTRL_REG1,0x01); //Active
}

signed short mag_read(uint8_t addr)
{
		uint8_t result;
			// Master & TX mode
		I2C0->C1  |=  I2C_C1_MST_MASK;
		I2C0->C1 |= I2C_C1_TX_MASK;
    WriteToI2C(MAG3110_I2C_ADDRESS|I2C_WRITE); //Device ID
	
		I2CWait();
		I2C_Get_Ack();
	
	  WriteToI2C(addr); //Device ID
		I2CWait();
		I2C_Get_Ack();
	
		I2C0->C1 |= 0x04;
		WriteToI2C(MAG3110_I2C_ADDRESS|I2C_READ);
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



void mag_calib(int* xoff, int* yoff, int* zoff){
	short Xout_16_bit_avg, Yout_16_bit_avg, Zout_16_bit_avg;  
	short Xout_16_bit_max, Yout_16_bit_max, Zout_16_bit_max;  
	short Xout_16_bit_min, Yout_16_bit_min, Zout_16_bit_min;  
	signed short xread, yread, zread;
	short j=0;  
	  
	while (j < 200){ // Calibration process ~10s (200 samples * 1/20Hz)    
		if ((mag_read(0x00)&0xf) != 0){   
	  
				xread   = mag_read(0x01)<<8;
				xread  |= mag_read(0x02);

				yread   = mag_read(0x03)<<8;
				yread  |= mag_read(0x04);

				zread   = mag_read(0x05)<<8;
				zread  |= mag_read(0x06);
	  
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
	  
	// Left-shift by one as magnetometer offset registers are 15-bit only, left justified  
	Xout_16_bit_avg <<= 1;  
	Yout_16_bit_avg <<= 1;  
	Zout_16_bit_avg <<= 1;  
	  
	mag_set(MAG_CTRL_REG1, 0x00);  // Standby mode  
	  
	// Set Offset  
	mag_set(OFF_X_LSB, (char)(Xout_16_bit_avg & 0xFF));  
	mag_set(OFF_X_MSB, (char)((Xout_16_bit_avg >>8) & 0xFF));  
	mag_set(OFF_Y_LSB, (char)(Yout_16_bit_avg & 0xFF));  
	mag_set(OFF_Y_MSB, (char)((Yout_16_bit_avg >>8) & 0xFF));  
	mag_set(OFF_Z_LSB, (char)(Zout_16_bit_avg & 0xFF));  
	mag_set(OFF_Z_MSB, (char)((Zout_16_bit_avg >>8) & 0xFF));  
	  
	mag_set(MAG_CTRL_REG1, 0x01);  //  Active mode again  *****
}
