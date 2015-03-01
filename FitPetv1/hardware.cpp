#include "hardware.h"
#include "gui.h"

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
void initMMA8452()
{
	byte c = readRegister(WHO_AM_I);  // Read WHO_AM_I register
	if (c == 0x2A) // WHO_AM_I should always be 0x2A
	{
		Serial.println("MMA8452Q is online...");
		DebugMessage("Accel init: OK");
	}
	else
	{
		Serial.print("Could not connect to MMA8452Q: 0x");
		DebugMessage("Accel: FAILED");
		Serial.println(c, HEX);
		while (1); // Loop forever if communication doesn't happen
	}

	MMA8452Standby();  // Must be in standby to change registers

	// Set up the full scale range to 2, 4, or 8g.
	byte fsr = GSCALE;
	if (fsr > 8) fsr = 8; //Easy error check
	fsr >>= 2; // Neat trick, see page 22. 00 = 2G, 01 = 4A, 10 = 8G
	writeRegister(XYZ_DATA_CFG, fsr);

	//The default data rate is 800Hz and we don't modify it in this example code

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

void UpdateAccel(void){
	int accelCount[3];  // Stores the 12-bit signed value
	readAccelData(accelCount);  // Read the x/y/z adc values

	// Now we'll calculate the accleration value into actual g's
	float accelG[3];  // Stores the real accel value in g's
	for (int i = 0; i < 3; i++)
	{
		accelG[i] = (float)accelCount[i] / ((1 << 12) / (2 * GSCALE)); // get actual g value, this depends on scale being set
	}

	// Print out values
	for (int i = 0; i < 3; i++)
	{
		Serial.print(accelG[i], 4);  // Print g values
		Serial.print("\t");  // tabs in between axes
	}
	Serial.println();

	delay(100);  // Delay here for visibility
}