// ----------------------------------------------------------------------------------------------------------------
// TFT LCD Setup
// ----------------------------------------------------------------------------------------------------------------

#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
TFT_eSprite playButton = TFT_eSprite(&tft);

#define TFT_GREY 0x5AEB // New colour

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   33  // Chip select control pin
#define TFT_DC   17  // Data Command control pin
#define TFT_RST  32 // Reset pin (could connect to RST pin)

// #define TOUCH_CS 35     // Chip select pin (T_CS) of touch screen

#define TFT_WR 4    // Write strobe for modified Raspberry Pi TFT only
#define TFT_RD    2

#define TFT_D0   12  // Must use pins in the range 0-31 for the data bus
#define TFT_D1   13  // so a single register write sets/clears all bits
#define TFT_D2   26
#define TFT_D3   25
#define TFT_D4   21
#define TFT_D5   5
#define TFT_D6   27
#define TFT_D7   14

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT



// ----------------------------------------------------------------------------------------------------------------
// API Setup
// ----------------------------------------------------------------------------------------------------------------

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include <WiFiClientSecure.h>

#include <SpotifyArduino.h>
#include <SpotifyArduinoCert.h>
#include <ArduinoJson.h>

//------- Replace the following! ------

char ssid[] = "Bin12345";         // your network SSID (name)
char password[] = "bin12345"; // your network password

char clientId[] = "6b426bc4f03b46d2a1990f2de1c3b763";     // Your client ID of your spotify APP
char clientSecret[] = "87d31dd4ffea4ad2b72bdbe2b93b227e"; // Your client Secret of your spotify APP (Do Not share this!)

// Country code, including this is advisable
#define SPOTIFY_MARKET "VN"

#define SPOTIFY_REFRESH_TOKEN "AQAiNb9WiXrDwOlKV3f4-P8Too6sHJtbbR4fOsWSFzyjISrcBEmKFa_tyPBc8V8dWKWNp9KD_7-YShPfgj2cLIlFKperA4cZhpwQf2ANtGQOLLhsd3wL4o5zCxIy_iaVtrU"

//------- ---------------------- ------

WiFiClientSecure client;
SpotifyArduino spotify(client, clientId, clientSecret, SPOTIFY_REFRESH_TOKEN);

unsigned long delayBetweenRequests = 500; // Time between requests (miliseconds)
unsigned long requestDueTime;               //time when request due


//----------------------------------------------------------------------------------------------------------------
// SETUP
//----------------------------------------------------------------------------------------------------------------

const int WIDTH = 320;   // Screen width
const int HEIGHT = 240;   // Screen height

const int H_WIDTH = WIDTH/2;   // Half Screen width
const int H_HEIGHT = HEIGHT/2;  // Half Screen height

const int progressBarYOffset = 210;
const int buttonsYOffset = 150;

const char* songName;
const char* artistName;

long progress = 0;
long duration = 60000 * 3;

bool isPlaying = true;
bool shuffleOn =  true;
bool repeatOn = true;


void setup()
{

    Serial.begin(115200);
    tft.init();
    tft.setRotation(3);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Handle HTTPS Verification
#if defined(ESP8266)
    client.setFingerprint(SPOTIFY_FINGERPRINT); // These expire every few months
#elif defined(ESP32)
    client.setCACert(spotify_server_cert);
#endif
    // ... or don't!
    //client.setInsecure();

    Serial.println("Refreshing Access Tokens");
    if (!spotify.refreshAccessToken())
    {
        Serial.println("Failed to get access tokens");
    }
}




void printCurrentlyPlayingToSerial(CurrentlyPlaying currentlyPlaying)
{
    // Use the details in this method or if you want to store them
    // make sure you copy them (using something like strncpy)
    // const char* artist =

    // Serial.println("--------- Currently Playing ---------");

    // Serial.print("Is Playing: ");
    // if (currentlyPlaying.isPlaying)
    // {
    //     Serial.println("Yes");
    // }
    // else
    // {
    //     Serial.println("No");
    // }

    // Serial.print("Track: ");
    // Serial.println(currentlyPlaying.trackName);

    songName = currentlyPlaying.trackName;
    isPlaying = currentlyPlaying.isPlaying;
    

    // Serial.print("Track URI: ");
    // Serial.println(currentlyPlaying.trackUri);
    // Serial.println();

    // Serial.println("Artists: ");
    // for (int i = 0; i < currentlyPlaying.numArtists; i++)
    // {
    //     Serial.print("Name: ");
    //     Serial.println(currentlyPlaying.artists[i].artistName);
    //     Serial.print("Artist URI: ");
    //     Serial.println(currentlyPlaying.artists[i].artistUri);
    //     Serial.println();
    // }

    artistName = currentlyPlaying.artists[0].artistName;

    // Serial.print("Album: ");
    // Serial.println(currentlyPlaying.albumName);
    // Serial.print("Album URI: ");
    // Serial.println(currentlyPlaying.albumUri);
    // Serial.println();

    progress = currentlyPlaying.progressMs; // duration passed in the song
    duration = currentlyPlaying.durationMs; // Length of Song
    // Serial.print("Elapsed time of song (ms): ");
    // Serial.print(progress);
    // Serial.print(" of ");
    // Serial.println(duration);
    // Serial.println();

    // float percentage = ((float)progress / (float)duration) * 100;
    // int clampedPercentage = (int)percentage;
    // Serial.print("<");
    // for (int j = 0; j < 50; j++)
    // {
    //     if (clampedPercentage >= (j * 2))
    //     {
    //         Serial.print("=");
    //     }
    //     else
    //     {
    //         Serial.print("-");
    //     }
    // }
    // Serial.println(">");
    // Serial.println();

    // // will be in order of widest to narrowest
    // // currentlyPlaying.numImages is the number of images that
    // // are stored
    // for (int i = 0; i < currentlyPlaying.numImages; i++)
    // {
    //     Serial.println("------------------------");
    //     Serial.print("Album Image: ");
    //     Serial.println(currentlyPlaying.albumImages[i].url);
    //     Serial.print("Dimensions: ");
    //     Serial.print(currentlyPlaying.albumImages[i].width);
    //     Serial.print(" x ");
    //     Serial.print(currentlyPlaying.albumImages[i].height);
    //     Serial.println();
    // }
    // Serial.println("------------------------");
}

void printPlayerDetailsToSerial(PlayerDetails playerDetails)
{
    // Serial.println("--------- Player Details ---------");

    // Serial.print("Device ID: ");
    // Serial.println(playerDetails.device.id);

    // Serial.print("Device Name: ");
    // Serial.println(playerDetails.device.name);

    // Serial.print("Device Type: ");
    // Serial.println(playerDetails.device.type);

    // Serial.print("Is Active: ");
    // if (playerDetails.device.isActive)
    // {
    //     Serial.println("Yes");
    // }
    // else
    // {
    //     Serial.println("No");
    // }

    // Serial.print("Is Resticted: ");
    // if (playerDetails.device.isRestricted)
    // {
    //     Serial.println("Yes, from API docs \"no Web API commands will be accepted by this device\"");
    // }
    // else
    // {
    //     Serial.println("No");
    // }

    // Serial.print("Is Private Session: ");
    // if (playerDetails.device.isPrivateSession)
    // {
    //     Serial.println("Yes");
    // }
    // else
    // {
    //     Serial.println("No");
    // }

    // Serial.print("Volume Percent: ");
    // Serial.println(playerDetails.device.volumePercent);

    // Serial.print("Progress (Ms): ");
    // Serial.println(playerDetails.progressMs);

    // Serial.print("Is Playing: ");
    // if (playerDetails.isPlaying)
    // {
    //     Serial.println("Yes");
    // }
    // else
    // {
    //     Serial.println("No");
    // }
  

    // Serial.print("Shuffle State: ");
    // if (playerDetails.shuffleState)
    // {
    //     Serial.println("On");
    // }
    // else
    // {
    //     Serial.println("Off");
    // }
        shuffleOn = playerDetails.shuffleState;

    // Serial.print("Repeat State: ");
    // switch (playerDetails.repeateState)
    // {
    // case repeat_track:
    //     Serial.println("track");
    //     break;
    // case repeat_context:
    //     Serial.println("context");
    //     break;
    // case repeat_off:
    //     Serial.println("off");
    //     break;
    // }

    switch (playerDetails.repeateState)
    {
    case repeat_track:
        repeatOn = true;
        break;
    case repeat_context:
        repeatOn = true;
        break;
    case repeat_off:
        repeatOn = false;
        break;
    }

    // Serial.println("------------------------");
}


//----------------------------------------------------------------------------------------------------------------
// LOOP
//----------------------------------------------------------------------------------------------------------------

void loop()
{
    if (millis() > requestDueTime)
    {
      
        Serial.print("Free Heap: ");
        Serial.println(ESP.getFreeHeap());

        Serial.println("getting currently playing song:");
        
        spotify.getPlayerDetails(printPlayerDetailsToSerial, SPOTIFY_MARKET);
        spotify.getCurrentlyPlaying(printCurrentlyPlayingToSerial, SPOTIFY_MARKET);

        //int player_status = spotify.getPlayerDetails(printPlayerDetailsToSerial, SPOTIFY_MARKET);
        // if (status == 200)
        // {
        //     Serial.println("Successfully got currently playing");
        // }
        // else if (status == 204)
        // {
        //     Serial.println("Doesn't seem to be anything playing");
        // }
        // else
        // {
        //     Serial.print("Error: ");
        //     Serial.println(status);
        // }
        requestDueTime = millis() + delayBetweenRequests;

        //LCD CONTOL=====================================================================================================
          tft.fillRectVGradient(0, 0, 320, 240, tft.color565(10, 10, 7), tft.color565(26, 65, 1) );


          //Song name
          tft.setTextFont(4);
          tft.setTextColor(TFT_WHITE);  
          tft.setTextSize(2);
          tft.drawString(songName, 20, 20);

          //Artist Name
          tft.setTextFont(4);
          tft.setTextColor(TFT_LIGHTGREY);  
          tft.setTextSize(1);
          tft.drawString(artistName, 20, 80);

          //Play button
          tft.fillSmoothCircle(H_WIDTH, buttonsYOffset, 27, TFT_WHITE);
          if (isPlaying){
            tft.fillRect(H_WIDTH - 11, buttonsYOffset - 12, 22, 24, tft.color565(0, 51, 0));    // Pause bars
            tft.fillRect(H_WIDTH - 3, buttonsYOffset - 12, 6, 24, TFT_WHITE);
          } else {
            tft.fillTriangle(H_WIDTH - 8, buttonsYOffset-17, H_WIDTH - 8, buttonsYOffset+17, H_WIDTH + 15, buttonsYOffset, tft.color565(0, 51, 0));    // Play arrow
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

            tft.fillCircle(40, buttonsYOffset + 30, 3, TFT_GREEN);
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
            tft.fillSmoothRoundRect(WIDTH - 55, buttonsYOffset - 13, 35, 25, 5, tft.color565(0, 51, 0));
            tft.fillRect(WIDTH - 45, buttonsYOffset - 8, 10, 25, tft.color565(0, 51, 0));
            tft.fillTriangle(WIDTH - 40, buttonsYOffset +14, WIDTH - 33, buttonsYOffset +21, WIDTH - 33, buttonsYOffset +7, TFT_GREEN);
            tft.fillCircle(WIDTH - 40, buttonsYOffset + 33, 3, TFT_GREEN);
          } else {
          tft.fillSmoothRoundRect(WIDTH - 60, buttonsYOffset - 18, 45, 35, 5, TFT_WHITE);
            tft.fillSmoothRoundRect(WIDTH - 55, buttonsYOffset - 13, 35, 25, 5, tft.color565(0, 51, 0));
            tft.fillRect(WIDTH - 45, buttonsYOffset - 20 + 10 + 2, 15 - 5, 30 - 5, tft.color565(0, 51, 0));
            tft.fillTriangle(WIDTH - 40, buttonsYOffset +14, WIDTH -33, buttonsYOffset +21, WIDTH - 33, buttonsYOffset +7, TFT_WHITE);
          }



          // Song Progress
          tft.setTextFont(2);
          tft.setTextSize(1);
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
          tft.drawFastHLine(60, progressBarYOffset, barWidth, TFT_DARKGREY);
          tft.drawFastHLine(60, progressBarYOffset, xPos, TFT_WHITE);
          tft.fillCircle(xPos + 60,progressBarYOffset,5, TFT_WHITE);
      //TOUCH CONTROL------------------------------------------------------------------------
        uint16_t x, y;

        // See if there's any touch data for us
        // if (tft.getTouch(&x, &y))
        // {
        //   if ((x > 200) && (x < 280) && (y>160) && (y<240)) {
        //     if (isPlaying){
        //       spotify.pause();
        //     } else {
        //       spotify.play();
        //     }
        //   }

        // }
      
    }
}
