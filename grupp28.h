#ifndef GRUPP28_H
#define GRUPP28_H

#include <stdbool.h>
#include <stdint.h>

#define BUTTON_PIN_1 23
#define BUTTON_PIN_2 24
#define BUTTON_PIN_3 8
#define BUTTON_PIN_4 9

#define LED_PIN_1 28
#define LED_PIN_2 29
#define LED_PIN_3 30
#define LED_PIN_4 31

void start_game_gruppX(void);
void uarte_init();
void simple_delay_ms(uint32_t ms);
void uarte_write(char* data, int length);
void first_use_led();
void button_init(void);
void flash_led(uint8_t index, uint16_t delay_ms);
void play_sequence(uint8_t length);
int wait_for_button_press(void);
bool check_user_input(uint8_t length);
void add_random_to_sequence(void);
uint8_t get_score(void);
uint8_t get_sequence_length(void);
void rtc_init(void);

#endif
