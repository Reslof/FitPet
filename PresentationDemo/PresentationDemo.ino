/*
  An example analogue/digital clock using a TFT LCD screen to show the 
  use of some of the drawing commands with the Adafruit_QDTech library.
  For a more accurate clock, it would be better to use the RTClib library.
  But this is just a demo. 

  This examples uses the hardware SPI only. Non-hardware SPI
  is just too slow (~8 times slower!)

  Gilchrist 6/2/2014 1.0
*/
#define d6 6 //piezzo, pwm output
#define d5 5 //button 1, active high
#define d4 4 //button 2, active high
#define sclk 13  // Don't change
#define mosi 11  // Don't change
#define cs   9
#define dc   8
#define rst  7  // you can also connect this to the Arduino reset
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_QDTech.h> // Hardware-specific library
#include <SPI.h>

Adafruit_QDTech tft = Adafruit_QDTech(cs, dc, rst);       // Invoke custom library

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osx=64, osy=64, omx=64, omy=64, ohx=64, ohy=64;  // Saved H, M, S x & y coords
uint16_t x0=0, x1=0, y0=0, y1=0;
uint32_t targetTime = 0;                    // for next 1 second timeout
uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6);  // Get H, M, S from compile time

void setup(void) {
  pinMode(d5, INPUT); //set the pinmodes for buttons
  pinMode(d4, INPUT);

  tft.init();

  tft.setRotation(0);
  tft.fillScreen(QDTech_BLACK);
  tft.setTextColor(QDTech_WHITE, QDTech_BLACK);  // Adding a black background colour erases previous text automatically
  
  // Draw UI Boxes face
              //x, y, width, height, color
  tft.drawRect(0,0,128,160,QDTech_BLUE); //main box
  //top boxes
  tft.drawRect(0,0,64, 25, QDTech_BLUE);
  tft.drawRect(64,0,64,25, QDTech_BLUE);
  //bot box
  tft.drawRect(0, 135, 128, 25, QDTech_BLUE);
  //steps
  tft.setCursor(3,3);
  tft.print("Steps: ");
  tft.setCursor(3,15);
  tft.print("0");
  //battery
  tft.setCursor(68,3);
  tft.print("Battery: ");
  tft.setCursor(68, 15);
  tft.print("100%");
  //time
  tft.setCursor (34, 150);
  tft.print(__DATE__);
  targetTime = millis() + 1000; 
}

void loop() {
  if (digitalRead(d5)){
    analogWrite(d6, HIGH);
    tft.fillCircle(64,80,40, QDTech_RED);
    analogWrite(d6, LOW);
  }
  if (digitalRead(d4)){
    analogWrite(d6, HIGH);
    tft.fillCircle(64,80,40, QDTech_BLACK);
    analogWrite(d6, LOW);
  }
  if (targetTime < millis()) {
    targetTime = millis()+1000;
    ss++;              // Advance second
    if (ss==60) {
      ss=0;
      mm++;            // Advance minute
      if(mm>59) {
        mm=0;
        hh++;          // Advance hour
        if (hh>23) {
          hh=0;
        }
      }
    }

    // Update digital time
    tft.setCursor (34, 138);

    if(hh>12) {
      if (hh<22) tft.print('0');      
      tft.print (hh-12);
    } else {
      if (hh<10) tft.print('0');
      tft.print (hh);
    }
    tft.print (':');
    if (mm<10) tft.print('0');
    tft.print (mm);
    tft.print (':');
    if (ss<10) tft.print('0');
    tft.print (ss);
    if (hh>12) tft.print(" pm"); else tft.print (" am");
  }
}

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}
