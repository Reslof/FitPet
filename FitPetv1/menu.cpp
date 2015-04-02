#include "menu.h"
#include "gui.h"

/* For those of you interested in creating a Menu system, we have defined
two arrays here. One holds the Menu Title and menu headings, and the other
holds the associated function to be called. This is a great way to simplify
the configuration of a menu especially when multiple menu's are rquired
*/

char* menu[] = { "",
"Pet Menu       ",
"Interact Menu  ",
"Stats          ",
"Action Cost    " };

MenuFuncPtr menu_func[] = {0,
	dispPetMenu,
	dispInteractMenu,
	dispStats,
	dispActionCost };

// It's usefult to know the number of menu items without hardcoding it
// We can calculate it thus.. (subtract 1 for the menu heading)

void tftMenuInit()
{
	// Clear screen and display the menu
	char i;

	tft.setTextColor(S6D02A1_WHITE, S6D02A1_BLACK);
	for (i = 0; i <= numMenu; i++)
	{
		tft.setCursor(0, ((i - 1) * 10) + menu_top);
		tft.println(menu[i]);
	}
	DrawMenuTitle("MAIN MENU", "  Choose an option.");

}

void tftMenuSelect(char menuitem)
{
	// Highlight a selected menu item
	char i;
	// Remove highlight of current item
	tft.setCursor(0, ((menu_select - 1) * 10) + menu_top);
	tft.setTextColor(S6D02A1_WHITE, S6D02A1_BLACK);
	tft.println(menu[menu_select]);
	// Highlight new menu item
	tft.setCursor(0, ((menuitem - 1) * 10) + menu_top);
	tft.setTextColor(S6D02A1_WHITE, S6D02A1_BLUE);
	tft.println(menu[menuitem]);
	// change menu_select to new item  
	menu_select = menuitem;

}

void dispPetMenu(void)
{
	// Print some small text
	char * petMenu = "This is the pet menu! Will be filled soon. ";
	tft.fillScreen(S6D02A1_BLACK);
	tft.setCursor(0, 0);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(petMenu);
	delay(2000);
}

void dispInteractMenu(void)
{
	// Print different font sizes
	char * interact = "This is where the interaction menu is";
	tft.setTextWrap(false);
	tft.fillScreen(S6D02A1_BLACK);
	tft.setCursor(0, 30);
	tft.setTextColor(S6D02A1_MAGENTA);
	tft.setTextSize(1);
	tft.println(interact);
	delay(2000);

}

void dispStats(void)
{
	// Print some small text
	char * stats = "This is where you can see the users' stats! ";
	tft.fillScreen(S6D02A1_BLACK);
	tft.setCursor(0, 0);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(stats);
	delay(2000);
}

void dispActionCost(void)
{
	// Print some small text
	char * action = "This is where you will need to see what you need to do to get an action! ";
	tft.fillScreen(S6D02A1_BLACK);
	tft.setCursor(0, 0);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(action);
	delay(2000);
}