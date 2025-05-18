#ifndef NONBLOCKING_H
#define NONBLOCKING_H
#define CLEAR_SCREEN "\033c"

#include <nrfx.h>
#include <assert.h>

#include <stdio.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrf.h>
#include <nrf_gpio.h>
#include <nrfx_uarte.h>
#include <nrfx_systick.h>
#include <time.h>
#include <nrfx_glue.h>
/*******************************************************/
/* DVA270 - Interface för dubbellänkad lista           */
/* Anpassad efter interface från DVA244                */
/* av Stefan Bygde och Caroline Uppsäll                */
/* Vid fel eller frågor, kontakta jonas.larsson@mdu.se */
/*******************************************************/

extern nrfx_uarte_t instance;

void the_Mighty_Assembly_Of_Arrays(int x, int y, int p, int f);
void the_Mighty_Assembly_Of_Arrays_Printer();
void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context);
void send_int(int int1);
void uarte_write(uint8_t* data, uint8_t length);
void print_newLine(void);

#endif
