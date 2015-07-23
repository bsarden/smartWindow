/*
 * Setup for the pwm function for output to the electrochromic film
 */


int PWM_PIN = 38;

void setup() {
  pinMode(PWM_PIN, OUTPUT);
//  Serial.begin(115200);
}

void loop() {
  analogWrite(PWM_PIN, outputVal);
  //Serial.println("wrote to smart window!");
  delay(100);
}
