/**
 * This example demonstrates how to use the touch sensor API to detect touch events.
 * 
 */

#include <Arduino.h>
#include <driver/touch_sensor.h>

int threshold = 180;
bool touch1detected = false;

void gotTouch1(){
 touch1detected = true;
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  analogRead(T1);
  touchAttachInterrupt(T1, gotTouch1, threshold);
}

void loop()
{
    if (touch1detected) {
      touch1detected = false;
      if (touchInterruptGetLastStatus(T1)) {
          Serial.println(" --- T1 Touched");
      } else {
          Serial.println(" --- T1 Released");
      }
    }
    delay(50);
}