#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrf.h>
#include <nrfx_bsdriver.h>
#include <assert.h>
#include <stdio.h>

// Standardiserad inkludering av alla gruppers headerfiler
#include "grupp1.h"
#include "grupp2.h"
#include "grupp3.h"
#include "grupp4.h"
#include "grupp5.h"
#include "grupp6.h"
#include "grupp7.h"
#include "grupp8.h"
#include "grupp9.h"
#include "grupp10.h"
#include "grupp11.h"
#include "grupp12.h"
#include "grupp13.h"
#include "grupp14.h"
#include "grupp15.h"
#include "grupp16.h"
#include "grupp17.h"
#include "grupp18.h"
#include "grupp19.h"
#include "grupp20.h"
#include "grupp21.h"
#include "grupp22.h"
#include "grupp23.h"
#include "grupp24.h"
#include "grupp25.h"
#include "grupp26.h"
#include "grupp27.h"
#include "grupp28.h"
#include "grupp29.h"
#include "grupp30.h"
#include "grupp31.h"

/* By Daniel Morberg*/

int main(void)
{
    int keypress = get_key();

    switch (keypress)
    {
        case 1:
            start_game_grupp1();
            break;
        case 2:
            start_game_grupp2();
            break;
        case 3:
            start_game_grupp3();
            break;
        case 4:
            start_game_grupp4();
            break;
        case 5:
            start_game_grupp5();
            break;
        case 6:
            start_game_grupp6();
            break;
        case 7:
            start_game_grupp7();
            break;
        case 8:
            start_game_grupp8();
            break;
        case 9:
            start_game_grupp9();
            break;
        case 10:
            start_game_grupp10();
            break;
        case 11:
            start_game_grupp11();
            break;
        case 12:
            start_game_grupp12();
            break;
        case 13:
            start_game_grupp13();
            break;
        case 14:
            start_game_grupp14();
            break;
        case 15:
            start_game_grupp15();
            break;
        case 16:
            start_game_grupp16();
            break;
        case 17:
            start_game_grupp17();
            break;
        case 18:
            start_game_grupp18();
            break;
        case 19:
            start_game_grupp19();
            break;
        case 20:
            start_game_grupp20();
            break;
        case 21:
            start_game_grupp21();
            break;
        case 22:
            start_game_grupp22();
            break;
        case 23:
            start_game_grupp23();
            break;
        case 24:
            start_game_grupp24();
            break;
        case 25:
            start_game_grupp25();
            break;
        case 26:
            start_game_grupp26();
            break;
        case 27:
            start_game_grupp27();
            break;
        case 28:
            start_game_grupp28();
            break;
        case 29:
            start_game_grupp29();
            break;
        case 30:
            start_game_grupp30();
            break;
        case 31:
            start_game_grupp31();
            break;
        default:
            // Hantera ogiltigt knapptryck om det behövs
            break;
    }
}
