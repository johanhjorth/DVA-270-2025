#ifndef GRUPP21_H
#define GRUPP21_H

#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrf.h>
#include <nrfx_uarte.h>
#include <nrf_gpio.h>
#include <nrfx_systick.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <nrfx_rtc.h>
#include <time.h>
#include "list.h"
//#include "BStree.h"
extern nrfx_uarte_t instance2;
extern nrfx_rtc_t rtc_instance;

#define BUTTON1 23
#define BUTTON2 24
#define BUTTON3 8
#define BUTTON4 9
#define OFF 1
#define ON 0
#define PIN_TXD 20
#define PIN_RXD 22









void uarte_write(char* data, int length);

void read_string(char *buffer, int max_len);

int read_int();

void send_int(int tal);

void print_n();

int Rand_int(int range);

int* Fill_array(int size, int range);

int is_sorted_arr(int arr[], int size);

void Fill_list(int size, int range, List *list);

int is_sorted_list(List *list);

void Switch_Nodes(List *list, Data data);

void bubble_sort(List *list);

int quick_sort(int arr[], int start, int end);

void insertion_sort(int arr[], int size);

void start_game_grupp21();


#endif