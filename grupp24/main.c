#include "grupp24.h"
#include <nrfx_gpiote.h>
#include <nrf_gpio.h>
#include <soc/nrfx_irqs.h>
#include <nrf5340_application.h>
#include <nrf_gpiote.h>
#include <nrfx_systick.h>

#define BUTTON1_PIN 23 

volatile int button_pressed = 0; 


void GPIOTE0_IRQHandler(void) {
    if (NRF_GPIOTE0->EVENTS_IN[0]) { 
        NRF_GPIOTE0->EVENTS_IN[0] = 0;
        button_pressed = 1; 
    }
}

void init_button_interrupt(void) {
    NRF_P0->PIN_CNF[BUTTON1_PIN] = 
        (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
        (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
        (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos); 

    NRF_GPIOTE0->CONFIG[0] = 
        (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) |
        (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos) |
        (BUTTON1_PIN << GPIOTE_CONFIG_PSEL_Pos); 

    NRF_GPIOTE0->INTENSET = GPIOTE_INTENSET_IN0_Msk; 

    NVIC_SetPriority(GPIOTE0_IRQn, 1); 
    NVIC_EnableIRQ(GPIOTE0_IRQn); 
}

void init_stuff(void) {
    
    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(20, 22); 
    nrfx_uarte_init(&instance, &config, NULL); 

 
    nrfx_rtc_config_t rtc_config = NRFX_RTC_DEFAULT_CONFIG; 
    nrfx_err_t err = nrfx_rtc_init(&rtc_instance, &rtc_config, NULL);
    nrfx_rtc_init(&rtc_instance, &rtc_config, NULL); 
    nrfx_rtc_enable(&rtc_instance);

   
    nrfx_systick_init(); 
    while (nrf_gpio_pin_read(BUTTON1) == 1); 
    srand(nrf_systick_val_get()); 

    nrf_systick_val_clear(); 
    uint32_t pretime = nrf_systick_val_get(); 

    nrf_gpio_cfg_output(LED1); 
    nrf_gpio_pin_write(LED1, LED_OFF); 


    nrf_gpio_cfg_input(BUTTON1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON2, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON3, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON4, NRF_GPIO_PIN_PULLUP); 
}


int main() {
    init_stuff(); 
    init_button_interrupt(); 

    int8_t clearscreen[] = CLEAR_SCREEN; 
    uarte_write((uint8_t *)clearscreen, sizeof(clearscreen)); 

    while (1) {
        play_game(); 
        uarte_write(clearscreen, sizeof(clearscreen));
    }

    return 0; 
    }