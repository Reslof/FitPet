#define INCLUDE_SPRITES 1//CHANGE THIS VALUE TO 1 IN ORDER TO USE SPRITES

#ifndef GUI_H
#define GUI_H

#include <TFT_S6D02A1\TFT_S6D02A1.h>
#include <RTCdue\RTCdue.h>
#include "Animations.c"

//Hardware constants
#define TFT_CS  38  // Chip select line for TFT display
#define TFT_DC  40  // Data/command line for TFT
#define TFT_RST 36
#define TFT_BL 3
#define ASCII 0		//PrintVariable option
#define HEX 16		//PrintVariable option

//GUI variables
#define MAIN_SCREEN_WIDTH 128
#define MAIN_SCREEN_HEIGHT 109
#define MIDDLE_MAIN_SCREEN_WIDTH 64
#define MIDDLE_MAIN_SCREEN_HEIGHT 54
#define MIDDLE_OF_SCREEN_HEIGHT 80
#define GUI_BOX_HEIGHT 25
#define GUI_BOX_WIDTH 64

#define EDDY_DELAY 20
//globals
extern TFT_S6D02A1 tft;
extern RTC_DS1307 rtc;
extern volatile uint8_t highlightLine;


//expressions
#define HAPPY 0
#define HAPPY_GRIN 1
#define HAPPY_SMILE 2
#define HEART 3
#define MAD 4
#define REALLY_MAD 5
#define QUESTION 6
#define SMILE 7
#define SAD 8
#define EXCLAMATION 9
#define DOTDOTDOT 10

//pets
#define LUIS 0
#define EDDY 1
#define AIDEE 2
#define ERIK 3
//function prototypes

extern const tImage * Logo[];
void DrawSprite(const tImage * sprite, uint8_t x, uint8_t y);

#if INCLUDE_SPRITES
extern const tImage * emotions[];
extern const tImage * Luis[];
extern const tImage * Eddy[];
extern const tImage * Erik[];
extern const tImage * Aidee[];

void AnimatePet(int);
void DrawPet(int);
void DrawExpression(int);
void ClearExpression(void);
#endif

extern boolean disableClock;
extern boolean enableSysMessage;
extern unsigned int stepsTaken;
extern unsigned int battery_level;
extern char *SysMessage;

int initGUI(void);
void DebugMessage(char *message);
void DebugMessage(String message);
void SystemMessage(char *message);
void EraseSysMessage(void);
void LoadingScreenCircles(void);
void LoadingScreenIcon(void);
void PrintVariable(unsigned char variable, int representation);
void UpdateBattery(void);
void UpdateClock(void);
void UpdateSteps(void);
void ClearMainScreen(void);
void EraseBMP(int x, int y);
void displayMenu(void);
void DrawMenuItem(char * item, int BG_COLOR = S6D02A1_BLACK);
void DrawMenuTitle(char* menu[]);
#endif
