/* 
 * Lights up the RED_LED if the light value exceeds sunny threshold
 * Lights up the BLUE_LED if the light value exceeds dark threshold
 */

#define darkThreshold 300
#define lightThreshold 500

void setupLED() {
    pinMode(RED_LED, OUTPUT); // Set LED pins as output
    pinMode(BLUE_LED, OUTPUT);
    digitalWrite(BLUE_LED, LOW); // init LEDs to off
    digitalWrite(RED_LED, LOW);
}

void loopLED() {
    if(light > lightThreshold) {
        digitalWrite(RED_LED, HIGH);
        digitalWrite(BLUE_LED, LOW);
    }
    else if (light < darkThreshold) {
        digitalWrite(BLUE_LED, HIGH);
        digitalWrite(RED_LED, LOW);
    }
    else {
        digitalWrite(RED_LED, LOW);
        digitalWrite(BLUE_LED, LOW);
    }
    delay(200);
}
