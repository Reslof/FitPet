#include <Wire\Wire.h>

#define PIEZO   3   //piezzo, pwm output
#define BTN1    4   //button 1, active high
#define BTN2    5   //button 2, active high
#define BTN3    22  //button 2, active high

#define EEPROM 0x50    //Address of 24LC256 eeprom chip

//function prototypes
void writeEEPROM(int deviceaddress, unsigned int eeaddress, char data);
unsigned char readEEPROM(int deviceaddress, unsigned int eeaddress);
void bmpDraw(char *filename, uint8_t x, uint8_t y);