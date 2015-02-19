/*
*FitPet v0.6: Back to School Edition
 
* TODO:
- Integrate Accelerometer, RTC
- Write menus and create UI (Eddy)
- Decide on memory locations on EEPROM for steps and other status
- Integrate second Bluetooth module
- Enable inter-pet communication

 More to be added.

*HISTORY:
1/14/2015
-Integrated RTC Module. Needs RTCdue library.
-Fixed bug with EEPROM + RTC on same bus. Accelerometer will now be placed on the Wire bus instead of Wire1.

1/12/2014 0.5
- Integrated Bluetooth Serial Module. Works great from smartphone, not so much via PC.
- Updated README.

1/3/2015 0.4
- Added more comments to code. Finally implemented drawing sprites from Flash memory. **Massive speed increases.Smooth animations now possible.**
- Created loading and sample pet animations. Working on tools to automate animation creation.
- Integrated EEPROM and wrote driver code for it.

12/26/14 0.3
- **Refactored entire code base. Project now requires Visual Studio 2013, Arduino 1.5.x and the Visual Micro plugin.**
- Added comments to code.
- Integrated ClearBMP and many other library functions.
*/

#include <Adafruit_GFX\Adafruit_GFX.h>
#include <TFT_S6D02A1\TFT_S6D02A1.h>
#include <SPI.h>
#include <Wire.h>
#include <RTCdue\RTCdue.h>
#include "gui.h"
#include "hardware.h"

//globals
TFT_S6D02A1 tft = TFT_S6D02A1(TFT_CS, TFT_DC);
RTC_DS1307 rtc;

//int steps = 0;
int battery_level = 100;
volatile int animateLoadingFlag = 0;
volatile int menuFlag = 0;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup(void) {

	Serial.begin(57600);
	Serial.println("Bluetooth ON");

	Wire.begin();		//initializes I2C bus
	Wire1.begin();		//initializes I2C bus
	rtc.begin();		//initializes RTC

	tft.init();			//initializes TFT

	// reserve 200 bytes for the inputString:
	inputString.reserve(200);

	unsigned int address = 0;

	SPI.setClockDivider(SPI_CLOCK_DIV2); //sets SPI clock to 24 MHz, fastest possible
	pinMode(BTN1, INPUT); //set the pinmodes for buttons
	pinMode(BTN2, INPUT);
	pinMode(BTN3, INPUT);
	pinMode(BTN4, INPUT);
	pinMode(PIEZO, OUTPUT);

	writeEEPROM(EEPROM, address, 0xFF); //writes test value on EEPROM	


  if (!initGUI()){
	  DebugMessage("GUI init: OK");
  }
  else{
	  Serial.write("GUI init failed");
  }
  
  attachInterrupt(BTN3, setAnimateLoadingFlag, FALLING); //creates flags on falling edge to minimize bouncing
  attachInterrupt(BTN4, setMenuFlag, FALLING);

  
  
  unsigned char EEPROMtest = readEEPROM(EEPROM, address); //reads EEPROM value

  if (EEPROMtest == 0xFF){
	  DebugMessage("EEPROM init: OK");
  }
  else{
	  DebugMessage("EEPROM init: FAILED");
  }
  
  /* Some old test code on how to use EEPROM
  unsigned char value = readEEPROM(EEPROM, 0);
	DebugMessage("Value Read EEPROM: ");
	PrintVariable(value, HEX);
	unsigned char letterA = 'A';
	writeEEPROM(EEPROM, address, letterA);
	unsigned char valueA = readEEPROM(EEPROM, 0);
	DebugMessage("Printing letter A:");
	PrintVariable(valueA, ASCII); //prints actual ascii letter!
	*/
  rtc.adjust(DateTime(__DATE__, __TIME__));
  if (!rtc.isrunning()) {
	  Serial.println("RTC is NOT running!");
	  delay(2000);
	  // following line sets the RTC to the date & time this sketch was compiled
	  rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  
  initMMA8452(); //Test and intialize the MMA8452

}

void loop() {

	double unitVector = 0;
	double previousValue = 0;
	int accelCount[3];  // Stores the 12-bit signed value
	readAccelData(accelCount);  // Read the x/y/z adc values

	// Now we'll calculate the accleration value into actual g's
	float accelG[3];  // Stores the real accel value in g's
	for (int i = 0; i < 3; i++)
	{
		accelG[i] = (float)accelCount[i] / ((1 << 12) / (2 * GSCALE));  // get actual g value, this depends on scale being set
	}

	// Print out values
	for (int i = 0; i < 3; i++)
	{	
		Serial.print(accelG[i], 4);  // Print g values
		Serial.print("\t");  // tabs in between axes

	}

	Serial.print("Unit Vector: ");
	unitVector = sqrt(accelG[0] * accelG[0] + accelG[1] * accelG[1] + accelG[2] * accelG[2]);
	if (unitVector > (previousValue + 0.40)){
		UpdateSteps();
		
	}

	Serial.print(unitVector);
	previousValue = unitVector;
	Serial.println();

	delay(100);  // Delay here for visibility




	if (battery_level < 0) {
		battery_level = 100;
	}
	if (animateLoadingFlag){
		analogWrite(PIEZO, HIGH);
		LoadingScreenIcon();
		analogWrite(PIEZO, LOW);
	}
	if (menuFlag){
		//analogWrite(PIEZO, HIGH);
		displayMenu();
		//analogWrite(PIEZO, LOW);
	}

	//UpdateSteps(steps++);   //Updates steps UI
	UpdateBattery(battery_level--); //Updates battery UI
	UpdateClock();					//Updates clock

	if (digitalRead(BTN1)) {		//prints line to test 
	analogWrite(PIEZO, HIGH);
	DebugMessage("This is a line");
	analogWrite(PIEZO, LOW);
	}
	if (digitalRead(BTN2)) {		//clears screen
	ClearMainScreen();

	 }
	if (stringComplete) {		//handles Bluetooth commands!!!! 
		String clear = "clr\n";
		String read = "read\n";
		String loading = "loading\n";
		String showPet = "pet\n";

		DebugMessage(inputString);
		if (inputString.equalsIgnoreCase(clear)){
			ClearMainScreen();
		}
		if (inputString.equalsIgnoreCase(loading)){
			animateLoadingFlag = !animateLoadingFlag;
		}
		if (inputString.equalsIgnoreCase(showPet)){
			menuFlag = !menuFlag;
		}
		if (inputString.equalsIgnoreCase(clear)){
			ClearMainScreen();
		}
		if (inputString.equalsIgnoreCase(read)){

			unsigned char EEPROMtest = readEEPROM(EEPROM, 0); //reads EEPROM value
			PrintVariable(EEPROMtest, DEC);
		}
		// clear the string:
		inputString = "";
		stringComplete = false;
	}

}


void serialEvent() {
	while (Serial.available()) {
		// get the new byte:
		char inChar = (char)Serial.read();
		// add it to the inputString:
		inputString += inChar;
		// if the incoming character is a newline, set a flag
		// so the main loop can do something about it:
		if (inChar == '\n') {
			stringComplete = true;
		}
	}
}


void setAnimateLoadingFlag(void){
	//ISR for BTN3
	animateLoadingFlag = !animateLoadingFlag;
}
void setMenuFlag(void){

	menuFlag = !menuFlag;

	if (menuFlag == 0){
		ClearMainScreen();
	}


	displayMenu();
	

}


void readAccelData(int *destination)
{
	byte rawData[6];  // x/y/z accel register data stored here

	readRegisters(OUT_X_MSB, 6, rawData);  // Read the six raw data registers into data array

	// Loop to calculate 12-bit ADC and g value for each axis
	for (int i = 0; i < 3; i++)
	{
		int gCount = (rawData[i * 2] << 8) | rawData[(i * 2) + 1];  //Combine the two 8 bit registers into one 12-bit number
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
		Serial.println(c, HEX);
		DebugMessage("Accel init: FAILED");
		delay(5000); // Loop forever if communication doesn't happen
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
	Wire1.beginTransmission(MMA8452_ADDRESS);
	Wire1.write(addressToRead);
	Wire1.endTransmission(false); //endTransmission but keep the connection active

	Wire1.requestFrom(MMA8452_ADDRESS, bytesToRead); //Ask for bytes, once done, bus is released by default

	while (Wire1.available() < bytesToRead); //Hang out until we get the # of bytes we expect

	for (int x = 0; x < bytesToRead; x++)
		dest[x] = Wire1.read();
}

// Read a single byte from addressToRead and return it as a byte
byte readRegister(byte addressToRead)
{
	Wire1.beginTransmission(MMA8452_ADDRESS);
	Wire1.write(addressToRead);
	Wire1.endTransmission(false); //endTransmission but keep the connection active

	Wire1.requestFrom(MMA8452_ADDRESS, 1); //Ask for 1 byte, once done, bus is released by default

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