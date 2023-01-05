// ----------------------------------------------------------------------------------------------------------------
// TFT LCD Setup
// ----------------------------------------------------------------------------------------------------------------
#include "FS.h"

#include <SPI.h>

#include <TFT_eSPI.h> // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

// This is the file name used to store the touch coordinate
// calibration data. Change the name to start a new calibration.
#define CALIBRATION_FILE "/TouchCalData3"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

String inData;
String message;
String songName = "NULL";
String artistName = "NULL";

const int WIDTH = 320;   // Screen width
const int HEIGHT = 240;   // Screen height

const int H_WIDTH = WIDTH/2;   // Half Screen width
const int H_HEIGHT = HEIGHT/2;  // Half Screen height

const int progressBarYOffset = 210;
const int buttonsYOffset = 150;


long progress;
long duration;

bool isPlaying = true;
bool shuffleOn =  true;
bool repeatOn = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  tft.init(); 
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(4);
  tft.setTextColor(TFT_WHITE);
  tft.setTextPadding(WIDTH);
}
int scroll_count = 0;
void loop() {


        //TOUCH CONTROL------------------------------------------------------------------------
        uint16_t x, y;

        // See if there's any touch data
        if (tft.getTouch(&x, &y))
        {
          //Play button
          if ((x > H_WIDTH - 27) && (x < H_WIDTH + 27) && (y>buttonsYOffset-27) && (y<buttonsYOffset+27)) {
            if (isPlaying){
              Serial.println("pause");
            } else {
              Serial.println("play");
            }
            delay(200);
          }

          //Previous button
          if ((x > H_WIDTH + 50) && (x < H_WIDTH + 75) && (y>buttonsYOffset-17) && (y<buttonsYOffset+17)) {
            Serial.println("prev");
            delay(200);
          }

          //Next button
          if ((x < H_WIDTH - 50) && (x > H_WIDTH - 75) && (y>buttonsYOffset-17) && (y<buttonsYOffset+17)) {
            Serial.println("next");
            delay(200);
          }

          if ((x > 15) && (x < 15 + 35) && (y>buttonsYOffset -12) && (y<buttonsYOffset -12 + 60)) {
            if (repeatOn){
              Serial.println("repeatOff");
            } else {
              Serial.println("repeatOn");
            }
            delay(200);
          }

          if ((x > WIDTH - 15 - 35) && (x < WIDTH - 15) && (y>buttonsYOffset -20) && (y<buttonsYOffset -12 + 60)) {
            if (shuffleOn){
              Serial.println("shuffleOff");
            } else {
              Serial.println("shuffleOn");
            }
            delay(200);
          }



        }

  // put your main code here, to run repeatedly:
  while(Serial.available() > 0){
    char recieved = Serial.read();
    inData += recieved;

    if (recieved == '>'){
      inData.remove(inData.length() - 1, 1);
      songName = inData;
      inData="";
    }
    if (recieved == '@'){
      inData.remove(inData.length() - 1, 1);
      artistName = inData;
      inData="";
    }
    if (recieved == '#'){
      isPlaying = inData == "1#";
      inData="";
    }
    if (recieved == '$'){
      inData.remove(inData.length() - 1, 1);
      progress = inData.toInt();
      inData="";
    }
    if (recieved == '%'){
      inData.remove(inData.length() - 1, 1);
      duration = inData.toInt();
      inData="";
    }
    if (recieved == '^'){
      repeatOn = inData == "1^";
      inData="";
    }
    if (recieved == '&'){
      shuffleOn = inData == "1&";
      inData="";
    }
  } 

  //LCD CONTOL=====================================================================================================
          // tft.fillRectVGradient(0, 0, 320, 240, tft.color565(10, 10, 7), tft.color565(26, 65, 1) );


          //Song name
          tft.setTextFont(4);
          tft.setTextColor(TFT_WHITE,TFT_BLACK);  
          tft.setTextSize(2);
          tft.drawString(songName, 20, 20);

          //Artist Name
          tft.setTextFont(4);
          tft.setTextColor(TFT_LIGHTGREY,TFT_BLACK);  
          tft.setTextSize(1);
          tft.drawString(artistName, 20, 80);

          //Play button
          tft.fillSmoothCircle(H_WIDTH, buttonsYOffset, 27, TFT_WHITE);
          if (isPlaying){
            tft.fillRect(H_WIDTH - 11, buttonsYOffset - 12, 22, 24, TFT_BLACK);    // Pause bars
            tft.fillRect(H_WIDTH - 3, buttonsYOffset - 12, 6, 24, TFT_WHITE);
          } else {
            tft.fillTriangle(H_WIDTH - 8, buttonsYOffset-17, H_WIDTH - 8, buttonsYOffset+17, H_WIDTH + 15, buttonsYOffset, TFT_BLACK);    // Play arrow
          }


          // Next Button
          tft.fillTriangle(H_WIDTH + 50, buttonsYOffset-17, H_WIDTH + 50, buttonsYOffset+17, H_WIDTH + 75, buttonsYOffset, TFT_WHITE);
          tft.fillRect(H_WIDTH + 72, buttonsYOffset - 15, 5, 30, TFT_WHITE);

          //Previous Button
          tft.fillTriangle(H_WIDTH - 50, buttonsYOffset-17, H_WIDTH - 50, buttonsYOffset+17, H_WIDTH - 75, buttonsYOffset, TFT_WHITE);
          tft.fillRect(H_WIDTH - 75, buttonsYOffset - 15, 5, 30, TFT_WHITE);

          //Shuffle button
          if (shuffleOn){
            tft.fillRect(15, buttonsYOffset -12, 35, 5, TFT_GREEN);
            tft.fillTriangle(50, buttonsYOffset -8, 50, buttonsYOffset -18,  60, buttonsYOffset -8, TFT_GREEN);

            tft.fillRect(25, buttonsYOffset + 5, 35, 5, TFT_GREEN);
            tft.fillTriangle(15, buttonsYOffset + 5, 25, buttonsYOffset + 5, 25, buttonsYOffset + 15, TFT_GREEN);

            // tft.fillCircle(40, buttonsYOffset + 30, 3, TFT_GREEN);
          } else {
            tft.fillRect(15, buttonsYOffset -12, 35, 5, TFT_WHITE);
            tft.fillTriangle(50, buttonsYOffset -8, 50, buttonsYOffset -18,  60, buttonsYOffset -8, TFT_WHITE);

            tft.fillRect(25, buttonsYOffset + 5, 35, 5, TFT_WHITE);
            tft.fillTriangle(15, buttonsYOffset + 5, 25, buttonsYOffset + 5, 25, buttonsYOffset + 15, TFT_WHITE);
          }

          // Repeat button
          // tft.fillRect(WIDTH - 80, buttonsYOffset - 20, 50, 40, TFT_WHITE);
          if (repeatOn){
            tft.fillSmoothRoundRect(WIDTH - 60, buttonsYOffset - 18,45, 35, 5, TFT_GREEN);
            tft.fillSmoothRoundRect(WIDTH - 55, buttonsYOffset - 13, 35, 25, 5, TFT_BLACK);
            tft.fillRect(WIDTH - 45, buttonsYOffset - 8, 10, 25, TFT_BLACK);
            tft.fillTriangle(WIDTH - 40, buttonsYOffset +14, WIDTH - 33, buttonsYOffset +21, WIDTH - 33, buttonsYOffset +7, TFT_GREEN);
            // tft.fillCircle(WIDTH - 40, buttonsYOffset + 33, 3, TFT_GREEN);
          } else {
            tft.fillSmoothRoundRect(WIDTH - 60, buttonsYOffset - 18, 45, 35, 5, TFT_WHITE);
            tft.fillSmoothRoundRect(WIDTH - 55, buttonsYOffset - 13, 35, 25, 5, TFT_BLACK);
            tft.fillRect(WIDTH - 45, buttonsYOffset - 20 + 10 + 2, 15 - 5, 30 - 5, TFT_BLACK);
            tft.fillTriangle(WIDTH - 40, buttonsYOffset +14, WIDTH -33, buttonsYOffset +21, WIDTH - 33, buttonsYOffset +7, TFT_WHITE);
          }

          // Song Progress
          tft.setTextFont(2);
          tft.setTextSize(1);
          tft.setTextColor(TFT_LIGHTGREY,TFT_BLACK);
          tft.setCursor(24, progressBarYOffset - 9);
          tft.print((progress / 1000) / 60);
          tft.print(":");
          tft.print((progress / 1000) % 60);

          // Song Duration
          tft.setCursor(WIDTH-50, progressBarYOffset - 9);
          tft.print((duration / 1000) / 60);
          tft.print(":");
          tft.print((duration / 1000) % 60);

          //Progress bar
          float barWidth = WIDTH - 120;
          float xPos = barWidth*progress/duration;

          tft.fillRect(55,progressBarYOffset-10,barWidth+5,20, TFT_BLACK);

          tft.drawFastHLine(60, progressBarYOffset, barWidth, TFT_DARKGREY);
          tft.drawFastHLine(60, progressBarYOffset, xPos, TFT_WHITE);
          tft.fillCircle(xPos + 60,progressBarYOffset,5, TFT_WHITE);
  

  
}
