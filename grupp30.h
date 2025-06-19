#ifndef GRUPP30_H
#define GRUPP30_H

#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrf.h>
#include <nrf_gpio.h>
#include <nrf_uarte.h>
#include <nrfx_uarte.h>
#include <nrfx_systick.h>
#include <stdio.h>
#include <nrfx_systick.h>
#include <nrfx_uarte.h>
#include <stdlib.h>
#include <string.h>
#include <nrfx_rtc.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <stddef.h>

#define PIN_TXD 20
#define PIN_RXD 22


extern nrfx_uarte_config_t config;
extern nrfx_uarte_t instance;


// Initierar ett nytt spel
void init_game(void);

// Skriver ut via UART
void uarte_write(uint8_t* data, uint8_t length);

// Skriver ut spelplanen till terminalen
void print_board(void);

// Försöker göra ett drag på angiven rad och kolumn
int make_move(int row, int col);

// Kontrollerar om någon spelare har vunnit
int check_winner(void);

// Växlar till nästa spelare
void switch_player(void);

// Returnerar nuvarande spelare (1 eller 2)
int get_current_player(void);

// UART-event handler som läser tangenttryck
void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context);

// Startar spelet
void start_game_grupp30(void);

// Initierar UART
void uarte_init(void);

#endif