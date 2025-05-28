#ifndef GRUPP15_H
#define GRUPP15_H
#define PIN_TXD 20
#define PIN_RXD 22
#define LED1 28
#define LED2 29
#define LED3 30
#define LED4 31
#define BUTTON1 23
#define MAX 100
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrfx_uarte.h>
#include <stdlib.h>
#include <string.h>
#include <nrf_gpio.h>
#include <nrfx_rtc.h>




extern nrfx_uarte_t uarte_instance;
extern int player_array[MAX];
extern int input_index;

void start_game_grupp15(void);

void uarte_write(char* data, int length);

void send_int(int input_value);

void start_counter();

void init_stuff();

int pattern_handler(int* game_array, int round_counter);

void feedback_led(int led_number);

void blink_led(int led_number);

void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context);

void blinking_phase(int* game_array, int round_counter);

void new_line();

#endif




