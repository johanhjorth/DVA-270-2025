#ifndef GRUPP19_H
#define GRUPP19_H

#include <nrfx_uarte.h>

void start_game_grupp19(void);
void game_update(void);
void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context);

#endif // GRUPP19_H
