#ifndef GRUPPX_31
#define GRUPPX_31


#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrf.h>
#include <nrfx_uarte.h>
#include <nrf_gpio.h>
#include <nrfx_systick.h>
#include <assert.h> 
#include <nrfx_rtc.h>


#define LED1 (28) 
#define LED2 (29) 
#define LED3 (30) 
#define LED4 (31)
#define PIN_TXD 20
#define PIN_RXD 22
#define BUTTON1 23
#define BUTTON2 24
#define BUTTON3 8
#define CLEAR_SCREEN "\033c" 

uint32_t read_int(nrfx_uarte_t *instance, uint8_t num[]);
void init_gpio();
void button_status();
void start_game();
void start_game_grupp31();
void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context);

#endif