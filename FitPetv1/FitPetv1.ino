/*
*FitPet v0.3: Holiday Edition
 
* TODO: 
 -Integrate Accelerometer, RTC, Bluetooth
 -Write Menus
 -Decide on memory locations on EEPROM for steps and other status

 More to be added.

*HISTORY:
12/26/14 0.3
Added comments to code. Integrated ClearBMP.
*/

#include <Adafruit_GFX\Adafruit_GFX.h>
#include <TFT_S6D02A1\TFT_S6D02A1.h>
#include <SPI.h>
#include <SD.h>
#include <Wire\Wire.h>
#include "gui.h"
#include "hardware.h"
#include "bigbidoof.c"

//globals
TFT_S6D02A1 tft = TFT_S6D02A1(TFT_CS, TFT_DC, TFT_RST);
int steps = 0;
int battery_level = 100;

void setup(void) {
	unsigned int address = 0;

	SPI.setClockDivider(SPI_CLOCK_DIV2); //sets SPI clock to 24 MHz, fastest possible
	pinMode(BTN1, INPUT); //set the pinmodes for buttons
	pinMode(BTN2, INPUT);
	pinMode(BTN3, INPUT);

	Serial.begin(9600); //initializes serial connection 
	Wire.begin();		//initializes I2C bus
	tft.init();			//initializes TFT

	writeEEPROM(EEPROM, address, 0xFF); //writes test value on EEPROM
	

  if (!initGUI()){
	  DebugMessage("GUI init: OK");
  }
  else{
	  DebugMessage("GUI init: FAILED");
	  Serial.write("GUI init failed");
  }

  if (!SD.begin(SD_CS)) {
	  DebugMessage("SD init: FAILED");
	  SPI.setClockDivider(SPI_CLOCK_DIV2); //required to speed up TFT SPI clock
	  return;
  }
  else{
	  DebugMessage("SD init: OK");
  }

  unsigned char EEPROMtest = readEEPROM(EEPROM, address); //reads EEPROM value

  if (EEPROMtest == 0xFF){
	  DebugMessage("EEPROM init: OK");
  }
  else{
	  DebugMessage("EEPROM init: FAILED");
  }
  /*
  unsigned char value = readEEPROM(EEPROM, 0);
	DebugMessage("Value Read EEPROM: ");
	PrintVariable(value, HEX);
	unsigned char letterA = 'A';
	writeEEPROM(EEPROM, address, letterA);
	unsigned char valueA = readEEPROM(EEPROM, 0);
	DebugMessage("Printing letter A:");
	PrintVariable(valueA, ASCII); //prints actual ascii letter!
	*/
  
}

void loop() {
	if (battery_level < 0) {
		battery_level = 100;
	}
	UpdateSteps(steps++);   //Updates steps UI
	UpdateBattery(battery_level--); //Updates battery UI
	UpdateClock();

	if (digitalRead(BTN1)) {		//prints line to test 
	analogWrite(PIEZO, HIGH);
	DebugMessage("This is a line");
	analogWrite(PIEZO, LOW);
	}
	if (digitalRead(BTN2)) {		//clears screen
	ClearMainScreen();
	//PlayScale();
  }
	if (digitalRead(BTN3)) {		//draws BMP
		analogWrite(PIEZO, HIGH);
		DrawSprite(bigbidoof, 30, 30);

		//ClearMainScreen();
		analogWrite(PIEZO, LOW);
	}

}


void DrawSprite(const tImage sprite, uint8_t x, uint8_t y) {

	int      w, h, pixel, col, imagePixels;

	if ((x >= tft.width()) || (y >= tft.height())) return; //cancels operation if image larger than screen size

	// Crop area to be loaded
	w = sprite.width; 
	h = sprite.height;
	
	if ((x + w - 1) >= tft.width())  w = tft.width() - x;
	if ((y + h - 1) >= tft.height()) h = tft.height() - y;
	imagePixels = w * h;
	// Set TFT address window to clipped image bounds
	tft.setAddrWindow(x, y, x + w - 1, y + h - 1); //a bit smaller than image


	for (pixel = 0; pixel < imagePixels; pixel++) {

		tft.pushColor(sprite.data[pixel]);

	}
}

