/*
 * Setup for the pwm function for output to the electrochromic film
 */

int PWM_PIN = 38;

void setup() {
  pinMode(PWM_PIN, OUTPUT);
}

/*
 * use PWM to control the MOSfet transistor for the electrochromic film
 * analogWrite generates PWM with dutycyle = outputVal and f ~ 500Hz
 */

void loop() {
  analogWrite(PWM_PIN, outputVal);
  delay(100);
}
