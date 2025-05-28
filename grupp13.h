#ifndef GRUPP13_H
#define GRUPP13_H

#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrf.h>
#include <nrfx_uarte.h>
#include <nrfx_systick.h>
#include <nrfx_rtc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CLEAR_SCREEN "\033c"
#define XSIZE 30
#define YSIZE 15
#define ON 0
#define OFF 1
#define REFRESHRATE 50
#define MAX_ATTEMPTS 10
#define PLAYERPOSITION 1
#define SCOREPOSITION 2
#define EMPTYPOSITION 0

#define kHz (32.768)

//SET TX and RX pins. Default is 20 and 22 for NRF5340DK.
#define PIN_TX          (20)
#define PIN_RX          (22)

int generate_srand_seed();
int generate_random_number(int rtc_counter_in_millisec);
int convert_rtc_counter_to_millisec(int rtc_counter);
static void printArray();
static void generate_score_pos();
static void increase_score();
static void game_end_message();
void uarte_write(char* data, int length);
void game_loop();
void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context);
void start_game_grupp13(void);

#endif