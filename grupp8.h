#ifndef GRUPP8_H
#define GRUPP8_H

#include <nrfx_systick.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <nrfx_uarte.h>
#include <nrf.h>
#include <string.h>


void start_game_grupp8(void);
void draw_grid(uint32_t seconds_left);
void init_game(void);
void process_input(char c);
bool game_should_quit(void);
int get_score(void);
void init_uart(void);
void uarte_write(char* data, int length);
bool has_new_uart_input(void);
char get_uart_char(void);

static bool new_data = false;
static volatile bool should_redraw = false;

#endif