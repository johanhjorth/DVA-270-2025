#include "grupp24.h"

extern volatile int button_pressed; 
nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0); 
nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(0); 

void uarte_write(uint8_t *data, uint8_t length) {
    nrfx_uarte_tx(&instance, data, length, 0);
}


void uart_send_string(const char* str)
{
    char msg[400]; 
    sprintf(msg, "%s", str); 
    uarte_write(msg, strlen(msg)); 
}

bool read_string(char *str, size_t max_len) {
    nrfx_uarte_rx(&instance, (uint8_t*)str, max_len);
    return true;

    uint8_t ch;
    int i = 0;

    while (i < max_len - 1)
    {
        nrfx_uarte_rx(&instance, &ch, 1); 
        if (ch == '\r' || ch == 13) break; 
        str[i++] = (char)ch; 
    }

    str[i] = '\0'; 
}


int read_int()
{
    char buffer[50]; 
    read_string(buffer, sizeof(buffer)); 
    return atoi(buffer); 
}

void set_seed(void) {
    srand(nrf_rtc_counter_get(rtc_instance.p_reg));
}

int get_random_number(int upper, int lower) {
    return lower + rand() % (upper - lower + 1);
}

void play_game(void) {
    char msg[100]; 
    int8_t clearscreen[] = CLEAR_SCREEN; 

    strcpy(msg, "Tryck på knappen när LED lyser!\n\r");
    uarte_write((uint8_t*)msg, strlen(msg));

    set_seed();
    int wait_time_ms = get_random_number(5000, 2000);
    nrfx_systick_delay_ms(wait_time_ms); 

    nrf_gpio_pin_clear(LED1);

    button_pressed = 0;
    nrfx_systick_state_t timer_state;
    nrfx_systick_get(&timer_state); 

    while (1) {
        if (button_pressed) { 
            break;
        }

        if (nrfx_systick_test(&timer_state, 200000)) { 
            break;
        }
    }

    nrf_gpio_pin_set(LED1); 

    uarte_write(clearscreen, sizeof(clearscreen));

    if (button_pressed) {
        strcpy(msg, "Du VANN!\n\r\n\r");
    } else {
        strcpy(msg, "För långsam. Du FÖRLORADE.\n\r\n\r");
    }
    uarte_write((uint8_t*)msg, strlen(msg));

    strcpy(msg, "Spela igen? Tryck på knappen...\n\r");
    uarte_write((uint8_t*)msg, strlen(msg));

    button_pressed = 0; 
    while (!button_pressed) {
    }


}