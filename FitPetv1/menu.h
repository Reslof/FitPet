// menu.h

#ifndef MENU_H
#define MENU_H
#define menu_top 27   // Postition of first menu item from top of screen
#define numMenu 4 // number of items on first menu - 1

extern char menu_select;
extern char* menu[];

typedef void(*MenuFuncPtr) (); // this is a typedef to the menu functions

extern MenuFuncPtr menu_func[];

void tftMenuSelect(char menuitem);
void tftMenuInit();
void dispPetMenu(void);
void dispInteractMenu(void);
void dispStats(void);
void dispActionCost(void);

#endif