// menu.h

#ifndef MENU_H
#define MENU_H
#define menu_top 27   // Postition of first menu item from top of screen
#define numMenu 7 // number of items on first menu - 1

extern char menu_select;
extern char* menu[];

typedef void(*MenuFuncPtr) (); // this is a typedef to the menu functions

extern MenuFuncPtr menu_func[];

void tftMediaButtonsTest(void);
void tftCirclesTest(void);
void tftBarGraphTest(void);
void tftRectTest(void);
void tftLinesTest(void);
void tftPrintTest();
void tftTextTest(void);
void tftMenuSelect(char menuitem);
void tftMenuInit();

#endif