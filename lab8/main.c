#include "MKL46Z4.h"
#include "Seg_LCD.h"
#include "I2C.h"
#include "mag.h"

void my_mag_calib(int* xoff, int* yoff, int* zoff);

int main() {		
	SystemClockConfiguration();
  SegLCD_Init();
	//InitI2C();
	mag_init();
	//init_I2C0();
	//init_MAG3110();

	int x_off, y_off, z_off;
	SegLCD_DisplayDecimal(88.21);
	for (int i = 0; i < 3000000; i ++);
	//my_mag_calib(&x_off, &y_off, &z_off);
	SegLCD_DisplayDecimal(0000);
	for (int i = 0; i < 3000000; i ++);
	int x, y, z;
	while (1) {
		x = mag_read(0x01)<<8;
		x |= mag_read(0x02);
		
		y = mag_read(0x03)<<8;
		y |= mag_read(0x04);

		z = mag_read(0x05)<<8;
		z |= mag_read(0x6);
		//SegLCD_DisplayDecimal((int)(sqrt(x*x+y*y+z*z) / 65536 / sqrt(2) * 9999));
		SegLCD_DisplayDecimal((int)(sqrt(x*x+y*y+z*z) / 113509 * 9999));
		for (int i = 0; i < 3000000; i ++);
		//SegLCD_DisplayDecimal(x); 
	}

	return 1;
}

void my_mag_calib(int* xoff, int* yoff, int* zoff){
	short Xout_16_bit_avg, Yout_16_bit_avg, Zout_16_bit_avg;  
	short Xout_16_bit_max, Yout_16_bit_max, Zout_16_bit_max;  
	short Xout_16_bit_min, Yout_16_bit_min, Zout_16_bit_min;  
	signed short xread, yread, zread;
	short j=0;  
	
	SegLCD_DisplayDecimal(2000);
	for (int i = 0; i < 3000000; i ++);
	while (j < 500){ // Calibration process ~10s (200 samples * 1/20Hz)
		SegLCD_DisplayDecimal(j);
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
	  
				// Check to see if current sample is the maximum or minimum Y-axis value  
				if (yread > Yout_16_bit_max) {Yout_16_bit_max = yread;}  
				if (yread < Yout_16_bit_min) {Yout_16_bit_min = yread;}  
	  
				// Check to see if current sample is the maximum or minimum Z-axis value  
				if (zread > Zout_16_bit_max) {Zout_16_bit_max = zread;}  
				if (zread < Zout_16_bit_min) {Zout_16_bit_min = zread;}  
	  
				j++;  
			}  
	}  

	SegLCD_DisplayDecimal(3000);
	for (int i = 0; i < 3000000; i ++);

	Xout_16_bit_avg = (Xout_16_bit_max + Xout_16_bit_min) / 2;    // X-axis hard-iron offset  
	Yout_16_bit_avg = (Yout_16_bit_max + Yout_16_bit_min) / 2;    // Y-axis hard-iron offset  
	Zout_16_bit_avg = (Zout_16_bit_max + Zout_16_bit_min) / 2;    // Z-axis hard-iron offset  

	SegLCD_DisplayDecimal(4000);
	for (int i = 0; i < 3000000; i ++){};

	SegLCD_DisplayDecimal(Xout_16_bit_avg);
	for (int i = 0; i < 3000000; i ++){};

	*xoff = Xout_16_bit_avg;
	*yoff = Yout_16_bit_avg;
	*zoff = Zout_16_bit_avg;
	
	SegLCD_DisplayDecimal(5000);
	for (int i = 0; i < 3000000; i ++);

	// Left-shift by one as magnetometer offset registers are 15-bit only, left justified  
	Xout_16_bit_avg <<= 1;  
	Yout_16_bit_avg <<= 1;  
	Zout_16_bit_avg <<= 1; 

	SegLCD_DisplayDecimal(6000);
	for (int i = 0; i < 3000000; i ++);
	  
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