#include "menu.h"
#include "gui.h"

/* For those of you interested in creating a Menu system, we have defined
two arrays here. One holds the Menu Title and menu headings, and the other
holds the associated function to be called. This is a great way to simplify
the configuration of a menu especially when multiple menu's are rquired
*/

char* menu[] = { "",
"Small Text     ",
"Font Sizes     ",
"Lines          ",
"Rectangles     ",
"Bar Graph      ",
"Circles        ",
"Media Buttons  " };

MenuFuncPtr menu_func[] = {0,
	tftTextTest,
	tftPrintTest,
	tftLinesTest,
	tftRectTest,
	tftBarGraphTest,
	tftCirclesTest,
	tftMediaButtonsTest };

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

void tftTextTest(void)
{
	// Print some small text
	char * lorem = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat.";
	tft.fillScreen(S6D02A1_BLACK);
	tft.setCursor(0, 0);
	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextWrap(true);
	tft.println(lorem);
	tft.println(lorem);
	tft.println(lorem);
	delay(2000);
}

void tftPrintTest()
{
	// Print different font sizes
	char * hello = "Hello World!";
	tft.setTextWrap(false);
	tft.fillScreen(S6D02A1_BLACK);
	tft.setCursor(0, 30);
	tft.setTextColor(S6D02A1_MAGENTA);
	tft.setTextSize(1);
	tft.println(hello);
	tft.setTextColor(S6D02A1_YELLOW);
	tft.setTextSize(2);
	tft.println(hello);
	tft.setTextColor(S6D02A1_GREEN);
	tft.setTextSize(3);
	tft.println(hello);
	tft.setTextColor(S6D02A1_BLUE);
	tft.setTextSize(4);
	tft.print(1234.567);
	tft.setTextSize(1);
	delay(2000);

}

void tftLinesTest(void)
{
	// Print lines
	tft.fillScreen(S6D02A1_BLACK);
	for (int16_t x = 0; x < tft.width(); x += 6)
	{
		tft.drawLine(0, 0, x, tft.height() - 1, S6D02A1_YELLOW);
	}
	for (int16_t y = 0; y < tft.height(); y += 6)
	{
		tft.drawLine(0, 0, tft.width() - 1, y, S6D02A1_YELLOW);
	}

	tft.fillScreen(S6D02A1_BLACK);
	for (int16_t x = 0; x < tft.width(); x += 6)
	{
		tft.drawLine(tft.width() - 1, 0, x, tft.height() - 1, S6D02A1_YELLOW);
	}
	for (int16_t y = 0; y < tft.height(); y += 6)
	{
		tft.drawLine(tft.width() - 1, 0, 0, y, S6D02A1_YELLOW);
	}

	tft.fillScreen(S6D02A1_BLACK);
	for (int16_t x = 0; x < tft.width(); x += 6)
	{
		tft.drawLine(0, tft.height() - 1, x, 0, S6D02A1_YELLOW);
	}
	for (int16_t y = 0; y < tft.height(); y += 6)
	{
		tft.drawLine(0, tft.height() - 1, tft.width() - 1, y, S6D02A1_YELLOW);
	}

	tft.fillScreen(S6D02A1_BLACK);
	for (int16_t x = 0; x < tft.width(); x += 6)
	{
		tft.drawLine(tft.width() - 1, tft.height() - 1, x, 0, S6D02A1_YELLOW);
	}
	for (int16_t y = 0; y < tft.height(); y += 6)
	{
		tft.drawLine(tft.width() - 1, tft.height() - 1, 0, y, S6D02A1_YELLOW);
	}
	delay(2000);

}

void tftRectTest(void)
{
	// Print Rectangles
	tft.fillScreen(S6D02A1_BLACK);
	for (int16_t x = 0; x < tft.width(); x += 6)
	{
		tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, S6D02A1_GREEN);
	}
	tft.fillScreen(S6D02A1_BLACK);
	delay(2000);

}

void tftBarGraphTest(void)
{
	// Print Bar Graph
	int16_t x;
	unsigned char origin_x = 10;
	unsigned char origin_y = 115;
	unsigned char width = 15;
	unsigned char spacing = 20;
	unsigned char height = 10;
	tft.fillScreen(S6D02A1_BLACK);

	tft.drawLine(origin_x, origin_y, origin_x, 1, S6D02A1_BLUE);
	tft.drawLine(origin_x, origin_y, tft.width(), origin_y, S6D02A1_BLUE);

	for (int16_t x = origin_x + 1; x <tft.width() - spacing; x += spacing)
	{
		tft.fillRect(x, origin_y - height, width, height, S6D02A1_GREEN);
		height += 10;
	}


	tft.setTextColor(S6D02A1_WHITE);
	tft.setTextSize(1);
	tft.setCursor(0, origin_y);
	tft.print("0");
	tft.setCursor(0, 1);
	tft.print("10");
	for (char i = 0; i <7; i++)
	{
		x = origin_x + 8 + (spacing*i);
		tft.setCursor(x, origin_y + 3);
		tft.print(i + 1);
	}
	tft.setCursor(40, 20);
	tft.print("Bar Graph");

	delay(2000);
}

void tftCirclesTest(void)
{
	// Print Circles
	char radius = 10;
	tft.fillScreen(S6D02A1_BLACK);
	for (int16_t x = radius; x < tft.width(); x += radius * 2)
	{
		for (int16_t y = radius; y < tft.height(); y += radius * 2)
		{
			tft.fillCircle(x, y, radius, S6D02A1_BLUE);
		}
	}
	for (int16_t x = 0; x < tft.width() + radius; x += radius * 2)
	{
		for (int16_t y = 0; y < tft.height() + radius; y += radius * 2)
		{
			tft.drawCircle(x, y, radius, S6D02A1_WHITE);
		}
	}
	delay(2000);

}

void tftMediaButtonsTest(void)
{
	// Media Buttons
	tft.setRotation(0); // Set to landscape   
	tft.fillScreen(S6D02A1_BLACK);
	tft.fillRoundRect(25, 10, 78, 60, 8, S6D02A1_WHITE);
	tft.fillTriangle(42, 20, 42, 60, 90, 40, S6D02A1_RED);
	delay(500);
	// pause
	tft.fillRoundRect(25, 90, 78, 60, 8, S6D02A1_WHITE);
	tft.fillRoundRect(39, 98, 20, 45, 5, S6D02A1_GREEN);
	tft.fillRoundRect(69, 98, 20, 45, 5, S6D02A1_GREEN);
	delay(500);
	// play color
	tft.fillTriangle(42, 20, 42, 60, 90, 40, S6D02A1_BLUE);
	delay(50);
	// pause color
	tft.fillRoundRect(39, 98, 20, 45, 5, S6D02A1_RED);
	tft.fillRoundRect(69, 98, 20, 45, 5, S6D02A1_RED);
	// play color
	tft.fillTriangle(42, 20, 42, 60, 90, 40, S6D02A1_GREEN);
	tft.setRotation(3); // Set to landscape   
	delay(2000);

}