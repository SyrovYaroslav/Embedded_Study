#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"

static const char *TAG = "PWM";

#define ADC_CH_LED ADC_CHANNEL_3
#define ADC_CH_MOTOR ADC_CHANNEL_4

#define GPIO_LED_PWM 6
#define GPIO_MOTOR_PWM 7

#define PWM_MODE LEDC_LOW_SPEED_MODE
#define PWM_RES LEDC_TIMER_12_BIT

#define LED_TIMER LEDC_TIMER_0
#define LED_CHANNEL LEDC_CHANNEL_0
#define LED_FREQ_HZ 5000

#define MOTOR_TIMER LEDC_TIMER_1
#define MOTOR_CHANNEL LEDC_CHANNEL_1
#define MOTOR_FREQ_HZ 15000

static adc_oneshot_unit_handle_t adc1_handle;

static void adc_setup(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CH_LED, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CH_MOTOR, &config));
}

static void pwm_setup(void)
{
    ledc_timer_config_t led_timer = {
        .speed_mode = PWM_MODE,
        .timer_num = LED_TIMER,
        .duty_resolution = PWM_RES,
        .freq_hz = LED_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&led_timer));

    ledc_timer_config_t motor_timer = {
        .speed_mode = PWM_MODE,
        .timer_num = MOTOR_TIMER,
        .duty_resolution = PWM_RES,
        .freq_hz = MOTOR_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&motor_timer));

    ledc_channel_config_t led_ch = {
        .gpio_num = GPIO_LED_PWM,
        .speed_mode = PWM_MODE,
        .channel = LED_CHANNEL,
        .timer_sel = LED_TIMER,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&led_ch));

    ledc_channel_config_t motor_ch = {
        .gpio_num = GPIO_MOTOR_PWM,
        .speed_mode = PWM_MODE,
        .channel = MOTOR_CHANNEL,
        .timer_sel = MOTOR_TIMER,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&motor_ch));
}

static void pwm_set(ledc_channel_t ch, uint32_t duty)
{
    ESP_ERROR_CHECK(ledc_set_duty(PWM_MODE, ch, duty));
    ESP_ERROR_CHECK(ledc_update_duty(PWM_MODE, ch));
}

void app_main(void)
{
    adc_setup();
    pwm_setup();

    while (1)
    {
        int raw_led = 0;
        int raw_motor = 0;

        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CH_LED, &raw_led));
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CH_MOTOR, &raw_motor));

        pwm_set(LED_CHANNEL, raw_led);
        pwm_set(MOTOR_CHANNEL, raw_motor);

        ESP_LOGI(TAG, "LED = %4d | MOTOR = %4d", raw_led, raw_motor);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}