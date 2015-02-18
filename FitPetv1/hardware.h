#include <Wire.h>

//Hardware constants
#define PIEZO   3   //piezzo, pwm output
#define BTN1    4   //button 1, active high
#define BTN2    5   //button 2, active high
#define BTN3    22  //button 3, active high
#define BTN4	24  //button 4, active high
#define EEPROM 0x50    //Address of 24LC256 eeprom chip

// The SparkFun breakout board defaults to 1, set to 0 if SA0 jumper on the bottom of the board is set
#define MMA8452_ADDRESS 0x1D  // 0x1D if SA0 is high, 0x1C if low

//Define a few of the registers that we will be accessing on the MMA8452
#define OUT_X_MSB		0x01
#define XYZ_DATA_CFG	0x0E
#define WHO_AM_I		0x0D
#define CTRL_REG1		0x2A

#define GSCALE 2 // Sets full-scale range to +/-2, 4, or 8g. Used to calc real g values.

//function prototypes
void writeEEPROM(int deviceaddress, unsigned int eeaddress, char data);
unsigned char readEEPROM(int deviceaddress, unsigned int eeaddress);