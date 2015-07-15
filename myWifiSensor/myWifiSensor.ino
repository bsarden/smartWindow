

/* 
 * myWifiSensor.ino
 * serves as a main for this project
 * all of the global variables that will be used must be created here
 */

// Libraries
#include <SPI.h>                // necessary for the CC3100 Wireless interface
#include <WiFi.h>               // handles all of the wireless protocols
#include <PubSubClient.h>       // adds MQTT broker support
#include <aJSON.h>              // JSON library, adds support for freeboard.io
#include <Event.h>              // custom library to handle Event processing


// Events
Event myEvent;
Event wifiReady;

//JSON Globals
aJsonStream serial_stream(&Serial);
char* jsonPayload;


// sensor globals
int light = 0;
int autoOutputVal = 1000;           // starting values for the I2C output, updated by MQTT incoming message
int manualOutputVal = 1000;         // starting value for manual mode, changed by Buttons

// blank setup, not necessary for main file
void setup() {
  // put your setup code here, to run once:

}

// blank loop, not necessary for main file
void loop() {
  // put your main code here, to run repeatedly: 
  
}
