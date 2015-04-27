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
#include "menu.h"

#define PET 3 //placeholder for pet selected, will be user selectable

//globals
TFT_S6D02A1 tft = TFT_S6D02A1(TFT_CS, TFT_DC, TFT_RST);
RTC_DS1307 rtc;

boolean animatePetFlag = true;
boolean menuFlag = false;
boolean isMenuDisplayed = false;
boolean updateGUI = true;
boolean screenON = true;
boolean TFTBLFlag = true;
boolean calibrateFlag = true;
boolean EEPROM_available, RTC_available, ACCEL_available = true;
boolean enableSysMessage = true;

float xcal, ycal, zcal, prevAcc = 0.0;
long last_interrupt_time = 0;        // will store last time LED was updated
long last_step_save_time = 0;
long last_message_time = 0;
unsigned int stepsTaken,AStepsTaken = 0;
unsigned int battery_level = 0;
char *SysMessage = "";
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
boolean doubleStepsFlag = false;

int currentPet = 0;
int  menu_select = 1;     // Currently elected menu item
char ** current_menu;

// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.
const int numReadings = 50;

int readings[numReadings];      // the readings from the analog input
int iindex = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

unsigned long sysMessageTime;

void setup(void) {

	Serial.begin(9600);
	Serial.println("Bluetooth ON");

	SPI.setClockDivider(SPI_CLOCK_DIV2); //sets SPI clock to 24 MHz, fastest possible
	pinMode(BTN1, INPUT); //set the pinmodes for buttons
	pinMode(BTN2, INPUT);
	pinMode(BTN3, INPUT);
	pinMode(BTN4, INPUT);
	pinMode(PIEZO, OUTPUT);
	pinMode(TFT_BL, OUTPUT);
	pinMode(BATTERY_IN, INPUT);

	analogReadResolution(12); //12-byte resolution 0-4095 max possible
	analogWrite(TFT_BL, 0); //make sure TFT backlight is off

	attachInterrupt(BTN1, setMenuFlag, FALLING); //handles button interrupts
	attachInterrupt(BTN4, setBLFlag, FALLING);

	Wire.begin();		//initializes I2C bus
	Wire1.begin();		//initializes I2C bus
	rtc.begin();		//initializes RTC

	tft.init();			//initializes TFT
	tft.fillScreen(S6D02A1_BLACK);
	DrawSprite(Logo[0], 15, 25);

	for (int i = 0; i < 256; i++){ //fade into max max brightness TFT BL
		analogWrite(TFT_BL, i);
		delay(5);
	}

	beep(150);
	beep(150);
	beep(150);

#if INCLUDE_SPRITES
	delay(1000);
	ClearMainScreen();
#endif
	// reserve 200 bytes for the inputString:
	inputString.reserve(200);

	RunInitTests();		//tests and initializes pheripherals

#if INCLUDE_SPRITES	
	ClearMainScreen();
	DrawPet(PET);
#endif

	//writeUint(0, 0x3E8); //stores 1000 steps
	//writeUint(10, 0x3E8); //stores 1000 steps

	setSteps(0); //loads step count from memory


	for (int thisReading = 0; thisReading < numReadings; thisReading++)
		readings[thisReading] = 0;
	
	current_menu = Mainmenu; //Starts at main menu.
	SysMessage = "This is a test.";

	}

void loop() {

	Serial.write("petPresent\r\n");


	sysMessageTime = millis();

	if (sysMessageTime - last_message_time > 8000){
		EraseSysMessage();
		last_message_time = sysMessageTime;
		doubleStepsFlag = false;
	}

	unsigned long interrupt_time = millis();

	// If interrupts come faster than 200ms, assume it's a bounce and ignore
	if (interrupt_time - last_interrupt_time > 100){
		UpdateAccel();
		UpdateSteps();
		//portraitLandscapeHandler();
		last_interrupt_time = interrupt_time;
	}

	unsigned long save_steps_time = millis();

	if (save_steps_time - last_step_save_time > 5000){
		Serial.print("Now saving steps: ");
		Serial.println(stepsTaken); //keep true count
		Serial.print("Now saving adjusted steps: ");
		Serial.println(AStepsTaken); //keep adjusted count

		writeUint(0, stepsTaken);
		writeUint(10, AStepsTaken);
		last_step_save_time = save_steps_time;
	}

	SmoothBatteryLevel();
	UpdateBattery();

	if (!disableClock){
		UpdateClock();					//Updates clock if in Pet context
	}
	
	if (menuFlag && updateGUI) {		//displays menu if Pet is being shown
		ClearMainScreen();
		//goes to main menu every time
		//current_menu = Mainmenu;
		tftMenuInit(current_menu);                  // Redraw the Menu
		tftMenuSelect(menu_select, current_menu);     // Highlight the current menu item
		beep(100);
		updateGUI = false;
	}
	else if(!menuFlag && !updateGUI){  //displays pet and clears menu		
		ClearMainScreen();
		tft.fillRect(1, 136, MAIN_SCREEN_WIDTH-2, GUI_BOX_HEIGHT-2, S6D02A1_BLACK); //clear Clock area
		initGUI();	//redraws GUI
		disableClock = false;
		updateGUI = true;
#if INCLUDE_SPRITES
		DrawPet(random(0,4));
#endif
	}

	if (digitalRead(BTN2) && menuFlag) {		//does nothing if no menu present
		Serial.println("BTN2 Pressed");
		// Down
		// move down one menu item, if at bottom wrap to top
		beep(100);
		if (menu_select < getArraySize(current_menu)){
			tftMenuSelect(menu_select + 1, current_menu);
		}else{ 
			tftMenuSelect(1, current_menu); 
		}
		
	}
	if (digitalRead(BTN2) && !menuFlag){
		AStepsTaken = AStepsTaken + 1000;
		stepsTaken = stepsTaken + 1000;
		Serial.println("Stored 1000 extra steps.");
	}

	if (digitalRead(BTN3)){
		Serial.println("BTN3 Pressed");
		animatePetFlag = true;
		if (menuFlag){ //if handling Menu context
			CallMenuFunction(menu_select);       // Call the appropriate menu function from array
			//delay(1000); //wait a bit
			//initGUI(); //redraw GUI
			updateGUI = true;
			animatePetFlag = false;
		}
		if (!menuFlag && animatePetFlag){ //if handling Pet context
#if INCLUDE_SPRITES
			currentPet = random(0, 4);
			PokePet(random(0,11));
#endif
		}
	}
		
	if (stringComplete) {		
		HandleBTCommands();
	}

}
void SmoothBatteryLevel(){
	// subtract the last reading:
	total = total - readings[iindex];
	// read from the sensor:  
	readings[iindex] = analogRead(A0);
	// add the reading to the total:
	total = total + readings[iindex];
	// advance to the next position in the array:  
	iindex = iindex + 1;

	// if we're at the end of the array...
	if (iindex >= numReadings)
		// ...wrap around to the beginning: 
		iindex = 0;

	// calculate the average:
	average = total / numReadings;
	// send it to the computer as ASCII digits
	//Serial.println("Average: ");
	//Serial.println(average);
	
	battery_level = map(average, 0, 4095, 0, 100);

}


void setMenuFlag(void){
	Serial.println("BTN1 Pressed");

	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	// If interrupts come faster than 200ms, assume it's a bounce and ignore
	if (interrupt_time - last_interrupt_time > 200)
	{
		menuFlag = !menuFlag;
	}
	last_interrupt_time = interrupt_time;

}

void setBLFlag(void){
	Serial.println("BTN4 Pressed");

	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	// If interrupts come faster than 200ms, assume it's a bounce and ignore
	if (interrupt_time - last_interrupt_time > 200)
	{
		if (screenON){
			analogWrite(TFT_BL, 0);			
		}
		else{
			analogWrite(TFT_BL, 255); //fade into max max brightness TFT BL
		}
		screenON = !screenON;
	}
	last_interrupt_time = interrupt_time;

}

void RunInitTests(void){

	if (!initGUI()){
		DebugMessage("GUI init: OK");
		Serial.println("GUI init: OK");
	}
	else{
		Serial.println("GUI init failed");
	}


	if (!initEEPROM()){
		DebugMessage("EEPROM init: OK");
		Serial.println("EEPROM init: OK");
		EEPROM_available = true;
	}
	else{
		DebugMessage("EEPROM init: FAILED");
		Serial.println("EEPROM init: FAILED");
		EEPROM_available = false;
	}

	//rtc.adjust(DateTime(__DATE__, __TIME__));
	if (!rtc.isrunning()) {
		Serial.println("RTC is NOT running!");
		delay(2000);
		// following line sets the RTC to the date & time this sketch was compiled
		rtc.adjust(DateTime(__DATE__, __TIME__));
		RTC_available = false;
	}
	else{
		RTC_available = true;
	}

	initMMA8452(SCALE, DATARATE); //Test and intialize the MMA8452

	delay(3000); //wait
	ClearMainScreen();
}
void HandleBTCommands(void){//handles Bluetooth commands!!!! 
	String Pet = "petPresent\r\n";
	String read = "read\n";
	String loading = "loading\n";
	String showPet = "pet\n";

	if (inputString.equalsIgnoreCase(Pet)){
		SysMessage="Friendly pet nearby!";
		SystemMessage(SysMessage);
		doubleStepsFlag = true;
	}


	if (inputString.equalsIgnoreCase("setsteps")){
		writeUint(0, 0x3E8); //stores 1000 steps
		writeUint(10, 0x3E8); //stores 1000 steps	
		Serial.println("Stored 1000 steps in EEPROM");
	}
	// clear the string:
	inputString = "";
	stringComplete = false;
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

int UpdateAccel(void){
	if (ACCEL_available){
		boolean range = true;
		int accelCount[3];  // Stores the 12-bit signed value
		float x, y, z, a = 0.00;
		float acceleration_mag;
		float accelG[3];  // Stores the real accel value in g's
		float accel_avg;

		readAccelData(accelCount);  // Read the x/y/z adc values
		// Now we'll calculate the accleration value into actual g's
		for (int i = 0; i < 3; i++){
			accelG[i] = (float)accelCount[i] / ((1 << 12) / (2 * GSCALE)); // get actual g value, this depends on scale being set
		}

		x = accelG[0];
		y = accelG[1];
		z = accelG[2];

		acceleration_mag = abs(x - xcal) + abs(y - ycal) + abs(z - zcal);

		if (acceleration_mag - prevAcc > 0.7){
			stepsTaken++;
			AStepsTaken++;
			if (doubleStepsFlag){
				AStepsTaken++;
			}
		}

		prevAcc = acceleration_mag;
		/*
		// Print out values
		Serial.print(x, 3);  // Print g values
		Serial.print("\t");  // tabs in between axes
		Serial.print(y, 3);  // Print g values
		Serial.print("\t");  // tabs in between axes
		Serial.print(z, 3);  // Print g values
		Serial.print("\t");  // tabs in between axes

		Serial.println();
		*/
		return 0;
	}

}
