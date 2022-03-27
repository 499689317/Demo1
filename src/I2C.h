/**
 * i2c通信驱动声明
 */

#ifndef I2C_H
#define I2C_H

extern void I2CDelay();
extern void I2CStart();
extern void I2CStop();
extern unsigned char I2CWrite(unsigned char data);
unsigned char I2CAckRead(unsigned char ack);

#endif