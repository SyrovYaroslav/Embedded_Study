#include "driver/ledc.h"
#include "esp_timer.h"

#define BUZZER_GPIO 18
#define TICK_MS 50

#define C5 523
#define D5 587
#define E5 659
#define F5 698
#define G5 784

static const int freqs[] = {E5, E5, F5, G5, G5, F5, E5, D5, C5, C5, D5, E5, E5, D5, D5};
static const int ticks[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 2, 8};
#define NOTES (sizeof(freqs) / sizeof(freqs[0]))

static int idx = 0;
static int left = 0;
static esp_timer_handle_t timer;

static void tick(void *arg)
{
    if (left == 0)
    {
        if (idx >= NOTES)
        {
            idx = 0;
        }
        ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, freqs[idx]);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 2048);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        left = ticks[idx];
        idx++;
    }

    left--;
    if (left == 0)
    {
        ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    }
}

void app_main(void)
{
    ledc_timer_config_t t = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&t);

    ledc_channel_config_t c = {
        .gpio_num = BUZZER_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };
    ledc_channel_config(&c);

    esp_timer_create_args_t args = {.callback = tick, .name = "tick"};
    esp_timer_create(&args, &timer);
    esp_timer_start_periodic(timer, TICK_MS * 1000);
}