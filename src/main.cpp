#include <Arduino.h>

const int releIn = 4;
const int releOut = 5;
const int buttonPin = 6;
const int debounceMs = 50;

unsigned long lastChangeTime = 0;
bool lastButtonState = HIGH;

volatile bool trigger = false;
volatile bool releRose = false;
volatile unsigned long releRiseTime = 0;

unsigned long gpioUpTime = 0;
bool waitingForRele = false;

void IRAM_ATTR releISR()
{
    releRiseTime = millis();
    releRose = true;
}

void IRAM_ATTR buttonISR()
{
    trigger = true;
}

void setup()
{
    Serial.begin(115200);
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(releIn, OUTPUT);
    digitalWrite(releIn, LOW);
    pinMode(releOut, INPUT);
    attachInterrupt(digitalPinToInterrupt(releOut), releISR, RISING);
    attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, CHANGE);
}

void loop()
{
    if (trigger)
    {
        trigger = false;
        if (millis() - lastChangeTime > debounceMs)
        {
            bool currentState = digitalRead(buttonPin);
            if (currentState != lastButtonState)
            {
                lastButtonState = currentState;
                lastChangeTime = millis();

                if (currentState == LOW)
                {
                    gpioUpTime = millis();
                    digitalWrite(releIn, HIGH);
                    waitingForRele = true;
                }
                else
                {
                    digitalWrite(releIn, LOW);
                    waitingForRele = false;
                }
            }
        }
    }

    if (releRose)
    {
        releRose = false;
        if (waitingForRele)
        {
            waitingForRele = false;
            unsigned long delayMs = releRiseTime - gpioUpTime;
            Serial.print("Rele start delay: ");
            Serial.print(delayMs);
            Serial.println(" ms");
        }
    }
}