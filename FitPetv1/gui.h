#include <TFT_S6D02A1\TFT_S6D02A1.h>

#define SD_CS   7  // Chip select line for SD card
#define TFT_CS  9  // Chip select line for TFT display
#define TFT_DC  8  // Data/command line for TFT
#define TFT_RST 6  // Reset line for TFT (or connect to +5V)
#define NEXT_LINE 10
#define SAME_LINE 0

extern TFT_S6D02A1 tft;

static uint8_t conv2d(const char* p);

int initGUI(void);
void DebugMessage(char message[], int LINE);
void UpdateBattery(int BATTERY_LEVEL);
void UpdateClock(void);
void UpdateSteps(int STEPS);
void ClearMainScreen(void);