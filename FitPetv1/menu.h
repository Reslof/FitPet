// menu.h

#ifndef MENU_H
#define MENU_H
#define menu_top 27   // Postition of first menu item from top of screen

typedef void(*MenuFuncPtr) (); // this is a typedef to the menu functions


extern int menu_select;
extern char ** current_menu;
extern char* Mainmenu[];
extern char* Petmenu[];


extern MenuFuncPtr Mainmenu_func[];


void CallMenuFunction(int select);
void tftMenuSelect(char menuitem, char** MENU);
void tftMenuInit(char**);
int getArraySize(char** menu);
void goToMainMenu(void);

void dispPetMenu(void);
void dispInteractMenu(void);
void dispStats(void);
void dispActionCost(void);

void dispPetHealth(void);
void dispPetMood(void);
void dispPetPersonality(void);

void dispFeed(void);
void dispPlay(void);
void dispTrick(void);
void dispSleep(void);
void dispClean(void);
void dispOptions(void);

void dispStepsTaken(void);
void dispGoal(void);
void dispNxtTrick(void);
void dispStepsLVLUP(void);

void dispFeedCost(void);
void dispPlayCost(void);
void dispTrickCost(void);
void dispSleepCost(void);
void dispCleanCost(void);

void resetText(void);
#endif