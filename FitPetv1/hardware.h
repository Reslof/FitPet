#ifndef HARDWARE_H
#define HARDWARE_H
#ifndef WIRE_H
#include <Wire.h>
#endif

//Hardware constants
#define BATTERY_IN A0
#define PIEZO   4    //piezzo, pwm output
#define BTN1    37   //button 1, active high
#define BTN2    39   //button 2, active high
#define BTN3    41  //button 3, active high
#define BTN4	43  //button 4, active high
#define EEPROM 0x50    //Address of 24LC256 eeprom chip
#define EEPROM_TEST_VALUE 0xFF
// The SparkFun breakout board defaults to 1, set to 0 if SA0 jumper on the bottom of the board is set
#define MMA8452_ADDRESS 0x1D  // 0x1D if SA0 is high, 0x1C if low
#define SCALE 4
#define DATARATE 0

//Define a few of the registers that we will be accessing on the MMA8452
#define OUT_X_MSB		0x01
#define XYZ_DATA_CFG	0x0E
#define WHO_AM_I		0x0D
#define CTRL_REG1		0x2A

#define GSCALE 2 // Sets full-scale range to +/-2, 4, or 8g. Used to calc real g values.

union
{
	byte b[4];
	unsigned int ui;
} data;

extern boolean EEPROM_available, RTC_available, ACCEL_available;
extern float xcal, ycal, zcal, prevAcc;

//function prototypes
void beep(uint8_t duration);
void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data);
unsigned int readEEPROM(int deviceaddress, unsigned int eeaddress);
void readAccelData(int *destination);
void initMMA8452(byte fsr, byte dataRate);
int initEEPROM(void);
void MMA8452Standby();
void MMA8452Active();
void readRegisters(byte addressToRead, int bytesToRead, byte * dest);
byte readRegister(byte addressToRead);
void writeRegister(byte addressToWrite, byte dataToWrite);
unsigned int readUint(unsigned int addr);
void writeUint(unsigned int addr, unsigned int x);
int UpdateAccel(void);
int tapHandler(void);
void setSteps(int correction);
void portraitLandscapeHandler();
void CalibrateAccelerometer(void);

#endif