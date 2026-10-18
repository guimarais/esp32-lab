#include <Arduino.h>
#define LED 2   // onboard LED on most devkits

void setup() { pinMode(LED, OUTPUT); }
void loop() {
  digitalWrite(LED, !digitalRead(LED));
  delay(500);
}
