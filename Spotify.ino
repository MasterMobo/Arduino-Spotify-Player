// ----------------------------------------------------------------------------------------------------------------
// TFT LCD Setup
// ----------------------------------------------------------------------------------------------------------------


#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h> // Hardware-specific library
#include <Fonts\FreeSansOblique12pt7b.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

MCUFRIEND_kbv tft;

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#define LCD_CS A3   // Chip Select goes to Analog 3
#define LCD_CD A2  // Command/Data goes to Analog 2
#define LCD_WR A1  // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//Param For 3.5"
#define TS_MINX 118
#define TS_MAXX 906

#define TS_MINY 92
#define TS_MAXY 951
// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate

#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

String inData;
String message;
String songName = "NULL";
String artistName = "NULL";

const int WIDTH = 480;   // Screen width
const int HEIGHT = 320;   // Screen height

const int H_WIDTH = WIDTH/2;   // Half Screen width
const int H_HEIGHT = HEIGHT/2;  // Half Screen height

const int progressBarYOffset = 270;
const int buttonsYOffset = 200;


long progress;
long duration;

bool isPlaying = true;
bool shuffleOn =  true;
bool repeatOn = true;

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  tft.reset(); 
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(3);
  tft.fillScreen(BLACK);
  tft.setFont(&FreeSansOblique12pt7b);
  tft.setTextColor(WHITE);
  //tft.setTextPadding(WIDTH);
  pinMode(13, OUTPUT);
}
int scroll_count = 0;
void loop() {


        //TOUCH CONTROL------------------------------------------------------------------------
        digitalWrite(13, HIGH);
        TSPoint p = ts.getPoint();
        digitalWrite(13, LOW);

        // if sharing pins, you'll need to fix the directions of the touchscreen pins
        //pinMode(XP, OUTPUT);
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);
        //pinMode(YM, OUTPUT);

        // we have some minimum pressure we consider 'valid'
        // pressure of 0 means no pressing!

        if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
        {
          //Play button
          if ((p.x > H_WIDTH - 27) && (p.x < H_WIDTH + 27) && (p.y>buttonsYOffset-27) && (p.y<buttonsYOffset+27)) {
            if (isPlaying){
              Serial.println("pause");
            } else {
              Serial.println("play");
            }
            delay(200);
          }

          //Previous button
          if ((p.x > H_WIDTH + 50) && (p.x < H_WIDTH + 75) && (p.y>buttonsYOffset-17) && (p.y<buttonsYOffset+17)) {
            Serial.println("prev");
            delay(200);
          }

          //Next button
          if ((p.x < H_WIDTH - 50) && (p.x > H_WIDTH - 75) && (p.y>buttonsYOffset-17) && (p.y<buttonsYOffset+17)) {
            Serial.println("next");
            delay(200);
          }

          if ((p.x > 15) && (p.x < 15 + 35) && (p.y>buttonsYOffset -12) && (p.y<buttonsYOffset -12 + 60)) {
            if (repeatOn){
              Serial.println("repeatOff");
            } else {
              Serial.println("repeatOn");
            }
            delay(200);
          }

          if ((p.x > WIDTH - 15 - 35) && (p.x < WIDTH - 15) && (p.y>buttonsYOffset -20) && (p.y<buttonsYOffset -12 + 60)) {
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
          tft.setFont(&FreeSansOblique12pt7b);
          tft.setTextColor(0xFFFF,0x0000);  
          tft.setTextSize(3);
          tft.setCursor(20, 80);
          tft.println(songName);

          //Artist Name
          tft.setFont(&FreeSansOblique12pt7b);
          tft.setTextColor(0xC618,0x0000);  
          tft.setTextSize(1);
          tft.setCursor(20, 120);
          tft.println(artistName);

          //Play button
          tft.fillCircle(H_WIDTH, buttonsYOffset, 35, TFT_WHITE);
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
            tft.fillRoundRect(WIDTH - 60, buttonsYOffset - 18,45, 35, 5, TFT_GREEN);
            tft.fillRoundRect(WIDTH - 55, buttonsYOffset - 13, 35, 25, 5, TFT_BLACK);
            tft.fillRect(WIDTH - 45, buttonsYOffset - 8, 10, 25, TFT_BLACK);
            tft.fillTriangle(WIDTH - 40, buttonsYOffset +14, WIDTH - 33, buttonsYOffset +21, WIDTH - 33, buttonsYOffset +7, TFT_GREEN);
            // tft.fillCircle(WIDTH - 40, buttonsYOffset + 33, 3, TFT_GREEN);
          } else {
            tft.fillRoundRect(WIDTH - 60, buttonsYOffset - 18, 45, 35, 5, TFT_WHITE);
            tft.fillRoundRect(WIDTH - 55, buttonsYOffset - 13, 35, 25, 5, TFT_BLACK);
            tft.fillRect(WIDTH - 45, buttonsYOffset - 20 + 10 + 2, 15 - 5, 30 - 5, TFT_BLACK);
            tft.fillTriangle(WIDTH - 40, buttonsYOffset +14, WIDTH -33, buttonsYOffset +21, WIDTH - 33, buttonsYOffset +7, TFT_WHITE);
          }

          // Song Progress
          tft.setFont(&FreeSansOblique12pt7b);
          tft.setTextSize(1); 
          tft.setTextColor(0xC618,0x0000);
          tft.setCursor(15, progressBarYOffset - 2);
          tft.print((progress / 1000) / 60);
          tft.print(":");
          tft.println((progress / 1000) % 60);

          // Song Duration
          tft.setCursor(WIDTH-50, progressBarYOffset - 2);
          tft.print((progress / 1000) / 60);
          tft.print(":");
          tft.println((progress / 1000) % 60);

          //Progress bar
          float barWidth = WIDTH - 120;
          float xPos = barWidth*progress/duration;

          tft.fillRect(55,progressBarYOffset-10,barWidth+5,20, TFT_BLACK);

          tft.writeFastHLine(60, progressBarYOffset, barWidth, TFT_DARKGREY);
          tft.writeFastHLine(60, progressBarYOffset, xPos, TFT_WHITE);
          tft.fillCircle(60,progressBarYOffset,5, TFT_WHITE);
  

  
}
