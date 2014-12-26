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
#include <TimerFreeTone\TimerFreeTone.h>

//globals
TFT_S6D02A1 tft = TFT_S6D02A1(TFT_CS, TFT_DC, TFT_RST);
int steps = 0;
int battery_level = 100;
int      bmpWidth, bmpHeight;   // W+H in pixels

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
	PlayScale();
  }
	if (digitalRead(BTN3)) {		//draws BMP
		analogWrite(PIEZO, HIGH);
		bmpDraw("bidoof.bmp", 50, 60);
		delay(200);
		EraseBMP(50, 60);
		bmpDraw("bidoof.bmp", 50, 70);
		delay(200);
		EraseBMP(50, 70);
		bmpDraw("bidoof.bmp", 50, 80);
		delay(200);
		analogWrite(PIEZO, LOW);
	}

}

#define BUFFPIXEL 80

void bmpDraw(char *filename, uint8_t x, uint8_t y) {

	File     bmpFile;
	uint8_t  bmpDepth;              // Bit depth (currently must be 24)
	uint32_t bmpImageoffset;        // Start of image data in file
	uint32_t rowSize;               // Not always = bmpWidth; may have padding
	uint8_t  sdbuffer[3 * BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
	uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
	boolean  goodBmp = false;       // Set to true on valid header parse
	boolean  flip = true;			// BMP is stored bottom-to-top
	int      w, h, row, col;
	uint8_t  r, g, b;
	uint32_t pos = 0, startTime = millis();


	if ((x >= tft.width()) || (y >= tft.height())) return;


	// Open requested file on SD card
	if ((bmpFile = SD.open(filename)) == NULL) {
		return;
	}

	// Parse BMP header
	if (read16(bmpFile) == 0x4D42) { // BMP signature
		read32(bmpFile);
		(void)read32(bmpFile); // Read & ignore creator bytes
		bmpImageoffset = read32(bmpFile); // Start of image data
		// Read DIB header
		read32(bmpFile);
		bmpWidth = read32(bmpFile);
		bmpHeight = read32(bmpFile);
		if (read16(bmpFile) == 1) { // # planes -- must be '1'
			bmpDepth = read16(bmpFile); // bits per pixel
			if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

				goodBmp = true; // Supported BMP format -- proceed!

				// BMP rows are padded (if needed) to 4-byte boundary
				rowSize = (bmpWidth * 3 + 3) & ~3;

				// Crop area to be loaded
				w = bmpWidth;
				h = bmpHeight;
				if ((x + w - 1) >= tft.width())  w = tft.width() - x;
				if ((y + h - 1) >= tft.height()) h = tft.height() - y;

				// Set TFT address window to clipped image bounds
				tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

				for (row = 0; row<h; row++) { // For each scanline...

					// Seek to start of scan line.  It might seem labor-
					// intensive to be doing this on every line, but this
					// method covers a lot of gritty details like cropping
					// and scanline padding.  Also, the seek only takes
					// place if the file position actually needs to change
					// (avoids a lot of cluster math in SD library).
					if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
						pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
					else     // Bitmap is stored top-to-bottom
						pos = bmpImageoffset + row * rowSize;
					if (bmpFile.position() != pos) { // Need seek?
						bmpFile.seek(pos);
						buffidx = sizeof(sdbuffer); // Force buffer reload
					}

					for (col = 0; col<w; col++) { // For each pixel...
						// Time to read more pixel data?
						if (buffidx >= sizeof(sdbuffer)) { // Indeed
							bmpFile.read(sdbuffer, sizeof(sdbuffer));
							buffidx = 0; // Set index to beginning
						}

						// Convert pixel from BMP to TFT format, push to display
						b = sdbuffer[buffidx++];
						g = sdbuffer[buffidx++];
						r = sdbuffer[buffidx++];
						tft.pushColor(tft.Color565(r, g, b));
					} // end pixel
				} // end scanline
			} // end goodBmp
		}
	}

	bmpFile.close();
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
	uint16_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read(); // MSB
	return result;
}

uint32_t read32(File f) {
	uint32_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read();
	((uint8_t *)&result)[2] = f.read();
	((uint8_t *)&result)[3] = f.read(); // MSB
	return result;
}


void PlayScale(void){
	int melody[] = { 262, 196, 196, 220, 196, 0, 247, 262 };
	int duration[] = { 250, 125, 125, 250, 250, 250, 250, 250 };

	for (int thisNote = 0; thisNote < 8; thisNote++) { // Loop through the notes in the array.
		TimerFreeTone(PIEZO, melody[thisNote], duration[thisNote]); // Play thisNote for duration.
		delay(50); // Short delay between notes.
	}
}




