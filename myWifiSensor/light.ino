/* 
 * Samples the light sensor values 5 times / sec
 * value for the sensor is stored in the light global var
 */
 
void setupLight() {
    // no setup code necessary, just doing an analogRead
}

void loopLight() {
    light = analogRead(24);
    delay(200);         // sampling frequency
}
