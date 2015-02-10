#include "gui.h"
#include "hardware.h"
#include <stdint.h>

uint8_t hh, mm, ss; // Get H, M, S from compile time
uint8_t previousLine = 0;  //global to keep track of DebugMessage line
uint32_t stepsTaken = 0;

void AnimatePet(void){
	DrawSprite(bidoof_frame_000, MIDDLE_MAIN_SCREEN_WIDTH - 20, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(bidoof_frame_001, MIDDLE_MAIN_SCREEN_WIDTH - 20, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(bidoof_frame_002, MIDDLE_MAIN_SCREEN_WIDTH - 20, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(bidoof_frame_003, MIDDLE_MAIN_SCREEN_WIDTH - 20, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(bidoof_frame_004, MIDDLE_MAIN_SCREEN_WIDTH - 20, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(bidoof_frame_005, MIDDLE_MAIN_SCREEN_WIDTH - 20, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(bidoof_frame_006, MIDDLE_MAIN_SCREEN_WIDTH - 20, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(bidoof_frame_007, MIDDLE_MAIN_SCREEN_WIDTH - 20, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(bidoof_frame_008, MIDDLE_MAIN_SCREEN_WIDTH - 20, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(bidoof_frame_009, MIDDLE_MAIN_SCREEN_WIDTH - 20, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(bidoof_frame_010, MIDDLE_MAIN_SCREEN_WIDTH - 20, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(bidoof_frame_011, MIDDLE_MAIN_SCREEN_WIDTH - 20, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(bidoof_frame_012, MIDDLE_MAIN_SCREEN_WIDTH - 20, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
}

void UpdateBattery(int BATTERY_LEVEL) {
	/// <summary>
	/// Prints battery level to GUI
	/// </summary>
	tft.setCursor(68, 15); //sets cursor in right place
	if (BATTERY_LEVEL < 100) {
		tft.print(" ");
	}

	tft.print(BATTERY_LEVEL);
	tft.print("%");
}
void UpdateClock(void) {
	/// <summary>
	/// Takes care of the clock for now. Will most likely be removed once RTC is implemented
	/// </summary>

	DateTime now = rtc.now(); //Gets time from RTC
	char *daynightFlag;

	// Update digital time
	tft.setCursor(34, 138);

	if (now.hour() > 13){
		tft.print(now.hour() - 12, DEC);
		daynightFlag = " pm";
	}
	else{
		tft.print(now.hour(), DEC);
		daynightFlag = " am";
	}
	
	tft.print(':');
	tft.print(now.minute(), DEC);
	tft.print(':');
	
	if (now.second() < 10){
		tft.print("0");
	}

	tft.print(now.second(), DEC);
	tft.print(daynightFlag);

	// Update date
	tft.setCursor(34, 150);

	tft.print(now.month(), DEC);
	tft.print('/');
	tft.print(now.day(), DEC);
	tft.print('/');
	tft.print(now.year(), DEC);
}

void UpdateSteps(void) {
	/// <summary>
	/// Prints steps_taken to GUI.
	/// </summary>
	//probably need to add code to fetch value from EEPROM once its integrated
	tft.setCursor(3, 15);
	tft.print(stepsTaken++);
}

void DebugMessage(char *message){
	/// <summary>
	/// Prints text to screen. Automatically wraps around main box
	/// </summary>
	tft.setCursor(0, (27 + previousLine));
	tft.println(message);
	previousLine += 10;
	if (previousLine > 100){ //if more than 10 lines printed, start at top again
		previousLine = 0;
	}
}

void DebugMessage(String message){
	/// <summary>
	/// Prints text to screen. Automatically wraps around main box
	/// </summary>
	tft.setCursor(0, (27 + previousLine));
	tft.println(message);
	previousLine += 10;
	if (previousLine > 100){ //if more than 10 lines printed, start at top again
		previousLine = 0;
	}
}
void DrawSprite(const tImage sprite, uint8_t x, uint8_t y) {
	/// <summary>
	/// Draws a sprite kept in c file. Sprite must be in RGB565 format using LCD Image Converter from http://code.google.com/p/lcd-image-converter/
	/// </summary>

	//NOTE: Image appears messed up if width or height do not fit on screen. Does not crop automatically.
	int w, h, pixel, col, imagePixels;

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
		//draws each pixel
		tft.pushColor(sprite.data[pixel]);
	}
}
void PrintVariable(unsigned char variable, int representation){
	/// <summary>
	/// Prints the ASCII code or HEX/DEC value of variable
	/// </summary>
	
	tft.setCursor(0, (27 + previousLine));
	if (representation == ASCII){
		tft.println(char(variable));
	}else{
		tft.println(variable, representation);
	}
	previousLine += 10;
	if (previousLine > 100){ //if more than 10 lines printed, start at top again
		previousLine = 0;
	}
}
void ClearMainScreen(void){
	/// <summary>
	/// Clears main GUI screen
	/// </summary>
	tft.fillRect(0, 26, MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT, S6D02A1_BLACK);
	previousLine = 0; //resets line count to print at top again
}

void LoadingScreenCircles(void){
	/// <summary>
	/// Displays animated circles in middle of screen
	/// </summary>
	for (int i = 0; i < 10; i++) {
		tft.drawCircle(MIDDLE_MAIN_SCREEN_WIDTH - 20, MIDDLE_OF_SCREEN_HEIGHT, i, S6D02A1_BLUE);
		//delay(40);
	}
	delay(200);
	for (int i = 10; i > 4; i--) {
		tft.drawCircle(MIDDLE_MAIN_SCREEN_WIDTH - 20, MIDDLE_OF_SCREEN_HEIGHT, i, S6D02A1_BLACK);
		//delay(40);
	}
	delay(200);
	for (int i = 0; i < 10; i++) {
		tft.drawCircle(MIDDLE_MAIN_SCREEN_WIDTH, MIDDLE_OF_SCREEN_HEIGHT, i, S6D02A1_BLUE);
		//delay(40);
	}
	delay(200);
	for (int i = 10; i > 4; i--) {
		tft.drawCircle(MIDDLE_MAIN_SCREEN_WIDTH, MIDDLE_OF_SCREEN_HEIGHT, i, S6D02A1_BLACK);
		//delay(40);
	}
	delay(200);
	for (int i = 0; i < 10; i++) {
		tft.drawCircle(MIDDLE_MAIN_SCREEN_WIDTH + 20, MIDDLE_OF_SCREEN_HEIGHT, i, S6D02A1_BLUE);
		//delay(40);
	}
	delay(200);
	for (int i = 10; i > 4; i--) {
		tft.drawCircle(MIDDLE_MAIN_SCREEN_WIDTH + 20, MIDDLE_OF_SCREEN_HEIGHT, i, S6D02A1_BLACK);
		//delay(40);
	}
	delay(200);
}
void LoadingScreenIcon(void){
	/// <summary>
	/// Displays animated icon in the middle of the screen
	/// </summary>
	
	DrawSprite(loading_frame_000, MIDDLE_MAIN_SCREEN_WIDTH - 25, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(loading_frame_001, MIDDLE_MAIN_SCREEN_WIDTH - 25, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(loading_frame_002, MIDDLE_MAIN_SCREEN_WIDTH - 25, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(loading_frame_003, MIDDLE_MAIN_SCREEN_WIDTH - 25, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(loading_frame_004, MIDDLE_MAIN_SCREEN_WIDTH - 25, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(loading_frame_005, MIDDLE_MAIN_SCREEN_WIDTH - 225, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(loading_frame_006, MIDDLE_MAIN_SCREEN_WIDTH - 25, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(loading_frame_007, MIDDLE_MAIN_SCREEN_WIDTH - 25, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
	DrawSprite(loading_frame_008, MIDDLE_MAIN_SCREEN_WIDTH - 25, MIDDLE_MAIN_SCREEN_HEIGHT);
	delay(15);
}
int initGUI(void){
	/// <summary>
	/// Draws GUI and initializes values
	/// </summary>
	tft.setRotation(2);
	tft.fillScreen(S6D02A1_BLACK);
	tft.setTextColor(S6D02A1_WHITE, S6D02A1_BLACK);  // Adding a black background colour erases previous text automatically

	// Draw UI Boxes face
	//top boxes
	tft.drawRect(0, 0, GUI_BOX_WIDTH, GUI_BOX_HEIGHT, S6D02A1_BLUE);
	tft.drawRect(64, 0, GUI_BOX_WIDTH, GUI_BOX_HEIGHT, S6D02A1_BLUE);
	//bot box
	tft.drawRect(0, 135, MAIN_SCREEN_WIDTH, GUI_BOX_HEIGHT, S6D02A1_BLUE);
	
	//steps
	tft.setCursor(3, 3);
	tft.print("Steps: ");
	
	//battery
	tft.setCursor(68, 3);
	tft.print("Battery: ");
	
	//reset counts
	UpdateSteps();
	UpdateBattery(100);
	UpdateClock();

	return 0;
}