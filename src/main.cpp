#include <Arduino.h>

#define BUTTON_LEFT 15
#define BUTTON_RIGHT 3

int16_t counter_left = 0;
int16_t counter_right = 0;

void IRAM_ATTR reaction_left()
{
  counter_left++;
  Serial.println("\nLEFT Button Pressed! Count: " + String(counter_left));
}

void IRAM_ATTR reaction_right()
{
  counter_right++;
  Serial.println("\nRIGHT Button Pressed! Count: " + String(counter_right));
}

void setup()
{
  pinMode(BUTTON_LEFT, INPUT);
  pinMode(BUTTON_RIGHT, INPUT);
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), reaction_left, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), reaction_right, RISING);
}

void loop()
{
  Serial.print("HELLO");
  delay(250);
}

// #define BUTTON 3
// #define DEBOUNCE_MS 15

// bool raw = LOW;
// bool stable = HIGH;
// bool prevStable = HIGH;
// unsigned long LastChanges = 0;
// int counter = 0;

// void setup()
// {
//   pinMode(BUTTON, INPUT_PULLUP);
//   Serial.begin(115200);
// }

// void loop()
// {
//   int reading = digitalRead(BUTTON);

//   if (reading != raw)
//   {
//     LastChanges = millis();
//     raw = reading;
//   }

//   if ((millis() - LastChanges) > DEBOUNCE_MS && raw != stable)
//   {
//     prevStable = stable;
//     stable = raw;

//     if (stable == LOW && prevStable == HIGH)
//     {
//       counter++;
//       Serial.println("Button Pressed! Count: " + String(counter));
//     }
//     else
//     {
//       Serial.println("Button Released");
//     }
//   }
// }
