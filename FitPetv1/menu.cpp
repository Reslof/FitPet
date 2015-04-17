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
	"<- Main Menu",
	NULL};

MenuFuncPtr Petmenu_func[] = { 0,
	dispPetHealth,
	dispPetMood,
	dispPetPersonality,
	goToMainMenu };

char* Interactmenu[] = { "INTERACT MENU",
	"Feed",
	"Play",
	"Trick",
	"Sleep",
	"Clean",
	"Options",
	"<- Main Menu",
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
	"Steps Taken",
	"Goal",
	"Next Trick",
	"Steps to LVL UP",
	"<- Main Menu",
	NULL };
MenuFuncPtr Statsmenu_func[]
	{0,
	dispStepsTaken,
	dispGoal,
	dispNxtTrick,
	dispStepsLVLUP,
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
	//tftMenuInit(ActionCostmenu);                  // Redraw the Menu
	//tftMenuSelect(menu_select, ActionCostmenu);     // Highlight the current menu item
	//current_menu = ActionCostmenu;
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

}
void dispPlay(void){
	Serial.println("This is the dispPlay Menu");

}
void dispTrick(void){
	Serial.println("This is the dispTrick Menu");

}
void dispSleep(void){
	Serial.println("This is the dispSleep Menu");

}
void dispClean(void){
	Serial.println("This is the dispClean Menu");

}
void dispOptions(void){
	Serial.println("This is the dispOptions Menu");

}