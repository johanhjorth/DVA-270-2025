#ifndef GRUPP6
#define GRUPP6

#include <nrfx_uarte.h>
#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrf.h>
#include <nrfx_systick.h>
#include <nrf_gpio.h>
#include <nrfx_rtc.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#define PIN_TXD 20
#define PIN_RXD 22

#define CLEAR_SCREEN "\033c"

#define LED1 28
#define LED2 29
#define LED3 30
#define LED4 31
#define LED_OFF 1
#define LED_ON 0

#define BUTTON_COUNT 4
#define BUTTON1 23
#define BUTTON2 24
#define BUTTON3 8
#define BUTTON4 9

extern nrfx_uarte_t instance;
extern uint8_t uarte_enabled;
extern nrfx_rtc_t rtc_instance;

void uarte_write(uint8_t *data, uint8_t length);
bool read_string(char *str, size_t max_len);
int read_int(void);
bool send_int(uint64_t num);
void init_stuff(void);
void init_uart(void);
void init_gpio(void);
int wait_for_any_button(void);
void toggle_leds(void);
uint8_t get_buttton_press();
int is_even(int num);
void set_random_seed(void);
int get_random_number(int upper, int lower);
void timer(int *gametime);
void play_reaction_game(void);
void start_game_gruppX(void);

#endif