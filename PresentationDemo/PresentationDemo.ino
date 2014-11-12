/*
  An example input and GUI routines for FitPet.
  But this is just a demo.

  Luis Flores 11/11/14 1.0
*/

#define PIEZO 3  //piezzo, pwm output
#define BTN1 5   //button 1, active high
#define BTN2 4   //button 2, active high
#define sclk 13  // Don't change
#define mosi 11  // Don't change
#define cs   9
#define dc   8
#define rst  6   // you can also connect this to the Arduino reset
#define SDcs 7

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_QDTech.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

Adafruit_QDTech tft = Adafruit_QDTech(cs, dc, rst);       // Invoke custom library

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t osx = 64, osy = 64, omx = 64, omy = 64, ohx = 64, ohy = 64; // Saved H, M, S x & y coords
uint16_t x0 = 0, x1 = 0, y0 = 0, y1 = 0;
uint32_t targetTime = 0;                    // for next 1 second timeout
uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6); // Get H, M, S from compile time

int steps = 0;
int battery_level = 100;
void setup(void) {

  pinMode(BTN1, INPUT); //set the pinmodes for buttons
  pinMode(BTN2, INPUT);

  tft.init();


  tft.setRotation(0);
  tft.fillScreen(QDTech_BLACK);
  tft.setTextColor(QDTech_WHITE, QDTech_BLACK);  // Adding a black background colour erases previous text automatically

  // Draw UI Boxes face
  //x, y, width, height, color
  tft.drawRect(0, 0, 128, 160, QDTech_BLUE); //main box
  //top boxes
  tft.drawRect(0, 0, 64, 25, QDTech_BLUE);
  tft.drawRect(64, 0, 64, 25, QDTech_BLUE);
  //bot box
  tft.drawRect(0, 135, 128, 25, QDTech_BLUE);
  //steps
  tft.setCursor(3, 3);
  tft.print("Steps: ");
  UpdateSteps(0);

  //battery
  tft.setCursor(68, 3);
  tft.print("Battery: ");
  UpdateBattery(100);

  //time
  tft.setCursor (34, 150);
  tft.print(__DATE__);
  targetTime = millis() + 1000;
}

void loop() {
  steps++;

  UpdateSteps(steps);   //Updates steps UI

  if (battery_level < 0) {
    battery_level = 100;
  }

  UpdateBattery(battery_level); //Updates battery UI

  if (digitalRead(BTN1)) {
    analogWrite(PIEZO, HIGH);
    tft.fillCircle(64, 80, 40, QDTech_RED);
    analogWrite(PIEZO, LOW);
  }
  if (digitalRead(BTN2)) {
    analogWrite(PIEZO, HIGH);
    tft.fillCircle(64, 80, 40, QDTech_BLACK);
    analogWrite(PIEZO, LOW);
  }
  UpdateClock();
}

void UpdateClock(void) {
  if (targetTime < millis()) {
    targetTime = millis() + 1000;
    ss++;              // Advance second
    battery_level--;
    if (ss == 60) {
      ss = 0;
      mm++;            // Advance minute
      if (mm > 59) {
        mm = 0;
        hh++;          // Advance hour
        if (hh > 23) {
          hh = 0;
        }
      }
    }

    // Update digital time
    tft.setCursor (34, 138);

    if (hh > 12) {
      if (hh < 22) tft.print('0');
      tft.print (hh - 12);
    } else {
      if (hh < 10) tft.print('0');
      tft.print (hh);
    }
    tft.print (':');
    if (mm < 10) tft.print('0');
    tft.print (mm);
    tft.print (':');
    if (ss < 10) tft.print('0');
    tft.print (ss);
    if (hh > 12) tft.print(" pm"); else tft.print (" am");
  }
}
void UpdateSteps(int steps_taken) {
  tft.setCursor(3, 15);
  tft.print(steps_taken);
}

void UpdateBattery(int battery_level) {
  tft.setCursor(68, 15);
  if (battery_level < 100) {
    tft.print(" ");
  }

  tft.print(battery_level);
  tft.print("%");
}

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

