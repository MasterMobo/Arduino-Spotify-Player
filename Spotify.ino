#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>


// ----------------------------------------------------------------------------------------------------------------
// TFT LCD Setup
// ----------------------------------------------------------------------------------------------------------------


#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h> // Hardware-specific library
#include <Fonts/FreeSans12pt7b.h>

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
String songName = "COCK";
String artistName = "Balls";

const int WIDTH = 480;   // Screen width
const int HEIGHT = 320;   // Screen height

const int H_WIDTH = WIDTH/2;   // Half Screen width
const int H_HEIGHT = HEIGHT/2;  // Half Screen height

const int progressBarYOffset = 256;
const int buttonsYOffset = 189;


long progress = 30000;
long duration = 60000;

bool isPlaying = false;
bool shuffleOn = true;
String repeatMode = "off";


void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  tft.reset(); 
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(3);
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextWrap(false);

  //tft.setTextPadding(WIDTH);
  pinMode(13, OUTPUT);
  fillShuffleButton();
  fillPlayButton();
  fillRepeatButton();
}

// ----------------------------------------------------------------------------------------------------------------
// BUTTONS VARIABLE SETUP------------------------------------------------------------------------

// PLAY BUTTON
int playButtonX = H_WIDTH;
int playButtonY = buttonsYOffset;
int playButtonLength = 36;  //optimally an even number
int playButtonWidth = 9;    //optimally an even number
float pauseButtonLength = 36;

// NEXT BUTTON
int nextButtonLineX = H_WIDTH + 127 - 8;
int nextButtonLineY = buttonsYOffset - 18;
int nextButtonLineLength = 36;
int nextButtonLineWidth = 8;

// PREVIOUS BUTTON
int previousButtonLineX = H_WIDTH - 127;
int previousButtonLineY = buttonsYOffset - 18;
int previousButtonLineLength = 36; 
int previousButtonLineWidth = 8;

// SHUFFLE BUTTON
int shuffleButtonX = 20;
int shuffleButtonY = buttonsYOffset;
float shuffleButtonLength = 42;
float shuffleButtonWidth = 36;
float shuffleButtonCrossWidth = 36 - (36 / 3) + 4;
float shuffleButttonThickness = 4;

// REPEAT BUTTON
int repeatButtonX = WIDTH - 62;
int repeatButtonY =  buttonsYOffset - 18;
int repeatButtonlength = 42;
int repeatButtonWidth = 36;
int repeatButtonThickness = 4;

// PROGRESS BAR
float barLength = WIDTH - 30;
float barWidth = 10;

int scrollOffset = 0;

// ----------------------------------------------------------------------------------------------------------------


void loop() {


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
      inData.remove(inData.length() - 1, 1);
      bool newPlayState;
      newPlayState = (inData == "1");
      if (isPlaying != newPlayState) {
        fillPlayButton();
        isPlaying = newPlayState;
      } 
      
      inData="";
    }
    if (recieved == '$'){
      inData.remove(inData.length() - 1, 1);
      progress = inData.toInt();
      inData="";
    }
    if (recieved == '%'){
      
      duration = inData.toInt();
      inData="";
    }
    if (recieved == '^'){
      inData.remove(inData.length() - 1, 1);
      if (repeatMode != inData){
        fillRepeatButton();
      }
      
      repeatMode = inData;
      inData="";
    }
    if (recieved == '&'){
      inData.remove(inData.length() - 1, 1);
      bool newShuffleState;
      newShuffleState = (inData == "1");
      if (shuffleOn != newShuffleState) {
        fillShuffleButton();
        shuffleOn = newShuffleState;
      } 

      inData="";
    }
  
}



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
          // Serial.println("===================================");
          // Serial.println(p.x);
          // Serial.println(p.y);
          // Serial.println("===================================");

          // //Quality check
          // tft.setFont(&FreeSans12pt7b);
          // tft.setTextSize(1); 
          // tft.setTextColor(0xC618,0x0000);
          // tft.setCursor(H_WIDTH - 40, H_HEIGHT - 20);
          // tft.println(p.x);
          // tft.println(p.y);


          //Play button
          if ((p.x > 430) && (p.x < 730) && (p.y>430) && (p.y<600)) {
            if (isPlaying){
              Serial.println("pause");
            } else {
              Serial.println("play");
            }
            delay(200);
          }

          //Previous button
          else if ((p.x > 546) && (p.x < 663) && (p.y>283) && (p.y<373)) {
            Serial.println("prev");
            delay(200);
          }

          //Next button
          else if ((p.x > 546) && (p.x < 663) && (p.y>675) && (p.y<775)) {
            Serial.println("next");
            delay(200);
          }

          // Repeat button
          else if ((p.x > 532) && (p.x < 700) && (p.y>876) && (p.y<941)) {
            Serial.print("repeat");
            if (repeatMode == "off"){
              Serial.print("context");
            } else if (repeatMode == "context"){
              Serial.print("track");
            } else if (repeatMode == "track"){
              Serial.print("off");
            }
  
            
            Serial.println();
            delay(200);
          }
          
          // Shuffle button
          else if ((p.x > 552) && (p.x < 662) && (p.y>122) && (p.y<212)) {
            if (shuffleOn){
              Serial.println("shuffleOff");
            } else {
              Serial.println("shuffleOn");
            }
            delay(200);
          }



        }

  // Update song data:
  
//LCD CONTOL=====================================================================================================
          // tft.fillRectVGradient(0, 0, 320, 240, tft.color565(10, 10, 7), tft.color565(26, 65, 1) );



          // Song name
          if (songName.length() <= 8)
          {
            tft.setTextColor(0xFFFF,0x0000);  
            tft.setTextSize(9);
            tft.setCursor(20, 30);
            tft.println(songName + "         ");
          }
          else
          {
            tft.setFont();
            tft.setTextColor(0xFFFF,0x0000);  
            tft.setTextSize(9);
            tft.setCursor(0, 30);
            scrollText(" " + songName + " ", 9, scrollOffset);
            scrollOffset += 1;
            if (scrollOffset >= songName.length() + 2 - 1)
            {
              scrollOffset = 0;
            }

          }


          //Artist Name
            tft.setTextColor(0xC618,0x0000);  
            tft.setTextSize(3);
            tft.setCursor(23, 108);
            tft.println(artistName + "               ");







          //Next Button
          tft.fillTriangle(nextButtonLineX - nextButtonLineLength + (nextButtonLineWidth / 2) + 2, buttonsYOffset - (nextButtonLineLength / 2), nextButtonLineX - nextButtonLineLength + (nextButtonLineWidth / 2) + 2, buttonsYOffset + (nextButtonLineLength / 2), nextButtonLineX + (nextButtonLineWidth / 2) + 2, buttonsYOffset, TFT_WHITE);
          tft.fillRect(nextButtonLineX, nextButtonLineY, nextButtonLineWidth, nextButtonLineLength, TFT_WHITE);



          //Previous Button
          tft.fillTriangle(previousButtonLineX + previousButtonLineLength, buttonsYOffset - (previousButtonLineLength / 2), previousButtonLineX + previousButtonLineLength, buttonsYOffset + (previousButtonLineLength / 2), previousButtonLineX, buttonsYOffset, TFT_WHITE);
          tft.fillRect(previousButtonLineX, previousButtonLineY, previousButtonLineWidth, previousButtonLineLength, TFT_WHITE);







          

          // Song Progress
          tft.setTextSize(2); 
          tft.setTextColor(0xC618,0x0000);
          tft.setCursor(20, progressBarYOffset + 10);           //a digit is a 10X13 pixel
          tft.print((progress / 1000) / 60);
          tft.print(":");
          printDigits((progress / 1000) % 60);



          // Song Duration
          tft.setCursor(WIDTH - 65, progressBarYOffset + 10);   //a digit is a 10X13 pixel
          tft.print((duration / 1000) / 60);
          tft.print(":");
          printDigits((duration / 1000) % 60);



          //Progress bar
          //refresh bar
          tft.fillRect(15,progressBarYOffset-10,barLength+5,20, TFT_BLACK);
          //background bar
          tft.fillRoundRect(15, progressBarYOffset - (barWidth / 2), barLength, barWidth, barWidth / 2,TFT_DARKGREY);
          //progress bar
          float xPos = barLength*progress/duration;
          tft.fillRoundRect(15, progressBarYOffset - (barWidth / 2), xPos, barWidth, barWidth / 2, TFT_WHITE);



          //Guide lines
          // tft.fillRect(0,HEIGHT * 2 / 5,WIDTH,1, TFT_WHITE);
          // tft.fillRect(0,189,WIDTH,1, TFT_WHITE);
          // tft.fillRect(0,HEIGHT - (HEIGHT / 5),WIDTH,1, TFT_WHITE);
          // tft.fillRect(0,HEIGHT - (HEIGHT / 5) + 10,WIDTH,1, TFT_WHITE);



          // //Quality check
          // tft.setFont(&FreeSans12pt7b);
          // tft.setTextSize(1); 
          // tft.setTextColor(0xC618,0x0000);
          // tft.setCursor(H_WIDTH - 40, H_HEIGHT - 20);
          // tft.print((3000 / 1000) % 60);

}


// ----------------------------------------------------------------------------------------------------------------
// ADDITIONAL FUNCTIONS


//Function to draw a diagonnal line:
void fillDiagonalLine(int startpointx, int startpointy, int length, int width, int angle, int color)
{
  //Horizontal line coordinates
    //A
  int Ax = startpointx;
  int Ay = startpointy + width/2;
    //B
  int Bx = startpointx;
  int By = startpointy - width/2;
    //C
  int Cx = startpointx + length;
  int Cy = startpointy - width/2;
    //D
  int Dx = startpointx + length;
  int Dy = startpointy + width/2;
  //Converts the coordinates according to the angle
  float angle1 = (angle * M_PI) / 180;
    //A1
  float A1x = Ax * cos(angle1) - Ay * sin(angle1);
  float A1y = Ay * cos(angle1) + Ax * sin(angle1);
    //B1
  float B1x = Bx * cos(angle1) - By * sin(angle1);
  float B1y = By * cos(angle1) + Bx * sin(angle1);
    //C1
  float C1x = Cx * cos(angle1) - Cy * sin(angle1);
  float C1y = Cy * cos(angle1) + Cx * sin(angle1);
    //D1
  float D1x = Dx * cos(angle1) - Dy * sin(angle1);
  float D1y = Dy * cos(angle1) + Dx * sin(angle1);
    //startpoint1
  int startpoint1x = (A1x + B1x) / 2;
  int startpoint1y = (A1y + B1y) / 2;
    //offset
  int offsetx = startpoint1x - startpointx;
  int offsety = startpoint1y - startpointy;
  //Draws the diagonal line
  tft.fillTriangle(A1x - offsetx, A1y - offsety, B1x - offsetx, B1y - offsety, D1x - offsetx, D1y- offsety, color);
  tft.fillTriangle(D1x - offsetx, D1y - offsety, C1x - offsetx, C1y - offsety, B1x - offsetx, B1y - offsety, color);
  //Quality check
  // tft.setFont(&FreeSans12pt7b);
  // tft.setTextSize(1); 
  // tft.setTextColor(0xC618,0x0000);
  // tft.setCursor(H_WIDTH - 40, H_HEIGHT - 20);
  // tft.print(Ax);
  // tft.print(" ");
  // tft.print(Ay);
  // tft.print(" ");
  // tft.print(A1x);
  // tft.print(" ");
  // tft.print(A1y);
}


//Function to print 2-digits minutes and seconds:
void printDigits(int digits)
{
  // utility function for digital clock display: prints leading 0
   if(digits < 10)
    tft.print('0');
   tft.print(digits);
}


//Function to scroll texts while using retarded font
void scrollText(String str, int w, int offset)
{
  // Set up
  const int width = w; // width of the marquee display (in characters)
  
  // Loop once through the string

  // Construct the string to display for this iteration
    String t = "";

    for (int i = 0; i < width; i++)
      t += str.charAt((offset + i) % str.length());

      // Print the string for this iteration
      tft.print(t);


}


void fillPlayButton() {
          //Play button
            //base circle
          tft.fillCircle(H_WIDTH, buttonsYOffset, 39, TFT_WHITE);
            if (!isPlaying)  //play bars
          {   
            tft.fillRect(H_WIDTH - playButtonWidth - (playButtonWidth / 2) , buttonsYOffset - (playButtonLength / 2), playButtonWidth, playButtonLength, TFT_BLACK);
            tft.fillRect(H_WIDTH + (playButtonWidth / 2) + 1, buttonsYOffset - (playButtonLength / 2), playButtonWidth, playButtonLength, TFT_BLACK);
          } 
          else  //pause triangle
          {            
            tft.fillTriangle(H_WIDTH - (pauseButtonLength * 1 / 3), buttonsYOffset - ((pauseButtonLength * 2 / sqrt(3)) / 2), H_WIDTH - (pauseButtonLength * 1 / 3), buttonsYOffset + ((pauseButtonLength * 2 / sqrt(3)) / 2), H_WIDTH + (pauseButtonLength * 2 / 3), buttonsYOffset, TFT_BLACK);
          }

}

void fillShuffleButton(){
            //Shuffle button
          if (!shuffleOn)
          {
            //refresh rect
            tft.fillRect(shuffleButtonX - 2, shuffleButtonY - (shuffleButtonWidth /2) - 5, shuffleButtonLength + 5, shuffleButtonWidth * 2, TFT_BLACK);
            //left rects 
            tft.fillRoundRect(shuffleButtonX, shuffleButtonY - (shuffleButtonCrossWidth / 2), shuffleButtonLength / 3, shuffleButttonThickness, shuffleButttonThickness / 2, TFT_GREEN);
            tft.fillRoundRect(shuffleButtonX, shuffleButtonY + (shuffleButtonCrossWidth / 2) - shuffleButttonThickness, shuffleButtonLength / 3, shuffleButttonThickness, shuffleButttonThickness / 2, TFT_GREEN);
            //right rects
            tft.fillRoundRect(shuffleButtonX + shuffleButtonLength - shuffleButtonLength / 3, shuffleButtonY - (shuffleButtonCrossWidth / 2), shuffleButtonLength / 3, shuffleButttonThickness, shuffleButttonThickness / 2, TFT_GREEN);
            tft.fillRoundRect(shuffleButtonX + shuffleButtonLength - shuffleButtonLength / 3, shuffleButtonY + (shuffleButtonCrossWidth / 2) - shuffleButttonThickness, shuffleButtonLength / 3, shuffleButttonThickness, shuffleButttonThickness / 2, TFT_GREEN);
            //up to down line
            tft.fillCircle(shuffleButtonX + (shuffleButtonLength / 3) - 2, shuffleButtonY - (shuffleButtonCrossWidth / 2) + (shuffleButttonThickness / 2), shuffleButttonThickness / 2, TFT_GREEN);
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - shuffleButtonLength / 3, shuffleButtonY + (shuffleButtonCrossWidth / 2) - (shuffleButttonThickness / 2) - 1, shuffleButttonThickness / 2, TFT_GREEN);
            fillDiagonalLine(shuffleButtonX + (shuffleButtonLength / 3), shuffleButtonY - (shuffleButtonCrossWidth / 2) + (shuffleButttonThickness / 2) - 1, sqrt( sq(shuffleButtonCrossWidth - shuffleButttonThickness) + sq(shuffleButtonLength / 3)) + 2, shuffleButttonThickness, 90 - (atan((shuffleButtonLength / 3) / (shuffleButtonCrossWidth - shuffleButttonThickness)) * 180 / M_PI), TFT_GREEN);
            //down to up line
            tft.fillCircle(shuffleButtonX + (shuffleButtonLength / 3) - 2, shuffleButtonY + (shuffleButtonCrossWidth / 2) - (shuffleButttonThickness / 2) - 1, shuffleButttonThickness / 2, TFT_GREEN);
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - shuffleButtonLength / 3, shuffleButtonY - (shuffleButtonCrossWidth / 2) + (shuffleButttonThickness / 2), shuffleButttonThickness / 2, TFT_GREEN);
            fillDiagonalLine(shuffleButtonX + (shuffleButtonLength / 3) - 2, shuffleButtonY + (shuffleButtonCrossWidth / 2) - (shuffleButttonThickness / 2), sqrt( sq(shuffleButtonCrossWidth - shuffleButttonThickness) + sq(shuffleButtonLength / 3)) + 2, shuffleButttonThickness, - (90 - (atan((shuffleButtonLength / 3) / (shuffleButtonCrossWidth - shuffleButttonThickness)) * 180 / M_PI)), TFT_GREEN);
            //upper arrow
            fillDiagonalLine(shuffleButtonX + shuffleButtonLength - (shuffleButttonThickness / 4) + 1, shuffleButtonY - (shuffleButtonCrossWidth / 2) + (shuffleButttonThickness / 4) + 1, sqrt(sq((shuffleButtonLength / 3) / 2) + sq((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4))), shuffleButttonThickness, 180 + (atan(((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4)) / ((shuffleButtonLength / 3) / 2)) * 180 / M_PI) + 1, TFT_GREEN);
            fillDiagonalLine(shuffleButtonX + shuffleButtonLength - (shuffleButttonThickness / 4) + 2, shuffleButtonY - (shuffleButtonCrossWidth / 2) + (shuffleButttonThickness / 4) + 1, sqrt(sq((shuffleButtonLength / 3) / 2) + sq((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4))), shuffleButttonThickness, 180 - (atan(((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4)) / ((shuffleButtonLength / 3) / 2)) * 180 / M_PI) - 1, TFT_GREEN);
            //upper arrow fillers
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - (shuffleButttonThickness / 4) + 1, shuffleButtonY - (shuffleButtonCrossWidth / 2) + (shuffleButttonThickness / 2), shuffleButttonThickness / 2, TFT_GREEN);
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - (shuffleButtonLength / 3 / 2) + 1, shuffleButtonY - (shuffleButtonWidth / 2), shuffleButttonThickness / 2, TFT_GREEN);
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - (shuffleButtonLength / 3 / 2) + 1, shuffleButtonY - (shuffleButtonWidth / 2) + (shuffleButtonWidth / 3), shuffleButttonThickness / 2, TFT_GREEN);
            //lower arrow
            fillDiagonalLine(shuffleButtonX + shuffleButtonLength - (shuffleButttonThickness / 4) + 1, shuffleButtonY + (shuffleButtonCrossWidth / 2) - (shuffleButttonThickness / 4), sqrt(sq((shuffleButtonLength / 3) / 2) + sq((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4))), shuffleButttonThickness, 180 + (atan(((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4)) / ((shuffleButtonLength / 3) / 2)) * 180 / M_PI) + 1, TFT_GREEN);
            fillDiagonalLine(shuffleButtonX + shuffleButtonLength - (shuffleButttonThickness / 4) + 2, shuffleButtonY + (shuffleButtonCrossWidth / 2) - (shuffleButttonThickness / 4) - 1, sqrt(sq((shuffleButtonLength / 3) / 2) + sq((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4))), shuffleButttonThickness, 180 - (atan(((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4)) / ((shuffleButtonLength / 3) / 2)) * 180 / M_PI) - 1, TFT_GREEN);
            //lower arrow fillers
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - (shuffleButttonThickness / 4) + 1, shuffleButtonY + (shuffleButtonCrossWidth / 2) - (shuffleButttonThickness / 2), shuffleButttonThickness / 2, TFT_GREEN);
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - (shuffleButtonLength / 3 / 2) + 1, shuffleButtonY + (shuffleButtonWidth / 2) - (shuffleButtonWidth / 3), shuffleButttonThickness / 2, TFT_GREEN);
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - (shuffleButtonLength / 3 / 2) + 1, shuffleButtonY + (shuffleButtonWidth / 2), shuffleButttonThickness / 2, TFT_GREEN);
            //on button
            tft.fillCircle(shuffleButtonX + (shuffleButtonLength / 2), shuffleButtonY + shuffleButtonWidth, shuffleButttonThickness - 1, TFT_GREEN);
          } 
          else 
          {
            //refresh rect
            tft.fillRect(shuffleButtonX - 2, shuffleButtonY - (shuffleButtonWidth /2) - 5, shuffleButtonLength + 5, shuffleButtonWidth * 2, TFT_BLACK);
            //left rects 
            tft.fillRoundRect(shuffleButtonX, shuffleButtonY - (shuffleButtonCrossWidth / 2), shuffleButtonLength / 3, shuffleButttonThickness, shuffleButttonThickness / 2, TFT_WHITE);
            tft.fillRoundRect(shuffleButtonX, shuffleButtonY + (shuffleButtonCrossWidth / 2) - shuffleButttonThickness, shuffleButtonLength / 3, shuffleButttonThickness, shuffleButttonThickness / 2, TFT_WHITE);
            //right rects
            tft.fillRoundRect(shuffleButtonX + shuffleButtonLength - shuffleButtonLength / 3, shuffleButtonY - (shuffleButtonCrossWidth / 2), shuffleButtonLength / 3, shuffleButttonThickness, shuffleButttonThickness / 2, TFT_WHITE);
            tft.fillRoundRect(shuffleButtonX + shuffleButtonLength - shuffleButtonLength / 3, shuffleButtonY + (shuffleButtonCrossWidth / 2) - shuffleButttonThickness, shuffleButtonLength / 3, shuffleButttonThickness, shuffleButttonThickness / 2, TFT_WHITE);
            //up to down line
            tft.fillCircle(shuffleButtonX + (shuffleButtonLength / 3) - 2, shuffleButtonY - (shuffleButtonCrossWidth / 2) + (shuffleButttonThickness / 2), shuffleButttonThickness / 2, TFT_WHITE);
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - shuffleButtonLength / 3, shuffleButtonY + (shuffleButtonCrossWidth / 2) - (shuffleButttonThickness / 2) - 1, shuffleButttonThickness / 2, TFT_WHITE);
            fillDiagonalLine(shuffleButtonX + (shuffleButtonLength / 3), shuffleButtonY - (shuffleButtonCrossWidth / 2) + (shuffleButttonThickness / 2) - 1, sqrt( sq(shuffleButtonCrossWidth - shuffleButttonThickness) + sq(shuffleButtonLength / 3)) + 2, shuffleButttonThickness, 90 - (atan((shuffleButtonLength / 3) / (shuffleButtonCrossWidth - shuffleButttonThickness)) * 180 / M_PI), TFT_WHITE);
            //down to up line
            tft.fillCircle(shuffleButtonX + (shuffleButtonLength / 3) - 2, shuffleButtonY + (shuffleButtonCrossWidth / 2) - (shuffleButttonThickness / 2) - 1, shuffleButttonThickness / 2, TFT_WHITE);
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - shuffleButtonLength / 3, shuffleButtonY - (shuffleButtonCrossWidth / 2) + (shuffleButttonThickness / 2), shuffleButttonThickness / 2, TFT_WHITE);
            fillDiagonalLine(shuffleButtonX + (shuffleButtonLength / 3) - 2, shuffleButtonY + (shuffleButtonCrossWidth / 2) - (shuffleButttonThickness / 2), sqrt( sq(shuffleButtonCrossWidth - shuffleButttonThickness) + sq(shuffleButtonLength / 3)) + 2, shuffleButttonThickness, - (90 - (atan((shuffleButtonLength / 3) / (shuffleButtonCrossWidth - shuffleButttonThickness)) * 180 / M_PI)), TFT_WHITE);
            //upper arrow
            fillDiagonalLine(shuffleButtonX + shuffleButtonLength - (shuffleButttonThickness / 4) + 1, shuffleButtonY - (shuffleButtonCrossWidth / 2) + (shuffleButttonThickness / 4) + 1, sqrt(sq((shuffleButtonLength / 3) / 2) + sq((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4))), shuffleButttonThickness, 180 + (atan(((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4)) / ((shuffleButtonLength / 3) / 2)) * 180 / M_PI) + 1, TFT_WHITE);
            fillDiagonalLine(shuffleButtonX + shuffleButtonLength - (shuffleButttonThickness / 4) + 2, shuffleButtonY - (shuffleButtonCrossWidth / 2) + (shuffleButttonThickness / 4) + 1, sqrt(sq((shuffleButtonLength / 3) / 2) + sq((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4))), shuffleButttonThickness, 180 - (atan(((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4)) / ((shuffleButtonLength / 3) / 2)) * 180 / M_PI) - 1, TFT_WHITE);
            //upper arrow fillers
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - (shuffleButttonThickness / 4) + 1, shuffleButtonY - (shuffleButtonCrossWidth / 2) + (shuffleButttonThickness / 2), shuffleButttonThickness / 2, TFT_WHITE);
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - (shuffleButtonLength / 3 / 2) + 1, shuffleButtonY - (shuffleButtonWidth / 2), shuffleButttonThickness / 2, TFT_WHITE);
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - (shuffleButtonLength / 3 / 2) + 1, shuffleButtonY - (shuffleButtonWidth / 2) + (shuffleButtonWidth / 3), shuffleButttonThickness / 2, TFT_WHITE);
            //lower arrow
            fillDiagonalLine(shuffleButtonX + shuffleButtonLength - (shuffleButttonThickness / 4) + 1, shuffleButtonY + (shuffleButtonCrossWidth / 2) - (shuffleButttonThickness / 4), sqrt(sq((shuffleButtonLength / 3) / 2) + sq((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4))), shuffleButttonThickness, 180 + (atan(((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4)) / ((shuffleButtonLength / 3) / 2)) * 180 / M_PI) + 1, TFT_WHITE);
            fillDiagonalLine(shuffleButtonX + shuffleButtonLength - (shuffleButttonThickness / 4) + 2, shuffleButtonY + (shuffleButtonCrossWidth / 2) - (shuffleButttonThickness / 4) - 1, sqrt(sq((shuffleButtonLength / 3) / 2) + sq((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4))), shuffleButttonThickness, 180 - (atan(((shuffleButtonWidth / 3) / 2 + (shuffleButttonThickness / 4)) / ((shuffleButtonLength / 3) / 2)) * 180 / M_PI) - 1, TFT_WHITE);
            //lower arrow fillers
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - (shuffleButttonThickness / 4) + 1, shuffleButtonY + (shuffleButtonCrossWidth / 2) - (shuffleButttonThickness / 2), shuffleButttonThickness / 2, TFT_WHITE);
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - (shuffleButtonLength / 3 / 2) + 1, shuffleButtonY + (shuffleButtonWidth / 2) - (shuffleButtonWidth / 3), shuffleButttonThickness / 2, TFT_WHITE);
            tft.fillCircle(shuffleButtonX + shuffleButtonLength - (shuffleButtonLength / 3 / 2) + 1, shuffleButtonY + (shuffleButtonWidth / 2), shuffleButttonThickness / 2, TFT_WHITE);
          }
}


void fillRepeatButton() {
  //Repeat button
          if (repeatMode == "off")
          {
            //refresh rect
            tft.fillRect(repeatButtonX - 2, repeatButtonY + 5, repeatButtonlength + 5, repeatButtonWidth * 2, TFT_BLACK);            
            //circleline
            tft.fillRoundRect(repeatButtonX, repeatButtonY - 1, repeatButtonlength, repeatButtonWidth, repeatButtonThickness * 2, TFT_GREEN);
            tft.fillRoundRect(repeatButtonX + repeatButtonThickness, repeatButtonY + repeatButtonThickness - 1, repeatButtonlength - (repeatButtonThickness * 2), repeatButtonWidth - (repeatButtonThickness * 2), repeatButtonThickness * 2, TFT_BLACK);
            //arrow
            fillDiagonalLine(repeatButtonX + 13 + 5 + (repeatButtonThickness / 4) - 2, repeatButtonY + repeatButtonWidth + (repeatButtonThickness / 4) - 3, 12, repeatButtonThickness, -36, TFT_GREEN);
            fillDiagonalLine(repeatButtonX + 13 + 5 + (repeatButtonThickness / 4) - 2, repeatButtonY + repeatButtonWidth + (repeatButtonThickness / 4) - 3, 12, repeatButtonThickness, -37, TFT_GREEN);
            fillDiagonalLine(repeatButtonX + 13 + 5 + (repeatButtonThickness / 4) - 2, repeatButtonY + repeatButtonWidth - (repeatButtonThickness / 4) - 3, 12, repeatButtonThickness, 34, TFT_GREEN);
            //arrow fillers
            tft.fillCircle(repeatButtonX + 13 + 5 + (repeatButtonThickness / 4) - 2 + 12 - 3, repeatButtonY + repeatButtonWidth - 3 - 6,repeatButtonThickness / 2, TFT_GREEN);
            tft.fillCircle(repeatButtonX + 13 + 5 + (repeatButtonThickness / 4) - 2 + 12 - 3, repeatButtonY + repeatButtonWidth - 3 + 5,repeatButtonThickness / 2, TFT_GREEN);
            //black line cover
            tft.fillRect(repeatButtonX + (repeatButtonlength / 2) - 10, repeatButtonY + repeatButtonWidth - repeatButtonThickness - 1, 6, repeatButtonThickness + 2, TFT_BLACK);
            //on button
            tft.fillCircle(repeatButtonX + (repeatButtonlength / 2), buttonsYOffset + repeatButtonWidth, repeatButtonThickness - 1, TFT_GREEN);
          } 
          else if (repeatMode == "context")
          {
            //refresh rect
            tft.fillRect(repeatButtonX - 2, repeatButtonY + 5, repeatButtonlength + 5, repeatButtonWidth * 2, TFT_BLACK);
            //circleline
            tft.fillRoundRect(repeatButtonX, repeatButtonY - 1, repeatButtonlength, repeatButtonWidth, repeatButtonThickness * 2, TFT_GREEN);
            tft.fillRoundRect(repeatButtonX + repeatButtonThickness, repeatButtonY + repeatButtonThickness - 1, repeatButtonlength - (repeatButtonThickness * 2), repeatButtonWidth - (repeatButtonThickness * 2), repeatButtonThickness * 2, TFT_BLACK);
            //arrow
            fillDiagonalLine(repeatButtonX + 13 + 5 + (repeatButtonThickness / 4) - 2, repeatButtonY + repeatButtonWidth + (repeatButtonThickness / 4) - 3, 12, repeatButtonThickness, -36, TFT_GREEN);
            fillDiagonalLine(repeatButtonX + 13 + 5 + (repeatButtonThickness / 4) - 2, repeatButtonY + repeatButtonWidth + (repeatButtonThickness / 4) - 3, 12, repeatButtonThickness, -37, TFT_GREEN);
            fillDiagonalLine(repeatButtonX + 13 + 5 + (repeatButtonThickness / 4) - 2, repeatButtonY + repeatButtonWidth - (repeatButtonThickness / 4) - 3, 12, repeatButtonThickness, 34, TFT_GREEN);
            //arrow fillers
            tft.fillCircle(repeatButtonX + 13 + 5 + (repeatButtonThickness / 4) - 2 + 12 - 3, repeatButtonY + repeatButtonWidth - 3 - 6,repeatButtonThickness / 2, TFT_GREEN);
            tft.fillCircle(repeatButtonX + 13 + 5 + (repeatButtonThickness / 4) - 2 + 12 - 3, repeatButtonY + repeatButtonWidth - 3 + 5,repeatButtonThickness / 2, TFT_GREEN);
            //black line cover
            tft.fillRect(repeatButtonX + (repeatButtonlength / 2) - 10, repeatButtonY - 1, 14, repeatButtonThickness, TFT_BLACK);
            tft.fillRect(repeatButtonX + (repeatButtonlength / 2) - 10, repeatButtonY + repeatButtonWidth - repeatButtonThickness - 1, 6, repeatButtonThickness + 2, TFT_BLACK);
            //on button
            tft.fillCircle(repeatButtonX + (repeatButtonlength / 2), buttonsYOffset + repeatButtonWidth, repeatButtonThickness - 1, TFT_GREEN);
            //number one
            tft.setTextSize(3); 
            tft.setTextColor(GREEN,0x0000);
            tft.setCursor(repeatButtonX + (repeatButtonlength / 2) - 7, repeatButtonY - 1); 
            tft.print(1);
          }
          else if (repeatMode == "track")
          {
            //refresh rect
            tft.fillRect(repeatButtonX - 2, repeatButtonY + 5, repeatButtonlength + 5, repeatButtonWidth * 2, TFT_BLACK);
            //circleline
            tft.fillRoundRect(repeatButtonX, repeatButtonY - 1, repeatButtonlength, repeatButtonWidth, repeatButtonThickness * 2, TFT_WHITE);
            tft.fillRoundRect(repeatButtonX + repeatButtonThickness, repeatButtonY + repeatButtonThickness - 1, repeatButtonlength - (repeatButtonThickness * 2), repeatButtonWidth - (repeatButtonThickness * 2), repeatButtonThickness * 2, TFT_BLACK);
            //arrow
            fillDiagonalLine(repeatButtonX + 13 + 5 + (repeatButtonThickness / 4) - 2, repeatButtonY + repeatButtonWidth + (repeatButtonThickness / 4) - 3, 12, repeatButtonThickness, -36, TFT_WHITE);
            fillDiagonalLine(repeatButtonX + 13 + 5 + (repeatButtonThickness / 4) - 2, repeatButtonY + repeatButtonWidth + (repeatButtonThickness / 4) - 3, 12, repeatButtonThickness, -37, TFT_WHITE);
            fillDiagonalLine(repeatButtonX + 13 + 5 + (repeatButtonThickness / 4) - 2, repeatButtonY + repeatButtonWidth - (repeatButtonThickness / 4) - 3, 12, repeatButtonThickness, 34, TFT_WHITE);
            //arrow fillers
            tft.fillCircle(repeatButtonX + 13 + 5 + (repeatButtonThickness / 4) - 2 + 12 - 3, repeatButtonY + repeatButtonWidth - 3 - 6,repeatButtonThickness / 2, TFT_WHITE);
            tft.fillCircle(repeatButtonX + 13 + 5 + (repeatButtonThickness / 4) - 2 + 12 - 3, repeatButtonY + repeatButtonWidth - 3 + 5,repeatButtonThickness / 2, TFT_WHITE);
            //black line cover
            tft.fillRect(repeatButtonX + (repeatButtonlength / 2) - 10, repeatButtonY + repeatButtonWidth - repeatButtonThickness - 1, 6, repeatButtonThickness + 2, TFT_BLACK);
          }


}
// ----------------------------------------------------------------------------------------------------------------
