#ifndef GRUPP10
#define GRUPP10

// Inkludera bibliotek för att hantera hårdvara och systemfunktioner
#include <nrfx_uarte.h>
#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrf.h>
#include <nrfx_systick.h>
#include <nrf_gpio.h>
#include <nrfx_rtc.h>

// Inkludera standardbibliotek för grundläggande operationer
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include <nrfx_gpiote.h>
#include <nrf_gpio.h>
#include <soc/nrfx_irqs.h>
#include <nrf5340_application.h>
#include <nrf_gpiote.h>
#include <nrfx_systick.h>
// Definiera LED-lampornas pinnar och tillstånd
#define LED1 28
#define LED2 29
#define LED3 30
#define LED4 31
#define LED_OFF 1  // LED av (hög signal)
#define LED_ON 0   // LED på (låg signal)

// Definiera knappar och deras antal
#define BUTTON_COUNT 4
#define BUTTON1 23
#define BUTTON2 24
#define BUTTON3 8
#define BUTTON4 9

// Definiera UART-kommunikationens pinnar
#define PIN_TXD 20  // Pin för att skicka data
#define PIN_RXD 22  // Pin för att ta emot data

// ANSI-kommandot för att rensa terminalskärmen
#define CLEAR_SCREEN "\033c"

#define BUTTON1_PIN 23 // Definierar knapp 1 att vara kopplad till pinne 23
volatile int button_pressed = 0; // En variabel som ändras vid avbrott – visar om knapp har blivit nedtryckt


// Deklarera externa variabler som används i programmet
extern nrfx_uarte_t instance; // UART-instans
extern uint8_t uarte_enabled; // Flagga för om UART är aktiverad
extern nrfx_rtc_t rtc_instance; // Realtidsklocka-instans

// Funktionsdeklarationer
void uarte_write(uint8_t *data, uint8_t length);  // Skicka data via UART
bool read_string(char *str, size_t max_len);  // Läs en textsträng från UART
int read_int(void);  // Läs ett heltal från UART
bool send_int(uint64_t num);  // Skicka ett heltal via UART
void init_stuff(void);  // Initiera systemets komponenter
void init_uart(void);  // Initiera UART-kommunikation
void init_gpio(void);  // Initiera GPIO (LED/knappar)
int wait_for_any_button(void);  // Vänta tills en knapp trycks ned
void toggle_leds(void);  // Växla LED-status
uint8_t get_buttton_press();  // Kontrollera vilken knapp som tryckts
int is_even(int num);  // Kontrollera om ett tal är jämnt
void set_random_seed(void);  // Ställ in slumpgenerator
int get_random_number(int upper, int lower);  // Generera ett slumpmässigt nummer
void timer(int *gametime);  // Hantera spelets timer
void play_simon_says_game(void);  // Huvudfunktion för spelet
void start_game_grupp10(void);

#endif
