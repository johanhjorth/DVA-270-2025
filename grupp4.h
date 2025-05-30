#ifndef GRUPP4_H
#define GRUPP4_H

#include <stdio.h>
#include <stdlib.h>
#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrfx_uarte.h> 
#include <nrfx_systick.h> 
#include <nrfx_rtc.h> 

// UARTE-pinnar
#define PIN_TXD (20)
#define PIN_RXD (22)

// Inställningar Snake (storlek på spelyta)
#define MTX_ROWS (20)
#define MTX_COLS (40)

// Inställning vilka knappar man trycker på
#define UP_KEY (119)
#define DOWN_KEY (115)
#define LEFT_KEY (97)
#define RIGHT_KEY (100)
#define EXIT (27)

// Skärmrensnings ANSI
#define CLEAR_SCREEN "\033c"

const extern nrfx_rtc_t rtc_instance;
extern nrfx_uarte_t uarte_instance; 
extern int uarte_enabled;
extern volatile bool tx_done; //Volatile gör att den kan hanteras utanför normala flödet
static uint8_t rx_byte; 

extern int snake, numgame; // Används för att se vilket spel man är i
extern int run; // Används för att pausa

// Variabler snake
extern int play_direction; 
extern int player_posXY[2];

// Variabler nummerfångaren
extern int target;
extern int num_showing;
extern int num_game_win;
extern int num_game_fail;

void start_game_grupp4(void);



#endif