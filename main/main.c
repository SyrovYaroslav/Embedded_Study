#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/ledc.h"

static const char *TAG = "servo";

#define ADC_CH_POT ADC_CHANNEL_3
#define SERVO_PIN 18
#define SERVO_RANGE_DEG 180.0f
#define MIN_PULSE_US 500
#define MAX_PULSE_US 2400
#define PERIOD_US 20000
#define SERVO_RESOLUTION LEDC_TIMER_13_BIT
#define LOOP_PERIOD_MS 20
#define ADC_FULL_SCALE 4095

static adc_oneshot_unit_handle_t adc;

static void adc_setup(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, ADC_CHANNEL_3, &config));
}

static void setup_servo_ledc(void)
{
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = SERVO_RESOLUTION,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));

    ledc_channel_config_t channel_conf = {
        .gpio_num = SERVO_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel_conf));
}

static void set_servo_angle(float angle)
{
    if (angle < 0)
        angle = 0;
    if (angle > SERVO_RANGE_DEG)
        angle = SERVO_RANGE_DEG;

    float pulse_us = MIN_PULSE_US + (angle / SERVO_RANGE_DEG) * (MAX_PULSE_US - MIN_PULSE_US);
    uint32_t duty = (uint32_t)((pulse_us / PERIOD_US) * (1 << SERVO_RESOLUTION));

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
}

static float raw_to_angle(int raw)
{
    return (float)raw * SERVO_RANGE_DEG / ADC_FULL_SCALE;
}

void app_main(void)
{
    adc_setup();
    setup_servo_ledc();
    while (1)
    {
        int raw = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc, ADC_CH_POT, &raw));
        float angle = raw_to_angle(raw);

        set_servo_angle(angle);

        ESP_LOGI(TAG, "ADC: %4d -> угол от левого края: %3f°", raw, angle);
        vTaskDelay(pdMS_TO_TICKS(LOOP_PERIOD_MS));
    }
}