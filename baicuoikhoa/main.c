#include "MKL46Z4.h"
#include "Seg_LCD.h"
#include "I2C.h"
#include "mma.h"
#include "math.h"

void my_mag_calib(int* xoff, int* yoff, int* zoff);

int main() {		
	SystemClockConfiguration();
  SegLCD_Init();
	//InitI2C();
	mma_init();
	//init_I2C0();
	//init_MAG3110();

	int x_off, y_off, z_off;
	for (int i = 0; i < 3000000; i ++);
	//my_mag_calib(&x_off, &y_off, &z_off);
	SegLCD_DisplayDecimal(0000);
	for (int i = 0; i < 3000000; i ++);
	int x, y, z;

	while (1) {
		
		if((mma_read(0x00)&0xf) != 0) {
			x = mma_read(0x01)<<8;
			x |= mma_read(0x02);

			y = mma_read(0x03)<<8;
			y |= mma_read(0x04);

			z = mma_read(0x05)<<8;
			z |= mma_read(0x6);
		}
			SegLCD_DisplayDecimal((sqrt(pow(x/100.0, 2)+pow(y/ 100.0, 2)+pow(z / 100.0, 2))));
			for (int i = 0; i < 3000000; i ++);
			//SegLCD_DisplayDecimal(x);  
	}

	return 1;
}
