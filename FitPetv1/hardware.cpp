#include "hardware.h"

void writeEEPROM(int deviceaddress, unsigned int eeaddress, char data)
{	/// <summary>
	/// Writes byte to EEPROM
	/// </summary>
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8));   // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.write(data);
	Wire.endTransmission();

	delay(5);
}

unsigned char readEEPROM(int deviceaddress, unsigned int eeaddress)
{	/// <summary>
	/// Reads byte from EEPROM
	/// </summary>
	unsigned char rdata;

	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8));   // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.endTransmission();

	Wire.requestFrom(deviceaddress, 1);

	if (Wire.available()) rdata = Wire.read();

	return rdata;
}


