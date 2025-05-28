#ifndef LEDgame
#define LEDgame

#include <nrf_gpio.h>
#include <nrfx_uarte.h>
#include <nrfx_systick.h>
#include <nrfx_rtc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>


// UART TX and RX pins for NRF5340DK
#define PIN_TX (20)
#define PIN_RX (22)

// Define Button pin for button press
#define BUTTON_1 23  
#define BUTTON_2 24  
#define BUTTON_3 8
#define BUTTON_4 9  

// LED pin mappings
#define LED1 (28)
#define LED2 (29)
#define LED3 (30)
#define LED4 (31)

#define MAX_SEQUENCE_LENGTH 12
#define NUM_LEDS 4
#define BUTTON_DEBOUNCE_DELAY 200 // ms
#define LED_ON_TIME 500           // ms (easy), reduced in higher levels
#define INTERVAL_TIME 250         // ms between LEDs



/*************** LED GAME ****************/
char get_last_key(void);
void clear_last_key(void);
void memory_game();
void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context);


/*************** UARTE ****************/
void init_uart(void);
void uarte_write(char* data, int length);


/*************** Other Helper functions ****************/
int getRand();
void initialize();
void initializeRTC();

void start_game_grupp14(void);

#endif 