/*
  An example input and GUI routines for FitPet.
 But this is just a demo.
 
 Luis Flores 11/11/14 1.0
 */
// FROM ARDUINO TO LCD PINS:
//    6-6
//    7-14
//    8-7
//    9-10

//    11-8
//    12-12
//    13-9
// ON LCD ONLY:
//    13-8
//    9-11

#define PIEZO 3  //piezzo, pwm output
#define BTN1 5   //button 1, active high
#define BTN2 4   //button 2, active high

#define SD_CS    7  // Chip select line for SD card
#define TFT_CS  9  // Chip select line for TFT display
#define TFT_DC   8  // Data/command line for TFT
#define TFT_RST  6  // Reset line for TFT (or connect to +5V)

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_QDTech.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

Adafruit_QDTech tft = Adafruit_QDTech(TFT_CS, TFT_DC, TFT_RST);

uint32_t targetTime = 0;                    // for next 1 second timeout
uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6); // Get H, M, S from compile time

int steps = 0;
int battery_level = 100;
void setup(void) {

  Serial.begin(9600);

  tft.init();  

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("OK!");

  pinMode(BTN1, INPUT); //set the pinmodes for buttons
  pinMode(BTN2, INPUT);

  //Serial.print("Initializing SD card...");

  //Serial.println("OK!");

  tft.setRotation(0);
  tft.fillScreen(QDTech_BLACK);
  tft.setTextColor(QDTech_WHITE, QDTech_BLACK);  // Adding a black background colour erases previous text automatically

  //bmpDraw("bidoof.bmp", 80, 40);

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
    bmpDraw("bidoof.bmp", 40, 40);
    tft.fillRect(2, 26, 124, 99, QDTech_BLACK);
    bmpDraw("bidoof.bmp", 40, 60);
    tft.fillRect(2, 26, 124, 99, QDTech_BLACK);
    bmpDraw("bidoof.bmp", 40, 40);
    tft.fillRect(2, 26, 124, 99, QDTech_BLACK);
    bmpDraw("bidoof.bmp", 40, 30);
    analogWrite(PIEZO, LOW);
  }
  if (digitalRead(BTN2)) {
    analogWrite(PIEZO, HIGH);
    tft.fillRect(2, 26, 124, 99, QDTech_BLACK);
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
    } 
    else {
      if (hh < 10) tft.print('0');
      tft.print (hh);
    }
    tft.print (':');
    if (mm < 10) tft.print('0');
    tft.print (mm);
    tft.print (':');
    if (ss < 10) tft.print('0');
    tft.print (ss);
    if (hh > 12) tft.print(" pm"); 
    else tft.print (" am");
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

#define BUFFPIXEL 20

void bmpDraw(char *filename, uint8_t x, uint8_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found");
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); 
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); 
    Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); 
    Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: "); 
      Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
          pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
          } // end pixel
        } // end scanline
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println("BMP format not recognized.");
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}





