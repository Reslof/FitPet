#include "menu.h"
#include "gui.h"

unsigned int FeedCost = 200;
unsigned int PlayCost = 600;
unsigned int TrickCost = 600;
unsigned int SleepCost = 400;
unsigned int CleanCost = 500;
unsigned int stepsGoal = 10000;
unsigned int currentLevel; 

/* For those of you interested in creating a Menu system, we have defined
two arrays here. One holds the Menu Title and menu headings, and the other
holds the associated function to be called. This is a great way to simplify
the configuration of a menu especially when multiple menu's are rquired
*/

char* Mainmenu[] = { "MAIN MENU", //menu title
	"Pet Menu       ",
	"Interact Menu  ",
	"Stats          ",
	"Action Cost    ", 
	NULL};

MenuFuncPtr Mainmenu_func[] = {0,
	dispPetMenu,
	dispInteractMenu,
	dispStats,
	dispActionCost };

char* Petmenu[] = { "PET MENU",
	"Health         ",
	"Mood           ",
	"Personality    ",
	"<- Main Menu   ",
	NULL};

MenuFuncPtr Petmenu_func[] = { 0,
	dispPetHealth,
	dispPetMood,
	dispPetPersonality,
	goToMainMenu };

char* Interactmenu[] = { "INTERACT MENU",
	"Feed           ",
	"Play           ",
	"Trick          ",
	"Sleep          ",
	"Clean          ",
	"Options        ",
	"<- Main Menu   ",
	NULL };

MenuFuncPtr Interactmenu_func[] = { 0,
	dispFeed,
	dispPlay,
	dispTrick,
	dispSleep,
	dispClean,
	dispOptions,
	goToMainMenu
	};

char* Statsmenu[] = { "STATS MENU",
	"Steps Taken    ",
	"Goal           ",
	"Next Trick     ",
	"Steps to LVL UP",
	"<- Main Menu   ",
	NULL };

MenuFuncPtr Statsmenu_func[] = {0,
	dispStepsTaken,
	dispGoal,
	dispNxtTrick,
	dispStepsLVLUP,
	goToMainMenu
	};

char* Costmenu[] = { "COSTS MENU",
	"Feed Cost      ",
	"Play Cost      ",
	"Trick Cost     ",
	"Sleep Cost     ",
	"Clean Cost     ",
	"<- Main Menu   ",
	NULL };

MenuFuncPtr Costmenu_func[] = {0,
	dispFeedCost,
	dispPlayCost,
	dispTrickCost,
	dispSleepCost,
	dispCleanCost,
	goToMainMenu
	};


struct Moods{
	int expression;
	char *expression_name;
}Moods[11] = {
	{ 0, "Happy" },
	{ 1, "Really Happy" },
	{ 2, "Very Happy" },
	{ 3, "In Love" },
	{ 4, "Mad" },
	{ 5, "Really Mad" },
	{ 6, "Confused" },
	{ 7, "Content" },
	{ 8, "Sad" },
	{ 9, "Surprised" },
	{ 10, "Aloof" },
};

char *Personalities[] = { "Adventurous",
"Classy",
"Affectionate",
"Brave",
"Naughty",
"Neat",
"Silly",
"Sneaky",
"Romantic",
"Timid",
"Unpredictable",
"Untidy"
};

void goToMainMenu(void){
	current_menu = Mainmenu;
	menu_select = 1;
}
// It's usefult to know the number of menu items without hardcoding it
// We can calculate it thus.. (subtract 1 for the menu heading)
void CallMenuFunction(int select){
	menu_select = 1;

	boolean done = false;

	Serial.println("select: ");
	Serial.println(select);

	Serial.println("Current menu");
	Serial.println(*current_menu);

	if (current_menu == Mainmenu && select == 1){ //Go To Pet menu
		Serial.println("Going to pet menu");
		current_menu = Petmenu;
		done = true;
	}
	if (current_menu == Mainmenu && select == 2){ //Go To Interact menu
		Serial.println("Going to interact menu");
		current_menu = Interactmenu;
		done = true;
	}
	if (current_menu == Mainmenu && select == 3){ //Go To Stats menu
		Serial.println("Going to Stats menu");
		current_menu = Statsmenu;
		done = true;
	}
	if (current_menu == Mainmenu && select == 4){ //Go To Action Cost menu
		Serial.println("Going to Action Cost menu");
		current_menu = Costmenu;
		done = true;
	}

	if (current_menu == Petmenu && !done){ //Pet menu handler
		Serial.println("Selecting Pet action...");
		Petmenu_func[select]();
		done = true;
	}

	if (current_menu == Interactmenu && !done){ //Interact menu handler
		Serial.println("Selecting Pet action...");
		Interactmenu_func[select]();
		done = true;
	}

	if (current_menu == Statsmenu && !done){ //Stats menu handler
		Serial.println("Selecting Pet action...");
		Statsmenu_func[select]();
		done = true;
	}

	if (current_menu == Costmenu && !done){ //Action Cost menu handler
		Serial.println("Selecting Pet action...");
		Costmenu_func[select]();
		done = true;
	}
		
}

int getArraySize(char** menu){
	char *j;
	int a = 0;
	while (j != NULL){
		j = menu[a];
		a++;
	}
	
	return a - 1; //get rid of the NULL element
}

void tftMenuInit(char** menu)
{
	// Clear screen and display the menu
	char i;
	int s = getArraySize(menu);

	tft.setTextColor(S6D02A1_WHITE, S6D02A1_BLACK);
	for (i = 1; i <= s; i++)
	{
		tft.setCursor(0, ((i - 1) * 10) + menu_top);
		tft.println(menu[i]);
	}

	DrawMenuTitle(menu);

}

void tftMenuSelect(char menuitem, char** MENU)
{
	
	// Highlight a selected menu item
	char i;
	// Remove highlight of current item
	tft.setCursor(0, ((menu_select - 1) * 10) + menu_top);
	tft.setTextColor(S6D02A1_WHITE, S6D02A1_BLACK);
	tft.println(MENU[menu_select]);
	// Highlight new menu item
	if (MENU[menuitem] == NULL){
		menuitem = 1;
	}

	tft.setCursor(0, ((menuitem - 1) * 10) + menu_top);
	tft.setTextColor(S6D02A1_WHITE, S6D02A1_BLUE);
	tft.println(MENU[menuitem]);
	// change menu_select to new item  
	menu_select = menuitem;
	tft.setTextColor(S6D02A1_WHITE, S6D02A1_BLACK);

}

//*******************************************************************Main Menus
void dispPetMenu(void)
{
	menu_select = 1;
	tftMenuInit(Petmenu);                  // Redraw the Menu
	tftMenuSelect(menu_select, Petmenu);     // Highlight the current menu item
	current_menu = Petmenu;
}

void dispInteractMenu(void)
{
	menu_select = 1;
	tftMenuInit(Interactmenu);                  // Redraw the Menu
	tftMenuSelect(menu_select, Interactmenu);     // Highlight the current menu item
	current_menu = Interactmenu;

}

void dispStatsMenu(void)
{
	menu_select = 1;
	tftMenuInit(Statsmenu);                  // Redraw the Menu
	tftMenuSelect(menu_select, Statsmenu);     // Highlight the current menu item
	current_menu = Statsmenu;
}

void dispActionCostMenu(void)
{
	menu_select = 1;
	tftMenuInit(Costmenu);                  // Redraw the Menu
	tftMenuSelect(menu_select, Costmenu);     // Highlight the current menu item
	current_menu = Costmenu;
}


//**********************************************************************Submenus
void dispPetHealth(void){
	ClearMainScreen();
	Serial.println("This is the Pet Health Menu");
	char * text = "Pet Health:";

	int randHealth= random(0, 11);
	DrawExpression(HEART);

	tft.setCursor(0, MIDDLE_MAIN_SCREEN_HEIGHT);
	tft.setTextColor(S6D02A1_WHITE);
	tft.println(text);
	tft.setTextSize(2);
	tft.setTextColor(S6D02A1_RED);

	tft.print("       x");
	tft.println(randHealth);

	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextSize(0);

	delay(2000);
	ClearMainScreen();
}
void dispPetMood(void){
	ClearMainScreen();	
	Serial.println("This is the Pet Mood Menu");
	char * text = "Pet Mood:";

	int randMood = random(0, 10);
	menuFlag = false;
	disableClock = false;
	tft.fillRect(1, 136, MAIN_SCREEN_WIDTH - 2, GUI_BOX_HEIGHT - 2, S6D02A1_BLACK); //clear Clock area
	UpdateClock();
	PokePet(randMood);

	char * m = Moods[randMood].expression_name;
	SysMessage = m;

	SystemMessage(SysMessage); //Prints at bottom of screen in case of connection, user action etc.
}
void dispPetPersonality(void){
	ClearMainScreen();
	Serial.println("This is the Pet Personality Menu");
	int r = random(0, 12);

	char * text = "Personality:";
	tft.setCursor(0, 27);
	
	tft.println(text);
	tft.setTextSize(1);
	tft.println("");
	tft.setTextColor(S6D02A1_CYAN);
	tft.println(Personalities[r]);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextSize(0);

	delay(2000);
	ClearMainScreen();
}
//************************************************************
void dispFeed(void){
	ClearMainScreen();
	Serial.println("This is the dispFeed Menu");
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	
	signed int cost = AStepsTaken - FeedCost;
	
	if (cost >= 0){
		tft.println("You feed your pet!");
		tft.println("");
		tft.setTextColor(S6D02A1_GREEN);

		tft.print("This cost you: ");
		tft.setTextColor(S6D02A1_RED);

		tft.print(FeedCost);
		tft.setTextColor(S6D02A1_GREEN);

		tft.print("   steps.");
		tft.println("");

		tft.print("Steps remaining: ");

		tft.setTextColor(S6D02A1_RED);

		tft.print(AStepsTaken - FeedCost);

		tft.setTextColor(S6D02A1_WHITE);

		AStepsTaken = AStepsTaken - FeedCost;

	}
	else{
	
		tft.print("Sorry you need ");
		tft.setTextColor(S6D02A1_RED);

		tft.print(abs(cost));
		tft.setTextColor(S6D02A1_WHITE);

		tft.print("   more steps to perform that action!");
	}


	delay(5000);
	ClearStepsScreen();
	ClearMainScreen();
	

}
void dispPlay(void){
	ClearMainScreen();
	Serial.println("This is the dispPlay Menu");
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);

	signed int cost = AStepsTaken - PlayCost;

	if (cost >= 0){
		tft.println("You play with your pet!");
		tft.println("");
		tft.setTextColor(S6D02A1_GREEN);

		tft.print("This cost you: ");
		tft.setTextColor(S6D02A1_RED);

		tft.print(PlayCost);
		tft.setTextColor(S6D02A1_GREEN);

		tft.print("   steps.");
		tft.println("");

		tft.print("Steps remaining: ");

		tft.setTextColor(S6D02A1_RED);

		tft.print(AStepsTaken - PlayCost);

		tft.setTextColor(S6D02A1_WHITE);

		AStepsTaken = AStepsTaken - PlayCost;

	}
	else{

		tft.print("Sorry you need ");
		tft.setTextColor(S6D02A1_RED);

		tft.print(abs(cost));
		tft.setTextColor(S6D02A1_WHITE);

		tft.print("   more steps to perform that action!");
	}

	delay(2000);
	ClearStepsScreen();
	ClearMainScreen();
	

}
void dispTrick(void){
	ClearMainScreen();
	Serial.println("This is the dispTrick Menu");
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);

	signed int cost = AStepsTaken - TrickCost;

	if (cost >= 0){
		tft.println("Your pet does a trick!");
		tft.println("");
		tft.setTextColor(S6D02A1_GREEN);

		tft.print("This cost you: ");
		tft.setTextColor(S6D02A1_RED);

		tft.print(TrickCost);
		tft.setTextColor(S6D02A1_GREEN);

		tft.print("   steps.");
		tft.println("");

		tft.print("Steps remaining: ");

		tft.setTextColor(S6D02A1_RED);

		tft.print(AStepsTaken - TrickCost);

		tft.setTextColor(S6D02A1_WHITE);

		AStepsTaken = AStepsTaken - TrickCost;

	}
	else{

		tft.print("Sorry you need ");
		tft.setTextColor(S6D02A1_RED);

		tft.print(abs(cost));
		tft.setTextColor(S6D02A1_WHITE);

		tft.print("   more steps to perform that action!");
	}

	delay(2000);
	ClearStepsScreen();
	ClearMainScreen();
	

}
void dispSleep(void){
	ClearMainScreen();
	Serial.println("This is the dispSleep Menu");
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);

	signed int cost = AStepsTaken - SleepCost;

	if (cost >= 0){
		tft.println("Your pet goes to sleep!");
		tft.println("");
		tft.setTextColor(S6D02A1_GREEN);

		tft.print("This cost you: ");
		tft.setTextColor(S6D02A1_RED);

		tft.print(SleepCost);
		tft.setTextColor(S6D02A1_GREEN);

		tft.print("   steps.");
		tft.println("");

		tft.print("Steps remaining: ");

		tft.setTextColor(S6D02A1_RED);

		tft.print(AStepsTaken - SleepCost);

		tft.setTextColor(S6D02A1_WHITE);

		AStepsTaken = AStepsTaken - SleepCost;

	}
	else{

		tft.print("Sorry you need ");
		tft.setTextColor(S6D02A1_RED);

		tft.print(abs(cost));
		tft.setTextColor(S6D02A1_WHITE);

		tft.print("   more steps to perform that action!");
	}

	delay(2000);
	ClearStepsScreen();
	ClearMainScreen();
	

}
void dispClean(void){
	ClearMainScreen();
	Serial.println("This is the dispClean Menu");
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);

	signed int cost = AStepsTaken - CleanCost;

	if (cost >= 0){
		tft.println("You clean your pet!");
		tft.println("");
		tft.setTextColor(S6D02A1_GREEN);

		tft.print("This cost you: ");
		tft.setTextColor(S6D02A1_RED);

		tft.print(CleanCost);
		tft.setTextColor(S6D02A1_GREEN);

		tft.print("   steps.");
		tft.println("");

		tft.print("Steps remaining: ");

		tft.setTextColor(S6D02A1_RED);

		tft.print(AStepsTaken - CleanCost);

		tft.setTextColor(S6D02A1_WHITE);

		AStepsTaken = AStepsTaken - CleanCost;

	}
	else{

		tft.print("Sorry you need ");
		tft.setTextColor(S6D02A1_RED);

		tft.print(abs(cost));
		tft.setTextColor(S6D02A1_WHITE);

		tft.print("   more steps to perform that action!");
	}

	delay(2000);
	ClearStepsScreen();
	ClearMainScreen();

}
void dispOptions(void){
	ClearMainScreen();
	Serial.println("This is the dispOptions Menu");
	char * text = "This is the Options Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();

}

//Stats menu functions
void dispStepsTaken(void){
	ClearMainScreen();
	Serial.println("This is the dispStepsTaken Menu");
	char * text = "Current Steps:";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	tft.setTextSize(2);
	tft.println(" ");
	tft.print(stepsTaken);
	tft.print(" Steps");
	delay(5000);
	resetText();
	ClearMainScreen();
}

void dispGoal(void){
	ClearMainScreen();
	Serial.println("This is the dispGoal Menu");
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println("Your current goal is:");
	tft.setTextColor(S6D02A1_CYAN);
	tft.setTextSize(2);
	tft.println(stepsGoal);
	resetText();
	delay(5000);
	ClearMainScreen();
}

void dispNxtTrick(void){
	ClearMainScreen();
	Serial.println("This is the dispNextTrick Menu");
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	int nextTrick = 0;

	if (stepsTaken < TrickCost){
		nextTrick = TrickCost - stepsTaken;
		tft.setTextColor(S6D02A1_RED);
		tft.print(nextTrick);
		tft.setTextColor(S6D02A1_WHITE);
		tft.print(" steps until next trick!");
	}
	else{
		tft.println("You're next trick is available!");
	}
	delay(5000);
	resetText();
	ClearMainScreen();
}

void dispStepsLVLUP(void){
	ClearMainScreen();
	Serial.println("This is the dispStepsLVLUP Menu");
	char * text = "This is the LVLUP Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();
}

//ActionCost menu functions
void dispFeedCost(void){
	ClearMainScreen();
	Serial.println("This is the dispFeedCost Menu");
	char * text1 = "The cost of feeding  your pet is:";
	char * text2 = "200 Steps";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text1);
	tft.setTextSize(2);
	tft.println(" ");
	tft.println(text2);
	delay(5000);
	resetText();
	ClearMainScreen();
}

void dispPlayCost(void){
	ClearMainScreen();
	Serial.println("This is the dispOptions Menu");
	char * text1 = "The cost of playing  with your pet is:";
	char * text2 = "600 Steps";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text1);
	tft.setTextSize(2);
	tft.println(" ");
	tft.println(text2);
	delay(5000);
	resetText();
	ClearMainScreen();
}

void dispTrickCost(void){
	ClearMainScreen();
	Serial.println("This is the dispTrickCost Menu");
	char * text1 = "The cost of having   your pet do a trick  is:";
	char * text2 = "600 Steps";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text1);
	tft.setTextSize(2);
	tft.println(" ");
	tft.println(text2);
	delay(5000);
	resetText();
	ClearMainScreen();
}

void dispSleepCost(void){
	ClearMainScreen();
	Serial.println("This is the dispSleepCost Menu");
	char * text1 = "The cost of your pet sleeping is:";
	char * text2 = "400 Steps";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text1);
	tft.setTextSize(2);
	tft.println(" ");
	tft.println(text2);
	delay(5000);
	resetText();
	ClearMainScreen();
}

void dispCleanCost(void){
	ClearMainScreen();
	Serial.println("This is the dispCleanCost Menu");
	char * text1 = "The cost of cleaning your pet is:";
	char * text2 = "500 Steps";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text1);
	tft.setTextSize(2);
	tft.println(" ");
	tft.println(text2);
	delay(5000);
	resetText();
	ClearMainScreen();
}

// this will reset the text size and color to default 0 and white
void resetText(void){
	tft.setTextSize(0);
	tft.setTextColor(S6D02A1_WHITE);
}
