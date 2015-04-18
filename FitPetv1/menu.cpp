#include "menu.h"
#include "gui.h"

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

void goToMainMenu(void){
	current_menu = Mainmenu;
}
// It's usefult to know the number of menu items without hardcoding it
// We can calculate it thus.. (subtract 1 for the menu heading)
void CallMenuFunction(int select){
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
		//current_menu = Actioncostmenu;
		done = true;
	}



	if (current_menu == Petmenu && !done){ //Pet menu handler
		Serial.println("Selecting Pet action...");
		Petmenu_func[select]();
		done = true;
	}

	if (current_menu == Petmenu && select == 4){ //<- Go back
		current_menu = Mainmenu;
		menu_select = 1;

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
	char * text = "This is the Pet Health Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();
}
void dispPetMood(void){
	ClearMainScreen();
	Serial.println("This is the Pet Mood Menu");
	char * text = "This is the Pet Mood Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();

}
void dispPetPersonality(void){
	ClearMainScreen();
	Serial.println("This is the Pet Personality Menu");
	char * text = "This is the Pet Personality Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();
}
//************************************************************
void dispFeed(void){
	Serial.println("This is the dispFeed Menu");
	char * text = "This is the Feed Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();

}
void dispPlay(void){
	Serial.println("This is the dispPlay Menu");
	char * text = "This is the Play Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();

}
void dispTrick(void){
	Serial.println("This is the dispTrick Menu");
	char * text = "This is the Trick Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();

}
void dispSleep(void){
	Serial.println("This is the dispSleep Menu");
	char * text = "This is the Sleep Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();

}
void dispClean(void){
	Serial.println("This is the dispClean Menu");
	char * text = "This is the Clean Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();

}
void dispOptions(void){
	Serial.println("This is the dispOptions Menu");
	char * text = "This is the Options Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();

}

void dispStepsTaken(void){
	Serial.println("This is the dispStepsTaken Menu");
	char * text = "This is the Steps Taken Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();
}

void dispGoal(void){
	Serial.println("This is the dispGoal Menu");
	char * text = "This is the Goal Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();
}

void dispNxtTrick(void){
	Serial.println("This is the dispNextTrick Menu");
	char * text = "This is the NxtTrick Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();
}

void dispStepsLVLUP(void){
	Serial.println("This is the dispStepsLVLUP Menu");
	char * text = "This is the LVLUP Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();
}

void dispFeedCost(void){
	Serial.println("This is the dispFeedCost Menu");
	char * text = "This is the FeedCost Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();
}

void dispPlayCost(void){
	Serial.println("This is the dispOptions Menu");
	char * text = "This is the Options Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();
}

void dispTrickCost(void){
	Serial.println("This is the dispTrickCost Menu");
	char * text = "This is the TrickCost Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();
}

void dispSleepCost(void){
	Serial.println("This is the dispSleepCost Menu");
	char * text = "This is the Sleep Cost Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();
}

void dispCleanCost(void){
	Serial.println("This is the dispCleanCost Menu");
	char * text = "This is the CleanCost Menu";
	tft.setCursor(0, 27);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(text);
	delay(2000);
	ClearMainScreen();
}

