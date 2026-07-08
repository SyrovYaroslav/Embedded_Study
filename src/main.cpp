#include <Arduino.h>

const int PIN4_LED = 4;
const int PIN5_LED = 5;

void ledState(int delayTime, byte state) {
  // put your main code here, to run repeatedly:
  digitalWrite(PIN4_LED, state);
  digitalWrite(PIN5_LED, state);
  delay(delayTime);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN4_LED, OUTPUT);
  pinMode(PIN5_LED, OUTPUT);
  digitalWrite(PIN4_LED, LOW);
  digitalWrite(PIN5_LED, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  ledState(1000, HIGH);
  ledState(2000, LOW);
}
