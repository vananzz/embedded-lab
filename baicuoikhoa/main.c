#include "MKL46Z4.h"
#include "stdio.h"
#include "Seg_LCD.h"
#include "I2C.h"
#include "mma.h"
#include "math.h"
#include "stdbool.h"
#include "stdlib.h"

void mma_calib(int* xoff, int* yoff, int* zoff);

int read_acc();

int main() {		
	SystemClockConfiguration();
  SegLCD_Init();
	mma_init();
	
	int x_off, y_off, z_off;
	
	mma_calib(&x_off, &y_off, &z_off);
	
	int off = sqrt(pow(x_off/3.0, 2)+pow(y_off/ 3.0, 2)+pow(z_off / 3.0, 2));
	
	int upper = 3500, lower = 1700;
	
	int count = 0;

	bool reach_upper = false;
	bool reach_lower = false;
	
	int acc, min_acc = 999999, max_acc = 0;

	while (1) {
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
		SegLCD_DisplayDecimal(count);

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