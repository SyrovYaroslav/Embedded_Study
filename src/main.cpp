#include <Arduino.h>

//gpio4
const int LED1_PIN = 4;
//gpio6
const int LED2_PIN = 6;
//gpio5
const int BUTTON1_PIN = 5;
//gpio0
const int BUTTON2_PIN = 0;
const int SWICH_DELAY = 2000;

bool button1WasPressed = false;
bool button2WasPressed = false;
bool longPressHandled = false;
unsigned long pressTime = 0;
unsigned long previousBlinkTime = 0;
int blinkState = 0;
int blinkSpeedIndex = 0;
const int blinkSpeeds[10] = {1000, 900, 800, 700, 600, 500, 400, 300, 200, 100};

void blinkFunction(int delayTime) {
  if (millis() - previousBlinkTime >= delayTime)
    {
        previousBlinkTime = millis();
        digitalWrite(LED1_PIN, !digitalRead(LED1_PIN));
        digitalWrite(LED2_PIN, !digitalRead(LED2_PIN));
    }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  Serial.println("Setup complete");
}

void loop() {
  int button1State = digitalRead(BUTTON1_PIN);
  int button2State = digitalRead(BUTTON2_PIN);

  if (button1State == LOW) {
    if (!button1WasPressed) {
      button1WasPressed = true;
      Serial.println("Button 1 short press");
      blinkState = 1;
    }
  } else {
    button1WasPressed = false;
  }
  

  if (button2State == LOW) {
    if (!button2WasPressed) {
      button2WasPressed = true;
      longPressHandled = false;
      pressTime = millis();
    }
    if (!longPressHandled &&
      millis() - pressTime >= SWICH_DELAY) {
      Serial.println("Switching to state 0");
      blinkState = 0;
      longPressHandled = true;
    }
  } else {
    if (button2WasPressed && !longPressHandled) {
      Serial.println("Button 2 short press");
      blinkState = 2;
    }
    button2WasPressed = false;
  }
  

  if (blinkState == 1) {
    blinkSpeedIndex--;
    if (blinkSpeedIndex < 0)
        blinkSpeedIndex = 0;

    Serial.println("Speed: " + String(blinkSpeeds[blinkSpeedIndex]));
    blinkState = 3;
  }

  if (blinkState == 2) {
      blinkSpeedIndex++;
      if (blinkSpeedIndex > 9)
          blinkSpeedIndex = 9;

      Serial.println("Speed: " + String(blinkSpeeds[blinkSpeedIndex]));
      blinkState = 3;
  }

  if (blinkState == 0) {
      blinkSpeedIndex = 5;   // вернуть скорость по умолчанию
      Serial.println("Reset");
      blinkState = 3;
  }

  blinkFunction(blinkSpeeds[blinkSpeedIndex]);
  delay(); // небольшая задержка для стабилизации работы
  
}