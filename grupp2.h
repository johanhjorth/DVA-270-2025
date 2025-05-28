#ifndef GRUPP2_H
#define GRUPP2_H

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

#define BUTTON1 23

#define PIN_HIGH 0
#define PIN_LOW 1

#define LED1 (28)
#define LED2 (29)
#define LED3 (30)
#define LED4 (31)

#define LED_OFF 1
#define LED_ON 0

#define NUMBER_OF_LEDS 4

extern nrfx_uarte_t instance;
extern nrfx_rtc_t rtc_instance2;

void uarte_rtc_error();
void leds_stuff();
void uarte_write(char* data, int length);

//*************************************************** */
//*************************************************** */
void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context);
void run_timer();

//*************************************************** */
//*************************************************** */
void start_game_grupp2(void);
void random_array();
void lit_leds_in_order();
void lit_led(int pin);
//void read_array();
//int compare_array();
//*************************************************** */
//*************************************************** */
void print_best_time();

#endif