#ifndef INCLUDES_I2C_H_
#define INCLUDES_I2C_H_

#include "MKL46Z4.h"

#define I2C_READ  1
#define I2C_WRITE 0

void InitI2C(void);
void WriteToI2C(uint8_t data);
void I2CWait(void);
uint16_t I2C_Get_Ack(void);


#endif /* INCLUDES_I2C_H_ */
