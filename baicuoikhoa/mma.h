#ifndef __MMA_H_
#define __MMA_H_

#include "MKL46Z4.h"
#include "I2C.h"

#define MMA8451Q_I2C_ADDRESS (0x1D << 1)
#define MMA_CTRL_REG1  0x2A // ODR = 800 Hz, standby mode.
#define MMA_CTRL_REG2  0x2B // Sleep enable, OS modes, RST, ST
#define MMA_CTRL_REG3  0x2C // Wake from sleep, IPOL, PP_OD
#define MMA_CTRL_REG4  0x2D // Interrupt enable register
#define MMA_CTRL_REG5  0x2E // Interrupt pin (INT1/INT2) map

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
void mma_set(uint8_t addr, uint8_t data);
void mma_init(void);
signed short mag_read(uint8_t addr);

#endif
