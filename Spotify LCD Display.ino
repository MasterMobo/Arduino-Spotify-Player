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

unsigned long delayBetweenRequests = 1000; // Time between requests (miliseconds)
unsigned long requestDueTime;               //time when request due


//----------------------------------------------------------------------------------------------------------------
// SETUP
//----------------------------------------------------------------------------------------------------------------

const int WIDTH = 480;   // Screen width
const int HEIGHT = 320;   // Screen height

const int H_WIDTH = WIDTH/2;   // Half Screen width
const int H_HEIGHT = HEIGHT/2;  // Half Screen height

const int progressBarYOffset = 270;
const int buttonsYOffset = 200;

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
    tft.setRotation(1);

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

    // If you want to enable some extra debugging
    // uncomment the "#define SPOTIFY_DEBUG" in SpotifyArduino.h

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
        // Market can be excluded if you want e.g. spotify.getCurrentlyPlaying()
        
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
        tft.fillRectVGradient(0, 0, 480, 320, tft.color565(10, 10, 7), tft.color565(26, 65, 1) );


        //Song name
        tft.setTextFont(4);
        tft.setTextColor(TFT_WHITE);  
        tft.setTextSize(2);
        tft.drawString(songName, 30, 50);

        //Artist Name
        tft.setTextFont(4);
        tft.setTextColor(TFT_LIGHTGREY);  
        tft.setTextSize(1.5);
        tft.drawString(artistName, 30, 110);

        //Play button
        tft.fillSmoothCircle(240, buttonsYOffset, 40, TFT_WHITE);
        if (isPlaying){
          tft.fillRect(H_WIDTH - 15, H_HEIGHT + 22, 30, 36, tft.color565(0, 51, 0));    // Pause bar
          tft.fillRect(H_WIDTH - 5, H_HEIGHT + 20, 10, 40, TFT_WHITE);
        } else {
          tft.fillTriangle(230, buttonsYOffset-20, 230, buttonsYOffset+20, 260, buttonsYOffset, tft.color565(0, 51, 0));    // Play arrow
        }


        //Next Button
        tft.fillTriangle(H_WIDTH + 90, buttonsYOffset-20, H_WIDTH + 90, buttonsYOffset+20, H_WIDTH + 120, buttonsYOffset, TFT_WHITE);
        tft.fillRect(260 + 97, buttonsYOffset - 20, 5, 40, TFT_WHITE);

        //Previous Button
        tft.fillTriangle(H_WIDTH - 10 - 80, buttonsYOffset-20, H_WIDTH - 10 - 80, buttonsYOffset+20, H_WIDTH + 20 - 140, 200, TFT_WHITE);
        tft.fillRect(260 - 140, buttonsYOffset - 20, 5, 40, TFT_WHITE);

        //Shuffle button
        if (shuffleOn){
          tft.fillRect(30, buttonsYOffset-15, 50, 30, TFT_GREEN);
          tft.fillRect(30, buttonsYOffset-10, 50, 20, tft.color565(0, 51, 0));
          tft.fillTriangle(25, buttonsYOffset-17 + 34 - 4, 32, buttonsYOffset-17 + 34 - 4 - 7, 32, buttonsYOffset-17 + 34 - 4 + 7, TFT_GREEN); // Bottom left arrow
          tft.fillTriangle(30 + 50 + 5, buttonsYOffset-17 + 3, 30 + 47, buttonsYOffset-17 + 3 - 7, 30 + 47, buttonsYOffset-17 + 3 + 8, TFT_GREEN); // Top right arrow
          tft.fillCircle(30 + 25, buttonsYOffset + 30 + 5, 4, TFT_GREEN);
        } else {
          tft.fillRect(30, buttonsYOffset-15, 50, 30, TFT_WHITE);
          tft.fillRect(30, buttonsYOffset-10, 50, 20, tft.color565(0, 51, 0));
          tft.fillTriangle(25, buttonsYOffset-17 + 34 - 4, 32, buttonsYOffset-17 + 34 - 4 - 7, 32, buttonsYOffset-17 + 34 - 4 + 7, TFT_WHITE); // Bottom left arrow
          tft.fillTriangle(30 + 50 + 5, buttonsYOffset-17 + 3, 30 + 47, buttonsYOffset-17 + 3 - 7, 30 + 47, buttonsYOffset-17 + 3 + 8, TFT_WHITE); // Top right arrow
        }

        // Repeat button
        // tft.fillRect(WIDTH - 80, buttonsYOffset - 20, 50, 40, TFT_WHITE);
        if (repeatOn){
          tft.fillRoundRect(WIDTH - 80, buttonsYOffset - 20, 50, 40, 5, TFT_GREEN);
          tft.fillRoundRect(WIDTH - 75, buttonsYOffset - 15, 40, 30, 5, tft.color565(0, 51, 0));
          tft.fillRect(WIDTH - 80 + 15, buttonsYOffset - 20 + 10, 10, 30, tft.color565(0, 51, 0));
          tft.fillTriangle(WIDTH - 80 + 20, buttonsYOffset - 20 + 37, WIDTH - 80 + 20 + 7, buttonsYOffset - 20 + 37 + 7, WIDTH - 80 + 20 + 7, buttonsYOffset - 20 + 37 - 7, TFT_GREEN);
          tft.fillCircle(WIDTH - 80 + 25, buttonsYOffset + 30 + 5, 4, TFT_GREEN);
        } else {
          tft.fillRoundRect(WIDTH - 80, buttonsYOffset - 20, 50, 40, 5, TFT_WHITE);
          tft.fillRoundRect(WIDTH - 75, buttonsYOffset - 15, 40, 30, 5, tft.color565(0, 51, 0));
          tft.fillRect(WIDTH - 80 + 15, buttonsYOffset - 20 + 10, 10, 30, tft.color565(0, 51, 0));
          tft.fillTriangle(WIDTH - 80 + 20, buttonsYOffset - 20 + 37, WIDTH - 80 + 20 + 7, buttonsYOffset - 20 + 37 + 7, WIDTH - 80 + 20 + 7, buttonsYOffset - 20 + 37 - 7, TFT_WHITE);
        }


        // Song Progress
        tft.setTextSize(1);
        tft.setCursor(40, progressBarYOffset - 10);
        tft.print((progress / 1000) / 60);
        tft.print(":");
        tft.print((progress / 1000) % 60);

        // Song Duration
        tft.setCursor(390, progressBarYOffset - 10);
        tft.print((duration / 1000) / 60);
        tft.print(":");
        tft.print((duration / 1000) % 60);

        //Progress bar
        float barWidth = 380 - 100;
        float xPos = barWidth*progress/duration;
        tft.drawFastHLine(100, progressBarYOffset, barWidth, TFT_DARKGREY);
        tft.drawFastHLine(100, progressBarYOffset, xPos, TFT_WHITE);
        tft.fillCircle(xPos + 100,progressBarYOffset,10, TFT_WHITE);
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
