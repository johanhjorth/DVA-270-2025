#include "grupp19.h"
#include <nrfx_uarte.h>
#include <nrf_gpio.h>
#include <string.h>
#include <stdlib.h>
#include <nrfx_rtc.h>
#include <nrfx_systick.h>

#define TX_PIN 20
#define RX_PIN 22

#define LED1 28
#define LED2 29
#define LED3 30
#define LED4 31

const nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(0);
static nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
static volatile uint8_t rx_buffer;
static int target_number;
static int lives = 4;
static uint32_t seed_value = 0;

// Flaggsystem
volatile int won_flag = 0;
volatile int lost_flag = 0;
volatile int invalid_flag = 0;
volatile int guess_wrong_flag = 0;
volatile int guess_valid = 0;

// Meddelanden
static char msg_intro[] = "Gissa ett tal mellan 1 och 9\r\n";
static char msg_wrong[] = "Fel gissning!\r\n";
static char msg_lost[]  = "Du förlorade! men... kolla på alla led lampor! \r\n";
static char msg_win[]   = "Du vann!\r\n";
static char msg_invalid[] = "Ogiltig gissning. Ange ett tal mellan 1 och 10.\r\n";

// UART interrupt handler
void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)
{
    if (p_event->type != NRFX_UARTE_EVT_RX_DONE)
        return;

    int guess = rx_buffer - '0';
    guess_valid = 0;

    if (guess < 1 || guess > 9)
    {
        invalid_flag = 1;
    }
    else if (guess == target_number)
    {
        won_flag = 1;
    }
    else
    {
        lives--;

        switch (lives)
        {
            case 3: nrf_gpio_pin_set(LED4); break;
            case 2: nrf_gpio_pin_set(LED3); break;
            case 1: nrf_gpio_pin_set(LED2); break;
            case 0:
                nrf_gpio_pin_set(LED1);
                lost_flag = 1;
                break;
        }

        if (!lost_flag) guess_wrong_flag = 1;
    }

    guess_valid = 1;
}

// Initiera LED-pinnar
static void init_leds(void)
{
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_cfg_output(LED2);
    nrf_gpio_cfg_output(LED3);
    nrf_gpio_cfg_output(LED4);

    nrf_gpio_pin_clear(LED1);
    nrf_gpio_pin_clear(LED2);
    nrf_gpio_pin_clear(LED3);
    nrf_gpio_pin_clear(LED4);
}

// Startfunktion
void start_game_grupp19(void)
{
    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(TX_PIN, RX_PIN);
    nrfx_uarte_init(&instance, &config, uarte_handler);

    init_leds();

    nrfx_rtc_enable(&rtc_instance);

    seed_value = 0;
    while (seed_value < 10)
    {
        seed_value = nrfx_rtc_counter_get(&rtc_instance);
    }

    srand(seed_value);
    target_number = 1 + (rand() % 9); // 1–9

    nrfx_uarte_tx(&instance, (uint8_t*)msg_intro, strlen(msg_intro), 0);
    nrfx_uarte_rx(&instance, &rx_buffer, 1);
}

// Huvudloop
void game_update(void)
{
    if (!guess_valid) return;

    guess_valid = 0;

    if (invalid_flag)
    {
        nrfx_uarte_tx(&instance, (uint8_t*)msg_invalid, strlen(msg_invalid), 0);
        invalid_flag = 0;
    }
    else if (won_flag)
    {
        nrfx_uarte_tx(&instance, (uint8_t*)msg_win, strlen(msg_win), 0);
        won_flag = 0;
        return;
    }
    else if (lost_flag)
    {
        nrfx_uarte_tx(&instance, (uint8_t*)msg_lost, strlen(msg_lost), 0);

        nrfx_systick_init();

        while (1)
        {
            nrf_gpio_pin_set(LED1);
            nrfx_systick_delay_ms(200);
            nrf_gpio_pin_toggle(LED1);
            nrf_gpio_pin_clear(LED1);

            nrf_gpio_pin_set(LED2);
            nrfx_systick_delay_ms(200);
            nrf_gpio_pin_toggle(LED2);
            nrf_gpio_pin_clear(LED2);

            nrf_gpio_pin_set(LED3);
            nrfx_systick_delay_ms(200);
            nrf_gpio_pin_toggle(LED3);
            nrf_gpio_pin_clear(LED3);

            nrf_gpio_pin_set(LED4);
            nrfx_systick_delay_ms(200);
            nrf_gpio_pin_toggle(LED4);
            nrf_gpio_pin_clear(LED4);

            nrfx_systick_delay_ms(200);
        }
    }
    else if (guess_wrong_flag)
    {
        nrfx_uarte_tx(&instance, (uint8_t*)msg_wrong, strlen(msg_wrong), 0);
        guess_wrong_flag = 0;
    }

    nrfx_uarte_rx(&instance, &rx_buffer, 1);
}
