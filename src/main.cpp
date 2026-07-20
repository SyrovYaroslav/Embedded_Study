#include <Arduino.h>

const int analog_pin = 4;
int count = 0;
uint8_t resolution = 12;

void setup()
{
    Serial.begin(115200);
    delay(100);

    analogReadResolution(resolution);
    analogSetPinAttenuation(analog_pin, ADC_11db);
}

void loop()
{
    count++;
    int raw = analogRead(analog_pin);
    int raw_voltage = analogReadMilliVolts(analog_pin);

    float voltage = raw * 3.1 / 4095.0;
    float diff = ((voltage * 1000.0f) - raw_voltage) / raw_voltage * 100.0f;
    Serial.printf("%d)raw=%d U=%.2f mU=%d diff=%.4f%% resolution=%d attenuation=ADC_11db\n", count, raw, voltage, raw_voltage, diff, resolution);
    delay(100);
}