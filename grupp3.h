#ifndef GRUPP3_H
#define GRUPP3_H


#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrf.h>
#include <nrfx_uarte.h>
#include <nrf_gpio.h>
#include <nrfx_systick.h>
#include <assert.h> 
#include <nrfx_rtc.h>

#define PIN_TXD 20
#define PIN_RXD 22

#define CLEAR_SCREEN "\033c" 

uint32_t ReadInt(nrfx_uarte_t *instance, uint8_t num[]);
void init_gpio();
void button_status();
void start_game_grupp3();
void StartGame();
void UarteHandler(nrfx_uarte_event_t const *p_event, void *p_context);

#endif