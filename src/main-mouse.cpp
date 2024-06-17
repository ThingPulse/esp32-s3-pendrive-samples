
#include <Arduino.h>
#include "USB.h"
#include "USBHIDMouse.h"
#include "USBHIDKeyboard.h"

USBHIDMouse Mouse;
USBHIDKeyboard Keyboard;

int range = 5;           // output range of X or Y movement; affects movement speed
int responseDelay = 10;  // response delay of the mouse, in ms

void setup() {

  // initialize mouse control:
  Mouse.begin();
  Keyboard.begin();
  USB.begin();

  Mouse.move(100, 0, 0);
  delay(1000);
  Mouse.move(0, 100, 0);
  delay(1000);
  Mouse.move(-100, 0, 0);
  Keyboard.press(0xef);
  delay(1000);
  Keyboard.release(0xef);
  Keyboard.pressRaw(HID_KEY_VOLUME_UP);
}

void loop() {

}