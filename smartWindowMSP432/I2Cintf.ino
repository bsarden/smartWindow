// Digital Potentiometer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This code was adopted from the dpot example from the wire library

#include <Wire.h>
//#include "~/Documents/Energia/smartWindowMSP432/globals.h"
//typedef enum modes{
//    AUTO_MODE,
//    MANUAL_MODE
//}modes;
// globals 
extern int autoOutputVal;    // read in val from light sensor
extern int manualOutputVal;
extern int mode;

void setupI2Cintf()
{
  Serial.begin(115200);
  Wire.begin(); // join i2c bus (address optional for master)
}

/*
 * currently this loop is only printing once
 * I think this is because it returns if it cannot connect to the device
 * this means that we need to find the device address from the dpot datasheet
 */
void loopI2Cintf()
{
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(byte(0x00));  // sends instruction byte
//  if(mode == AUTO_MODE) { Wire.write(autoOutputVal); }
//  else { Wire.write(manualOutputVal); }
  Wire.write(autoOutputVal);
  Serial.println("autoOutputVal:");
  Serial.println(autoOutputVal);
  Wire.endTransmission();    // stop transmitting
  Serial.println(autoOutputVal);
  delay(100);      // 500 ms delay in between writes to dpot
}
