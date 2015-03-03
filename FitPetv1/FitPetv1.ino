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
#include "menu.h"

#define PET 3 //placeholder for pet selected, will be user selectable

//globals
TFT_S6D02A1 tft = TFT_S6D02A1(TFT_CS, TFT_DC, TFT_RST);
RTC_DS1307 rtc;

float xcal, ycal, zcal = 0.0;
int battery_level = 100;
boolean animatePetFlag = true;
boolean menuFlag = false;
boolean isMenuDisplayed = false;
boolean updateGUI = true;
boolean screenON = true;
boolean TFTBLFlag = true;

volatile int calibrateFlag = 1;

long previousMillis = 0;        // will store last time LED was updated

long interval = 500;           // interval at which to blink (milliseconds)

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

char  menu_select = 1;     // Currently elected menu item

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

	analogWrite(TFT_BL, 0); //make sure TFT backlight is off

	attachInterrupt(BTN1, setMenuFlag, FALLING); //handles button interrupts
	attachInterrupt(BTN4, setBLFlag, FALLING);

	Wire.begin();		//initializes I2C bus
	Wire1.begin();		//initializes I2C bus
	rtc.begin();		//initializes RTC

	tft.init();			//initializes TFT
	tft.fillScreen(S6D02A1_BLACK);

	for (int i = 0; i < 256; i++){ //fade into max max brightness TFT BL
		analogWrite(TFT_BL, i);
		delay(5);
	}

	beep(150);
	beep(150);
	beep(150);

	// reserve 200 bytes for the inputString:
	inputString.reserve(200);

	RunInitTests();		//tests and initializes pheripherals

#if INCLUDE_SPRITES
	DrawPet(PET);
#endif
}

void loop() {
	float acc;
	unsigned long currentMillis = millis();

	if (currentMillis - previousMillis > 500) { //early pedometer code
		acc = UpdateAccel();

		if (acc > 0.5){
			UpdateSteps();
		}
	}

	if (battery_level < 0) {
		battery_level = 100;
	}

	if (!disableClock){
		UpdateClock();					//Updates clock if in Pet context
	}
	
	if (menuFlag && updateGUI) {		//displays menu if Pet is being shown
		ClearMainScreen();
		tftMenuInit();                  // Redraw the Menu
		tftMenuSelect(menu_select);     // Highlight the current menu item
		beep(100);
		updateGUI = false;
	}
	else if(!menuFlag && !updateGUI){  //displays pet and clears menu		
		ClearMainScreen();
		tft.fillRect(1, 136, MAIN_SCREEN_WIDTH-2, GUI_BOX_HEIGHT-2, S6D02A1_BLACK); //clear Clock area
		initGUI();	//redraws GUI
		disableClock = false;
		updateGUI = true;
		DrawPet(random(0,4));
	}

	if (digitalRead(BTN2) && menuFlag) {		//does nothing if no menu present
		// Down
		// move down one menu item, if at bottom wrap to top
		beep(100);
		if (menu_select<numMenu) tftMenuSelect(menu_select + 1);
		else tftMenuSelect(1);
		
	 }
	if (digitalRead(BTN3)){
		animatePetFlag = true;
		if (menuFlag){ //if handling Menu context
			menu_func[menu_select]();       // Call the appropriate menu function from array
			delay(1000); //wait a bit
			initGUI(); //redraw GUI
			updateGUI = true;
			animatePetFlag = false;
		}
		if (!menuFlag && animatePetFlag){ //if handling Pet context
			beep(150);
			ClearMainScreen();
			AnimatePet(random(0, 4));	//This is Poke Pet action
			beep(150);
			DrawExpression(random(0,11)); //get random emotion from being poked
			delay(1000);
			ClearExpression();
		}
	}
		
	if (stringComplete) {		
		HandleBTCommands();
	}

}

void setMenuFlag(void){
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
	}
	else{
		Serial.write("GUI init failed");
	}


	if (initEEPROM() == 0xFF){
		DebugMessage("EEPROM init: OK");
	}
	else{
		DebugMessage("EEPROM init: FAILED");
	}

	//rtc.adjust(DateTime(__DATE__, __TIME__));
	if (!rtc.isrunning()) {
		Serial.println("RTC is NOT running!");
		delay(2000);
		// following line sets the RTC to the date & time this sketch was compiled
		rtc.adjust(DateTime(__DATE__, __TIME__));
	}

	initMMA8452(); //Test and intialize the MMA8452

	delay(3000); //wait
	ClearMainScreen();
}
void HandleBTCommands(void){//handles Bluetooth commands!!!! 
	String clear = "clr\n";
	String read = "read\n";
	String loading = "loading\n";
	String showPet = "pet\n";

	DebugMessage(inputString);
	if (inputString.equalsIgnoreCase(clear)){
		ClearMainScreen();
	}
	if (inputString.equalsIgnoreCase(loading)){
		animatePetFlag = !animatePetFlag;
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

float UpdateAccel(void){
	int accelCount[3];  // Stores the 12-bit signed value
	float x, y, z = 0.00;
	float thresh = 0.5;
	readAccelData(accelCount);  // Read the x/y/z adc values

	// Now we'll calculate the accleration value into actual g's
	float accelG[3];  // Stores the real accel value in g's
	for (int i = 0; i < 3; i++)
	{
		accelG[i] = (float)accelCount[i] / ((1 << 12) / (2 * GSCALE)); // get actual g value, this depends on scale being set
	}
	
	if (calibrateFlag){
		Serial.print("Calibrating...");
		xcal = accelG[0];
		ycal = accelG[1];
		zcal = accelG[2];
		calibrateFlag = 0;
	}
	
	x = accelG[0] - xcal;
	y = accelG[1] - ycal;
	z = accelG[2] - zcal;

	// Print out values
	Serial.print(x, 3);  // Print g values
	Serial.print("\t");  // tabs in between axes
	Serial.print(y, 3);  // Print g values
	Serial.print("\t");  // tabs in between axes
	Serial.print(z, 3);  // Print g values
	Serial.print("\t");  // tabs in between axes
	
	Serial.println();

	return x;
}

