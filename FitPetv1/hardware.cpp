#include "hardware.h"
#include "gui.h"

void beep(uint8_t duration){
	analogWrite(PIEZO, 150);
	delay(duration);
	analogWrite(PIEZO, 0);
	delay(duration);
}

int initEEPROM(void){
	int status = 1;
	unsigned int address = 0;
	writeEEPROM(EEPROM, address, 0xFF); //writes test value on EEPROM	
	unsigned int EEPROMtest = readEEPROM(EEPROM, address); //reads EEPROM value

	if (EEPROMtest == 0xFF){
		status = 0;
	}
	Serial.println("EEPROM Test Value:");
	Serial.println(EEPROMtest, HEX);

	return status;
}

void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data)
{	/// <summary>
	/// Writes byte to EEPROM
	/// </summary>
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8));   // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.write(data);
	Wire.endTransmission();

}

unsigned int readEEPROM(int deviceaddress, unsigned int eeaddress)
{	/// <summary>
	/// Reads byte from EEPROM
	/// </summary>
	byte rdata;

	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8));   // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.endTransmission();

	Wire.requestFrom(deviceaddress, 1);

	if (Wire.available()) rdata = Wire.read();

	return rdata;
}

unsigned int readUint(unsigned int addr)
{
	for (int i = 0; i < 2; i++)
	{
		data.b[i] = readEEPROM(EEPROM, addr + i);
	}
	return data.ui;
}
void writeUint(unsigned int addr, unsigned int x)
{

	data.ui = x;

	for (int i = 0; i < 2; i++)
	{
		writeEEPROM(EEPROM, addr + i, data.b[i]);

	}
}


void readAccelData(int *destination)
{
	byte rawData[6];  // x/y/z accel register data stored here

	readRegisters(OUT_X_MSB, 6, rawData);  // Read the six raw data registers into data array

	// Loop to calculate 12-bit ADC and g value for each axis
	for (int i = 0; i < 3; i++)
	{
		int gCount = (rawData[i * 2] << 8) | rawData[(i * 2) + 1]; //Combine the two 8 bit registers into one 12-bit number
		gCount >>= 4; //The registers are left align, here we right align the 12-bit integer

		// If the number is negative, we have to make it so manually (no 12-bit data type)
		if (rawData[i * 2] > 0x7F)
		{
			gCount -= 0x1000;
		}

		destination[i] = gCount; //Record this gCount into the 3 int array
	}
}

// Initialize the MMA8452 registers
// See the many application notes for more info on setting all of these registers:
// http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=MMA8452Q
void initMMA8452(byte fsr, byte dataRate)
{
	// Read the WHO_AM_I register, this is a good test of communication
	byte c = readRegister(0x0D);  // Read WHO_AM_I register
	if (c == 0x2A) // WHO_AM_I should always be 0x2A
	{
		Serial.println("MMA8452Q is online.");
		DebugMessage("Accel init: OK");
	}
	else
	{
		Serial.print("Could not connect to MMA8452Q: 0x");
		Serial.println(c, HEX);
		DebugMessage("Accel init : FAILED");
	}

	MMA8452Standby();  // Must be in standby to change registers

	// Set up the full scale range to 2, 4, or 8g.
	if ((fsr == 2) || (fsr == 4) || (fsr == 8))
		writeRegister(0x0E, fsr >> 2);
	else
		writeRegister(0x0E, 0);

	// Setup the 3 data rate bits, from 0 to 7
	writeRegister(0x2A, readRegister(0x2A) & ~(0x38));
	if (dataRate <= 7)
		writeRegister(0x2A, readRegister(0x2A) | (dataRate << 3));

	// Set up portrait/landscap registers - 4 steps:
	// 1. Enable P/L
	// 2. Set the back/front angle trigger points (z-lock)
	// 3. Set the threshold/hysteresis angle
	// 4. Set the debouce rate
	// For more info check out this app note: http://cache.freescale.com/files/sensors/doc/app_note/AN4068.pdf
	writeRegister(0x11, 0x40);  // 1. Enable P/L
	writeRegister(0x13, 0x44);  // 2. 29deg z-lock (don't think this register is actually writable)
	writeRegister(0x14, 0x84);  // 3. 45deg thresh, 14deg hyst (don't think this register is writable either)
	writeRegister(0x12, 0x50);  // 4. debounce counter at 100ms (at 800 hz)

	/* Set up single and double tap - 5 steps:
	1. Set up single and/or double tap detection on each axis individually.
	2. Set the threshold - minimum required acceleration to cause a tap.
	3. Set the time limit - the maximum time that a tap can be above the threshold
	4. Set the pulse latency - the minimum required time between one pulse and the next
	5. Set the second pulse window - maximum allowed time between end of latency and start of second pulse
	for more info check out this app note: http://cache.freescale.com/files/sensors/doc/app_note/AN4072.pdf */
	writeRegister(0x21, 0x7F);  // 1. enable single/double taps on all axes
	// writeRegister(0x21, 0x55);  // 1. single taps only on all axes
	// writeRegister(0x21, 0x6A);  // 1. double taps only on all axes
	writeRegister(0x23, 0x20);  // 2. x thresh at 2g, multiply the value by 0.0625g/LSB to get the threshold
	writeRegister(0x24, 0x20);  // 2. y thresh at 2g, multiply the value by 0.0625g/LSB to get the threshold
	writeRegister(0x25, 0x08);  // 2. z thresh at .5g, multiply the value by 0.0625g/LSB to get the threshold
	writeRegister(0x26, 0x30);  // 3. 30ms time limit at 800Hz odr, this is very dependent on data rate, see the app note
	writeRegister(0x27, 0xA0);  // 4. 200ms (at 800Hz odr) between taps min, this also depends on the data rate
	writeRegister(0x28, 0xFF);  // 5. 318ms (max value) between taps max

	// Set up interrupt 1 and 2
	writeRegister(0x2C, 0x02);  // Active high, push-pull interrupts
	writeRegister(0x2D, 0x19);  // DRDY, P/L and tap ints enabled
	writeRegister(0x2E, 0x01);  // DRDY on INT1, P/L and taps on INT2

	MMA8452Active();  // Set to active to start reading
}

// Sets the MMA8452 to standby mode. It must be in standby to change most register settings
void MMA8452Standby()
{
	byte c = readRegister(CTRL_REG1);
	writeRegister(CTRL_REG1, c & ~(0x01)); //Clear the active bit to go into standby
}

// Sets the MMA8452 to active mode. Needs to be in this mode to output data
void MMA8452Active()
{
	byte c = readRegister(CTRL_REG1);
	writeRegister(CTRL_REG1, c | 0x01); //Set the active bit to begin detection
}

// Read bytesToRead sequentially, starting at addressToRead into the dest byte array
void readRegisters(byte addressToRead, int bytesToRead, byte * dest)
{
	Wire1.requestFrom(MMA8452_ADDRESS, bytesToRead, addressToRead, 1);

	while (Wire1.available() < bytesToRead);

	for (int i = 0; i < bytesToRead; i++)
		dest[i] = Wire1.read();
}

// Read a single byte from addressToRead and return it as a byte
byte readRegister(byte addressToRead)
{
	Wire1.requestFrom(MMA8452_ADDRESS, 1, addressToRead, 1);

	while (!Wire1.available()); //Wait for the data to come back
	return Wire1.read(); //Return this one byte
}

// Writes a single byte (dataToWrite) into addressToWrite
void writeRegister(byte addressToWrite, byte dataToWrite)
{
	Wire1.beginTransmission(MMA8452_ADDRESS);
	Wire1.write(addressToWrite);
	Wire1.write(dataToWrite);
	Wire1.endTransmission(); //Stop transmitting
}

// This function will read the status of the tap source register.
// Print if there's been a single or double tap, and on what axis.
int tapHandler()
{
	int tap;

	byte source = readRegister(0x22);  // Reads the PULSE_SRC register

	if ((source & 0x40) == 0x40)  // If AxZ bit is set
	{
		if ((source & 0x08) == 0x08){  // If DPE (double puls) bit is set
			tap = 2; //double tap
			Serial.print("Double (2) tap on Z");
		}
		else{
			tap = 1; //double tap
			Serial.print("Single (1) tap on Z");
		}
	}
	return tap;
}
// This function will read the p/l source register and
// print what direction the sensor is now facing
void portraitLandscapeHandler()
{
	byte pl = readRegister(0x10);  // Reads the PL_STATUS register
	switch ((pl & 0x06) >> 1)  // Check on the LAPO[1:0] bits
	{
	case 0:
		Serial.print("Portrait up, ");
		break;
	case 1:
		Serial.print("Portrait Down, ");
		break;
	case 2:
		Serial.print("Landscape Right, ");
		break;
	case 3:
		Serial.print("Landscape Left, ");
		break;
	}
	if (pl & 0x01)  // Check the BAFRO bit
		Serial.print("Back");
	else
		Serial.print("Front");
	if (pl & 0x40)  // Check the LO bit
		Serial.print(", Z-tilt!");
	Serial.println();
}

void setSteps(int correction){
	stepsTaken = readUint(32);
	stepsTaken + correction;
}