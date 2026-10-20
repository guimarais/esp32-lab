#include <Arduino.h>

int relayPin = 23;

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
}

void loop() {
  digitalWrite(relayPin, HIGH);
  Serial.println("Relay ON");
  delay(1000);
  digitalWrite(relayPin, LOW);
  Serial.println("Relay OFF");
  delay(1000);
}
