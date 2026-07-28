#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

const int button_pin = 5;
const int adc_pin = 4;
const int led_pin = 48;
const int num = 1;

uint8_t resolution = 12;

Adafruit_NeoPixel pixels(num, led_pin, NEO_GRB + NEO_KHZ800);

struct RGB
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

RGB colors[] = {
    {255, 0, 0},    // Красный
    {0, 255, 0},    // Зеленый
    {0, 0, 255},    // Синий
    {255, 255, 0},  // Желтый
    {255, 0, 255},  // Фиолетовый
    {0, 255, 255},  // Голубой
    {255, 255, 255} // Белый
};

int currentColor = 0;
const int numColors = sizeof(colors) / sizeof(colors[0]);
bool buttonPressed = HIGH;
bool buttonStable = HIGH;
unsigned long lastChanges = 0;
const int debounceMs = 25;
const int ledOnThreshold = 2500;
const int ledMaxBrightThreshold = 1000;
const int maxBright = 20;
const int minBright = 0;

bool ledEnabled = false;

void setup()
{
    Serial.begin(115200);
    delay(100);
    pinMode(button_pin, INPUT_PULLUP);
    analogReadResolution(resolution);
    analogSetPinAttenuation(adc_pin, ADC_11db);
    pixels.begin();
    pixels.setBrightness(10);
}

void loop()
{
    bool read_button = digitalRead(button_pin);

    if (read_button != buttonPressed)
    {
        lastChanges = millis();
        buttonPressed = read_button;
    }
    if ((millis() - lastChanges) > debounceMs && buttonPressed != buttonStable)
    {
        buttonStable = buttonPressed;
        if (buttonStable == LOW)
        {
            currentColor++;
            if (currentColor >= numColors)
            {
                currentColor = 0;
            }
        }
    }
    int raw = analogRead(adc_pin);
    if (raw < ledOnThreshold)
    {
        int brightness_led = map(raw, ledMaxBrightThreshold, ledOnThreshold, maxBright, minBright);
        brightness_led = constrain(brightness_led, minBright, maxBright);
        pixels.setBrightness(brightness_led);
        pixels.setPixelColor(0, pixels.Color(colors[currentColor].r, colors[currentColor].g, colors[currentColor].b));
    }
    else
    {
        pixels.setPixelColor(0, 0);
    }

    pixels.show();
}