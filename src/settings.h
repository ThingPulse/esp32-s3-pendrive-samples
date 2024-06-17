#pragma once

#include <Wire.h>

#define MATH_MIN(a,b) ((a)<(b)?(a):(b))
#define MATH_MAX(a,b) ((a)>(b)?(a):(b))

#define PIN_I2C_SDA 4
#define PIN_I2C_SCL 5

#define PIN_TOUCH_1 6
#define PIN_TOUCH_2 7

#define PIN_BUZZZER  15
#define BUZZER_CHANNEL 0

#define PIN_LED_EN 16

#define PIN_TACTILE_BUTTON GPIO_NUM_17

#define PIN_BAT_MON 9
#define PIN_BAT_CHARGE 18
#define CHARGE_PIN_ACTIVITY_TIMEOUT_MS 3 * 1000
// voltage divider
#define BAT_R1 2.2f
#define BAT_R2 2.2f

#define DEEP_SLEEP_MICROS 5 * 1000 * 1000

void scanI2cBus()
{
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

}
