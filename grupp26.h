#ifndef GRUPP26_H
#define GRUPP26_H
#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrf.h>
#include <nrf_gpio.h>
#include <nrfx_uarte.h>
#include <nrfx_systick.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h> 
#include <nrfx_rtc.h>
//#include <grupp26.h>


//SET TX and RX pins. Default is 20 and 22 for NRF5340DK.
#define PIN_TX          (20)
#define PIN_RX          (22)


/*
From the NRF5340DK documentation:
Button 1    P0.23   i.e. 23
Button 2    P0.24
Button 3    P0.08
Button 4    P0.09
LED 1       P0.28   i.e. 28
LED 2       P0.29
LED 3       P0.30
LED 4       P0.31
*/


#define LED1 (28)
#define LED2 (29)
#define LED3 (30)
#define LED4 (31)
#define BUTTON1 (23)
#define BUTTON2 (24)
#define BUTTON3 (8)
#define BUTTON4 (9)

#define LED_OFF (1)
#define LED_ON (0)




#define LED_ON_TIME 200
#define CLEAR_SCREEN "\033c"

void inituart(void);
void Send_Int(int value);
int Read_Int( void );
void Read_String(char *data, int max_length);
void uarte_write(char* data, int length);
void __assert_func(const char *file, int line, const char *func, const char *expr);
void start_game_grupp26(void);

typedef struct employee{

    char name[20];
    int birthYear;
    int moneyTalks;
}Employee;

void AddEmployee(char*name, int *birthYear);
void AddMoney (Employee EmployeeToUpdate, int moneyToAdd);
void printInfo(Employee EmployeeToGossip);



#endif