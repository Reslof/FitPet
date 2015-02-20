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

#include <Adafruit_GFX_Library\Adafruit_GFX.h>
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

	UpdateAccel();


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

