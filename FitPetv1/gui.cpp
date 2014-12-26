#include "gui.h"
#include "hardware.h"
#include <stdint.h>

uint8_t hh, mm, ss; // Get H, M, S from compile time
int previousLine = 0;  //global to keep track of DebugMessage line

void UpdateBattery(int BATTERY_LEVEL) {
	tft.setCursor(68, 15); //sets cursor in right place
	if (BATTERY_LEVEL < 100) {
		tft.print(" ");
	}

	tft.print(BATTERY_LEVEL);
	tft.print("%");
}
void UpdateClock(void) {
	//takes care of the clock
	//Will most likely be removed once RTC is implemented

	if (1) {
		ss++;              // Advance second
		if (ss == 60) {
			ss = 0;
			mm++;            // Advance minute
			if (mm > 59) {
				mm = 0;
				hh++;          // Advance hour
				if (hh > 23) {
					hh = 0;
				}
			}
		}

		// Update digital time
		tft.setCursor(34, 138);

		if (hh > 12) {
			if (hh < 22) tft.print('0');
			tft.print(hh - 12);
		}
		else {
			if (hh < 10) tft.print('0');
			tft.print(hh);
		}
		tft.print(':');
		if (mm < 10) tft.print('0');
		tft.print(mm);
		tft.print(':');
		if (ss < 10) tft.print('0');
		tft.print(ss);
		if (hh > 12) tft.print(" pm");
		else tft.print(" am");
	}
}

void UpdateSteps(int steps_taken) {
	//probably need to add code to fetch value from EEPROM once its integrated
	tft.setCursor(3, 15);
	tft.print(steps_taken);
}

void DebugMessage(char *message){
	tft.setCursor(0, (27 + previousLine));
	tft.println(message);
	previousLine += 10;
	if (previousLine > 100){ //if more than 10 lines printed, start at top again
		previousLine = 0;
	}
}
void PrintVariable(unsigned char variable, int representation){
	//this funtion prints the ASCII code or HEX value of variable
	//can also do decimal I believe with the DEC option
	tft.setCursor(0, (27 + previousLine));
	if (representation == ASCII){
		tft.println(char(variable));
	}
	if (representation == HEX){
		tft.println(variable, HEX);
	}
	previousLine += 10;
	if (previousLine > 100){ //if more than 10 lines printed, start at top again
		previousLine = 0;
	}
}
void ClearMainScreen(void){
	tft.fillRect(0, 26, MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT, S6D02A1_BLACK);
	previousLine = 0; //resets line count to print at top again
}

void EraseBMP(int x, int y){
	tft.fillRect(x, y, bmpWidth, bmpHeight, S6D02A1_BLACK);
}
int initGUI(void){
	tft.setRotation(2);
	tft.fillScreen(S6D02A1_BLACK);
	tft.setTextColor(S6D02A1_WHITE, S6D02A1_BLACK);  // Adding a black background colour erases previous text automatically

	// Draw UI Boxes face
	//x, y, width, height, color
	//tft.drawRect(0, 0, 128, 160, S6D02A1_BLUE); //main box
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

	//time
	tft.setCursor(34, 150);
	tft.print(__DATE__);
	
	//reset counts
	UpdateSteps(0);
	UpdateBattery(100);
	UpdateClock();

	return 0;
}
static uint8_t conv2d(const char* p) {
	uint8_t v = 0;
	if ('0' <= *p && *p <= '9')
		v = *p - '0';
	return 10 * v + *++p - '0';
}