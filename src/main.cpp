#include <Arduino.h>

enum class LedState
{
    Off,
    On
};

class Config
{
public:
    static const uint32_t SerialBaud = 115200;
    static const uint16_t PrintPeriod = 1000;
};

class Led
{
public:
    static constexpr uint8_t Pin = 4;
    static constexpr uint32_t BlinkPeriod = 200;

    void init()
    {
        pinMode(Pin, OUTPUT);
        set(LedState::Off);
    }

    void update()
    {
        const unsigned long currentTime = millis();

        if (currentTime - lastBlinkTime >= BlinkPeriod)
        {
            lastBlinkTime = currentTime;
            toggle();
        }
    }

    void set(LedState newState)
    {
        state = newState;
        digitalWrite(Pin, state == LedState::On ? HIGH : LOW);
    }

private:
    void toggle()
    {
        set(state == LedState::On ? LedState::Off : LedState::On);
    }

    LedState state = LedState::Off;
    unsigned long lastBlinkTime = 0;
};

Led led;

void setup()
{
    Serial.begin(Config::SerialBaud);
    led.init();
}

void loop()
{
    static uint32_t loopCounter = 0;

    const unsigned long startTime = micros();

    led.update();

    const unsigned long loopTime = micros() - startTime;

    if (++loopCounter >= Config::PrintPeriod)
    {
        loopCounter = 0;

        Serial.print("Loop time: ");
        Serial.print(loopTime);
        Serial.println(" us");
    }
}