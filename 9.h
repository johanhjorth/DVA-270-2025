#ifndef GRUPP_9
#define GRUPP_9

#include <nrf5340_application.h>
#include <nrfx_config.h> 
#include <nrf.h>
#include <nrf_gpio.h>
#include <nrfx_uarte.h>
#include <nrfx_systick.h>
#include <nrfx.h>
#include <string.h>
#include <stdio.h>
#include <nrfx_rtc.h>
#include <nrf_gpio.h>

#define PIN_TX          (20)
#define PIN_RX          (22)

#define LED1 (28)
#define LED_OFF 1
#define LED_ON 0
#define LED_OFF 1
#define LED_ON 0

#define BUTTON1 23

#define PIN_HIGH 0
#define PIN_LOW 1

extern nrfx_uarte_t instance;
extern nrfx_rtc_t rtc_instance2;

void uarte_rtc_error();
void led_of();
void uarte_write(char* data, int length);
int read_int();
void read_string(char string[]);

void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context);
void run_timer();

void game();
void led();
void print_best_time();
void start_game_grupp9(void);

#endif
