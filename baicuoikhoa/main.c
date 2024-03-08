#include "MKL46Z4.h"
#include "Seg_LCD.h"
#include "I2C.h"
#include "mma.h"
#include "math.h"

int main() {		
	SystemClockConfiguration();
  SegLCD_Init();
	mma_init();

	int x_off, y_off, z_off;
	int x = 0, y = 0, z = 0;

	while (1) {
		if((mma_read(STATUS_REG)&0xf) == 0xf) {
			for (int i = 0; i < 1000000; i ++);
			x = mma_read(OUT_X_MSB)<<6;
			x |= mma_read(OUT_X_LSB)>>2;

			y = mma_read(OUT_Y_MSB)<<6;
			y |= mma_read(OUT_Y_LSB)>>2;

			z = mma_read(OUT_Z_MSB)<<6;
			z |= mma_read(OUT_Z_LSB)>>2;
		}
		SegLCD_DisplayDecimal((sqrt(pow(x/100.0, 2)+pow(y/ 100.0, 2)+pow(z / 100.0, 2))));
		for (int i = 0; i < 3000000; i ++);
	}

	return 1;
}
