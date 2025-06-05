#include "grupp28.h"
#include <nrfx_uarte.h>
#include <stdlib.h>
#include "nrf_gpio.h"
#include "stdio.h"

#define NUM_LEDS 4
#define MAX_SEQUENCE_LENGTH 100

nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
volatile uint8_t score = 0;  // Global poängvariabel

static const uint8_t led_pins[4] = {LED_PIN_1, LED_PIN_2, LED_PIN_3, LED_PIN_4};
static const uint8_t button_pins[4] = {BUTTON_PIN_1, BUTTON_PIN_2, BUTTON_PIN_3, BUTTON_PIN_4};
static uint8_t sequence[100];
static uint8_t sequence_length = 0;

char uarte_buffer[1];  // För att ta emot en byte via UARTE

void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)
{
    if (p_event->type == NRFX_UARTE_EVT_RX_DONE) {
        // Läs det mottagna tecknet
        char received_char = p_event->data.rx.p_data[0];
        
        if (received_char == 'q') {
            // Lägg till 10 poäng om 'q' tas emot
            score += 10;
            
            // Skicka feedback till användaren
            char score_msg[100];
            snprintf(score_msg, sizeof(score_msg), "Score increased by 10! New score: %d\n\r", score);
            uarte_write(score_msg, strlen(score_msg));
            
        }
        nrfx_uarte_rx(&instance, uarte_buffer, sizeof(uarte_buffer));
    }
}

void uarte_init()
{
    // Initiera UARTE
    nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(20, 22);
    nrfx_err_t err = nrfx_uarte_init(&instance, &config, uarte_handler);  // Registrera callback
    if (err != NRFX_SUCCESS) {
        // Hantera fel vid UARTE-initiering
        while (1);
    }

    // Rensa eventuella väntande interrupt
    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    
    // Aktivera UARTE-interruptet
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    
    // Starta mottagning
    nrfx_uarte_rx(&instance, uarte_buffer, sizeof(uarte_buffer));
}

void uarte_write(char* data, int length) {
    nrfx_uarte_tx(&instance, (const uint8_t*)data, length, 0);
}

void simple_delay_ms(uint32_t ms) {
    volatile uint32_t count;
    while (ms--) {
        count = 3000;  // Justera för din CPU-frekvens
        while (count--) {
            __NOP();
        }
    }
}

void first_use_led() {
    for (int i = 0; i < 4; i++) {
        nrf_gpio_cfg_output(led_pins[i]);
        nrf_gpio_pin_write(led_pins[i], 1); // turn off (active low)
    }
}

void button_init(void) {
    nrf_gpio_cfg_input(BUTTON_PIN_1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON_PIN_2, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON_PIN_3, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON_PIN_4, NRF_GPIO_PIN_PULLUP);
}

void play_sequence(uint8_t length) {
    for (int i = 0; i < length; i++) {
        flash_led(sequence[i], 200);
        simple_delay_ms(200);
    }
}

int wait_for_button_press(void) {
    while (1) {
        for (int i = 0; i < NUM_LEDS; i++) {
            if (!nrf_gpio_pin_read(button_pins[i])) {
                while (!nrf_gpio_pin_read(button_pins[i])); // wait for release
                return i;
            }
        }
    }
}

bool check_user_input(uint8_t length) {
    for (int i = 0; i < length; i++) {
        int pressed = wait_for_button_press();
        flash_led(pressed, 100);
        if (pressed != sequence[i]) {
            return false;
        }
    }
    return true;
}

void add_random_to_sequence(void) {
    if (sequence_length < MAX_SEQUENCE_LENGTH) {
        sequence[sequence_length++] = rand() % NUM_LEDS;
    }
}

uint8_t get_score(void) {
    score += 1;
    return score;
}

void flash_led(uint8_t index, uint16_t delay_ms) {
    uint8_t pin = led_pins[index];
    nrf_gpio_pin_write(pin, 0);  // ON (active low)
    simple_delay_ms(delay_ms);
    nrf_gpio_pin_write(pin, 1);  // OFF
    simple_delay_ms(delay_ms);
}

uint8_t get_sequence_length(void) {
    return sequence_length;
}

void rtc_init(void) {
    NRF_RTC1->PRESCALER = 0; // 32768 Hz / (0 + 1) = 32768 Hz
    NRF_RTC1->TASKS_START = 1;
}

void start_game_gruppX(void);
