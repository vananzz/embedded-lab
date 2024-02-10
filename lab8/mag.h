#ifndef __MAG_H_
#define __MAG_H_

#include "MKL46Z4.h"
#include "I2C.h"

#define MAG3110_I2C_ADDRESS (0x0E << 1)
#define MAG_CTRL_REG1 0x10
#define MAG_CTRL_REG2 0x11

#define STATUS_REG	0x00
#define OUT_X_MSB	0x01
#define OUT_X_LSB	0x02
#define OUT_Y_MSB	0x03
#define OUT_Y_LSB	0x04
#define OUT_Z_MSB	0x05
#define OUT_Z_LSB	0x06
#define WHO_AM_I	0x07

#define SYSMOD		0x08
#define OFF_X_MSB	0x09
#define OFF_X_LSB 	0x0A
#define OFF_Y_MSB 	0x0B
#define OFF_Y_LSB 	0x0C
#define OFF_Z_MSB 	0x0D
#define OFF_Z_LSB 	0x0E
#define DIE_TEMP	0x0F

static void pause(void);
void mag_set(uint8_t addr, uint8_t data);
void mag_init(void);
signed short mag_read(uint8_t addr);
void mag_calib(int* xoff, int* yoff, int* zoff);
//signed short xread, yread, zread;

#endif
