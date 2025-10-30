#include "Labb4.h"

nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
nrfx_rtc_t rtc_instance2 = NRFX_RTC_INSTANCE(0);

int arrRand[NUMBER_OF_LEDS];  // array med slumpade tal som ska slumpa leds.
int arrTemp[NUMBER_OF_LEDS];  // temp array som tar användarens inmatning som sedan omvandlas till arrInput.
int arrInput[NUMBER_OF_LEDS]; // Array som ska stämma överens med slumpad array.

int BEST_TIME = -1;
int tick;
char msgTimes[50];
char msgTotalWrong[] = "Wrong in total, try again!";
char msgWrong[] = "\n\rWrong led\n\r";

volatile bool suspend_timer_print = false;
volatile int input_index = 0;
int answear[NUMBER_OF_LEDS];

static uint8_t uarte_buffer;
volatile bool timerStop = true;

//***************************************************
//***************************************************
void uarte_rtc_error()
{
    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(20, 22);
    nrfx_err_t errr = nrfx_uarte_init(&instance, &config, uarte_handler); // uarte_handler

    nrfx_rtc_config_t config2 = NRFX_RTC_DEFAULT_CONFIG;
    nrfx_err_t errr2 = nrfx_rtc_init(&rtc_instance2, &config2, NULL);
    nrfx_rtc_enable(&rtc_instance2);
    nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));
}

void leds_stuff()
{
    // LED som output
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_cfg_output(LED2);
    nrf_gpio_cfg_output(LED3);
    nrf_gpio_cfg_output(LED4);

    // Nollar LEDs
    nrf_gpio_pin_write(LED1, LED_OFF);
    nrf_gpio_pin_write(LED2, LED_OFF);
    nrf_gpio_pin_write(LED3, LED_OFF);
    nrf_gpio_pin_write(LED4, LED_OFF);
}

void uarte_write(char *data, int length)
{
    nrfx_uarte_tx(&instance, data, length, 0);
}

// ---- Helper: skriv en hel rad säkert (tystar timerraden, rensar raden, skriver texten) ----
static void uart_print_line(const char *s)
{
    // Tysta timerns "Tid:"-rad medan vi skriver
    suspend_timer_print = true;

    // Rensa aktuell terminalrad (ESC[2K) och flytta till början av raden
    while (nrfx_uarte_tx_in_progress(&instance))
    {
    }
    uarte_write("\r\x1b[2K", 5);

    // Skriv meddelandet
    while (nrfx_uarte_tx_in_progress(&instance))
    {
    }
    uarte_write((char *)s, strlen(s));

    // Tillåt timern att skriva igen
    suspend_timer_print = false;
}

//***************************************************
//***************************************************
// ------------------- Interrupt ---------------------
void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)
{
    (void)p_context;

    if (p_event->type != NRFX_UARTE_EVT_RX_DONE)
    {
        nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));
        return;
    }

    // Läs mottaget tecken (din nrfx-variant använder .rx)
    uint8_t input = p_event->data.rx.p_data[0];
    int value = 0;

    // Behåller din mappning: '4'->1, '5'->2, '1'->3, '2'->4
    switch (input)
    {
    case '4':
        value = 1;
        break;
    case '5':
        value = 2;
        break;
    case '1':
        value = 3;
        break;
    case '2':
        value = 4;
        break;
    default:
        // ignorera CR/LF/annat skräp och armera ny RX
        nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));
        return;
    }

    // Spara input
    answear[input_index] = value;
    input_index++;

    // När 4 inputs kommit: räkna poäng och skriv resultat
    if (input_index == NUMBER_OF_LEDS)
    {
        int points = 0;
        for (int i = 0; i < NUMBER_OF_LEDS; i++)
        {
            if (answear[i] == arrRand[i])
                points++;
        }

        // Stoppa timern direkt så den inte skriver över resultatet
        timerStop = true;

        if (points == NUMBER_OF_LEDS)
        {
            char msg[64];
            snprintf(msg, sizeof(msg), "\n\rAlla Rätt!", tick * 100);
            uart_print_line(msg);
        }
        else
        {
            char msg[64];
            snprintf(msg, sizeof(msg), "\n\r%d av %d rätt :(", points, NUMBER_OF_LEDS);
            uart_print_line(msg);
        }

        // Reset för nästa runda
        input_index = 0;
    }

    // Armera nästa mottagning
    nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));
}

void run_timer()
{
    tick = 0;
    timerStop = false;

    nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));

    char msgTimer[100];

    while (!timerStop)
    {
        if (!suspend_timer_print)
        {
            sprintf(msgTimer, "\r\x1b[2KTid: %d ms", tick * 100); // \x1b[2K = clear line
            while (nrfx_uarte_tx_in_progress(&instance))
            {
            }
            uarte_write(msgTimer, strlen(msgTimer));
        }

        nrfx_systick_delay_ms(100);
        tick++;
    }

    while (nrfx_uarte_tx_in_progress(&instance))
    {
    }
    uarte_write("\r\x1b[2K\r\n", 6);
}

//***************************************************
//***************************************************
//------------------- Spelet ---------------------
void start_game_grupp31(void)
{
    input_index = 0;
    random_array();
    lit_leds_in_order();

    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));

    char msgEnter[] = "\r\nEnter the last led\r\n";
    while (nrfx_uarte_tx_in_progress(&instance))
    {
    }
    uarte_write(msgEnter, strlen(msgEnter));

    nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer)); // startar första rx
    run_timer();
}

void random_array()
{
    int i;
    for (i = 0; i < NUMBER_OF_LEDS; i++)
    {
        arrRand[i] = (rand() % (NUMBER_OF_LEDS)) + 1;
    }
}

void lit_leds_in_order()
{
    for (int i = 0; i < NUMBER_OF_LEDS; i++)
    {
        if (arrRand[i] == 1)
            lit_led(LED1);
        if (arrRand[i] == 2)
            lit_led(LED2);
        if (arrRand[i] == 3)
            lit_led(LED3);
        if (arrRand[i] == 4)
            lit_led(LED4);
    }
}

void lit_led(int pin)
{
    nrf_gpio_pin_write(pin, 0);
    nrfx_systick_delay_ms(300);
    nrf_gpio_pin_write(pin, 1);
    nrfx_systick_delay_ms(150);
}
