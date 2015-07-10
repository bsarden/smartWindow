/* 
 This code was adapted from the debounce example in order to fit our the smart window project
 uses two buttons on the bottom of the board to change the mode that our project is in.
 There are two modes, MANUAL_MODE and AUTO_MODE. Manual mode implies that the user will press the buttons to 
 increase/decrease the tint. Auto mode implies that the light sensor data will be used in order to calculate
 the tint on the windows. The user can press both buttons at once to toggle in between the modes.
 */
//#include "./globals.h"
typedef enum modes{
    AUTO_MODE,
    MANUAL_MODE
}modes;

#define MAX_DPOT_VAL    1000   // guess for the max dpot val, will need to change later
// constants won't change. They're used here to 
// set pin numbers:
const int autoPin = PUSH1;
const int manualPin = PUSH2;     // the number of the pushbutton pin
const int ledAuto =  GREEN_LED;      // the number of the LED pin
const int ledManual = RED_LED;
const int ledMode = BLUE_LED;

// globals
int manualOutputVal = MAX_DPOT_VAL;
// Variables will change:
int autoLedState = HIGH;         // the current state of the autoOutput pin
int manualLedState = HIGH;
int autoButtonState;             // the current reading from the input pin
int manualButtonState;
int lastAutoButtonState = LOW;   // the previous reading from the input pin
int lastManualButtonState = LOW; 

int mode = AUTO_MODE;

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastAutoDebounceTime = 0;  // the last time the output pin was toggled
long lastManualDebounceTime = 0;
long debounceDelay = 50;    // the debounce time; increase if the output flickers

void Userintfsetup() {
  pinMode(autoPin, INPUT_PULLUP);
  pinMode(ledAuto, OUTPUT);
  pinMode(manualPin, INPUT_PULLUP);
  pinMode(ledManual, OUTPUT);
  digitalWrite(autoPin, HIGH);   // start in AUTO_MODE
  Serial.begin(115200);
}

void Userintfloop() {
  // read the state of the switch into a local variable:
  int autoReading = digitalRead(autoPin);
  int manualReading = digitalRead(manualPin);

  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (autoReading != lastAutoButtonState) {
    // reset the debouncing timer
    lastAutoDebounceTime = millis();
  }
  if (manualReading != lastManualButtonState) {
    lastManualDebounceTime = millis();
  } 
  
  if ((millis() - lastAutoDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    autoButtonState = autoReading;
  }
  
  if ((millis() - lastManualDebounceTime) > debounceDelay) {
    manualButtonState = manualReading;
  }
  
  switch(mode) {
    case AUTO_MODE:
      if(autoButtonState == LOW && manualButtonState == LOW) { 
        mode = MANUAL_MODE;
        Serial.println("MODE_CHANGE!!!");
        digitalWrite(manualPin, HIGH);    // indicate mode with LED
        digitalWrite(autoPin, LOW);
        delay(500);
      }
      break;
    case MANUAL_MODE:
      if(autoButtonState == LOW && manualButtonState == LOW) {
        mode = AUTO_MODE;
        Serial.println("MODE_CHANGE!!!");
        digitalWrite(autoPin, HIGH);
        digitalWrite(manualPin, LOW);
        delay(500);
      }
      else if (autoButtonState == LOW) { 
        manualOutputVal -= 50;
        if(manualOutputVal < 0) { manualOutputVal = 0; }
        }
      else if (manualButtonState == LOW) { 
        manualOutputVal += 50;
        if(manualOutputVal > MAX_DPOT_VAL) { manualOutputVal = MAX_DPOT_VAL; }
        }
      break;
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastAutoButtonState = autoReading;
  lastManualButtonState = manualReading;
}

