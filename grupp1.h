#ifndef GRUPPX_1
#define GRUPPX_1

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

#include <nrfx_gpiote.h>
#include <soc/nrfx_irqs.h>
#include <nrf_gpiote.h>


// Define the pins used for transmitting and receiving data:
#define PIN_TXD 20
#define PIN_RXD 22
#define CLEAR_SCREEN "\033c"

/*
From the NRF5340DK documentation:
Button 1    P0.23   i.e. 23
Button 2    P0.24
Button 3    P0.08
Button 4    P0.09
*/

#define BUTTON1 23
#define BUTTON2 24
#define BUTTON3 8
#define BUTTON4 9

// Make the UART instance accessible in other files
extern nrfx_uarte_t instance;
extern uint8_t uarte_enabled;
extern nrfx_rtc_t rtc_instance;
extern volatile int resign;//imports the global varaible

typedef struct piece {
    char type;
    int x_coordinate;
    int y_coordinate;
} piece;

typedef struct chess_board {
    piece* board[8][8];
} chess_board;


void start_game_grupp1(void); // Main loop

// Function declarations from goodfuncs.c
void uarte_write(uint8_t *data, uint8_t length);
bool read_string(char *str, size_t max_len);
void init_stuff(void);
void init_uart(void);
void init_gpio(void);
void GPIOTE0_IRQHandler(void);
void init_button_interrupt(void);
void uart_event_handler(nrfx_uarte_event_t const *p_event, void *p_context);

// Function declarations from chess.c
void starting_positions(chess_board* chess);
void print_board(chess_board* chess);
int is_white(char type);
int pawn_moves(chess_board* chess, piece* p, int dest_y, int dest_x);
int rook_moves(chess_board* chess, piece* p, int dest_y, int dest_x);
int knight_moves(chess_board* chess, piece* p, int dest_y, int dest_x);
int bishop_moves(chess_board* chess, piece* p, int dest_y, int dest_x);
int queen_moves(chess_board* chess, piece* p, int dest_y, int dest_x);
int king_moves(chess_board* chess, piece* p, int dest_y, int dest_x);
int is_valid_move(chess_board* chess, piece* p, int dest_y, int dest_x);
int make_move(chess_board* chess, piece* p, int dest_y, int dest_x);
int parse_input(char input[], int* from_y, int* from_x, int* to_y, int* to_x);
#endif
