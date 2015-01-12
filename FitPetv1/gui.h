#include <TFT_S6D02A1\TFT_S6D02A1.h>
#include "loadingIconAnimation.c"

//Hardware constants
#define SD_CS   7  // Chip select line for SD card
#define TFT_CS  9  // Chip select line for TFT display
#define TFT_DC  8  // Data/command line for TFT
#define TFT_RST 6  // Reset line for TFT (or connect to +5V)
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

//globals
extern TFT_S6D02A1 tft;
static uint8_t conv2d(const char* p);

//function prototypes
int initGUI(void);
void DebugMessage(char *message);
void DebugMessage(String message);
void DrawSprite(const tImage sprite, uint8_t x, uint8_t y);
void LoadingScreenCircles(void);
void LoadingScreenIcon(void);
void PrintVariable(unsigned char variable, int representation);
void UpdateBattery(int BATTERY_LEVEL);
void UpdateClock(void);
void UpdateSteps(int STEPS);
void ClearMainScreen(void);
void EraseBMP(int x, int y);