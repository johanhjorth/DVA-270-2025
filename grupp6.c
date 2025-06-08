#include "grupp6.h"
#include <nrfx_gpiote.h>
#include <nrf_gpio.h>
#include <soc/nrfx_irqs.h>
#include <nrf5340_application.h>
#include <nrf_gpiote.h>
#include <nrfx_systick.h>

#define BUTTON1_PIN 23 // Sätter knapp 1 till pinne nummer 23
volatile int button_pressed = 0; // En variabel som ändras i interrupt – visar om knapp tryckts


// Dessa variabler används även i andra filer
extern volatile int button_pressed; // Variabel som visar om knappen tryckts
nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0); // UART-instans
nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(0); // RTC-instans

// Funktion för att skicka data via UART
void uarte_write(uint8_t *data, uint8_t length) {
    nrfx_uarte_tx(&instance, data, length, 0); // Skicka data via UART utan callback
}

// Funktion för att skicka en textsträng via UART
void uart_send_string(const char* str)
{
    char msg[400]; // Temporär buffer
    sprintf(msg, "%s", str); // Kopiera texten till msg
    uarte_write(msg, strlen(msg)); // Skicka texten via UART
}

// Funktion för att läsa en textsträng via UART
bool read_string(char *str, size_t max_len) {
    nrfx_uarte_rx(&instance, (uint8_t*)str, max_len); // Starta läsning (blocking)
    return true;

    // OBS: Denna kod nedan körs aldrig pga return ovan!
    uint8_t ch;
    int i = 0;

    while (i < max_len - 1)
    {
        nrfx_uarte_rx(&instance, &ch, 1); // Läs ett tecken
        if (ch == '\r' || ch == 13) break;  // Avsluta vid Enter
        str[i++] = (char)ch; // Spara tecknet
    }

    str[i] = '\0'; // Avsluta strängen
}

// Funktion för att läsa ett heltal som användaren skriver in
int read_int()
{
    char buffer[50]; // Buffer för texten
    read_string(buffer, sizeof(buffer)); // Läs in text via UART
    return atoi(buffer); // Konvertera till heltal och returnera
}

// Sätter ett slumpfrö baserat på RTC
void set_random_seed(void) {
    srand(nrf_rtc_counter_get(rtc_instance.p_reg)); // Använd RTC-tid som frö
}

// Returnerar ett slumptal mellan lower och upper (inklusive)
int get_random_number(int upper, int lower) {
    return lower + rand() % (upper - lower + 1);
}

// Spelets huvudfunktion
void play_reaction_game(void) {
    char msg[100]; // Buffer för meddelanden
    int8_t clearscreen[] = CLEAR_SCREEN; // Kod för att rensa skärmen

    // Visa instruktion till användaren
    strcpy(msg, "Tryck på knappen när LED lyser!\n\r");
    uarte_write((uint8_t*)msg, strlen(msg));

    // Vänta en slumpmässig tid mellan 2 och 5 sekunder
    set_random_seed();
    int wait_time_ms = get_random_number(5000, 2000);
    nrfx_systick_delay_ms(wait_time_ms); // Väntar så länge

    // Tänd LED (signalen att trycka)
    nrf_gpio_pin_clear(LED1); // LED ON

    // Starta timer och nollställ knappstatus
    button_pressed = 0;
    nrfx_systick_state_t timer_state;
    nrfx_systick_get(&timer_state); // Spara starttid

    // Vänta tills spelaren trycker eller max 250 000 ticks (3 sekunder)
    while (1) {
        if (button_pressed) { // Om spelaren tryckt
            break;
        }

        if (nrfx_systick_test(&timer_state, 250000)) { // Om tiden har gått ut
            break;
        }
    }

    // Släck LED
    nrf_gpio_pin_set(LED1); // LED OFF

    // Rensa skärmen
    uarte_write(clearscreen, sizeof(clearscreen));

    // Kolla om spelaren hann trycka
    if (button_pressed) {
        strcpy(msg, "Du VANN!\n\r\n\r");
    } else {
        strcpy(msg, "För långsam. Du FÖRLORADE.\n\r\n\r");
    }
    uarte_write((uint8_t*)msg, strlen(msg)); // Skicka resultat

    // Fråga om spelaren vill spela igen
    strcpy(msg, "Spela igen? Tryck på Enter...\n\r");
    uarte_write((uint8_t*)msg, strlen(msg));

    char dummy[10];
    read_string(dummy, 10); // Vänta på att spelaren trycker på Enter
}

// Interrupt-funktion som körs när knapp 1 trycks
void GPIOTE0_IRQHandler(void) {
    if (NRF_GPIOTE0->EVENTS_IN[0]) { // Om ett tryck registrerats
        NRF_GPIOTE0->EVENTS_IN[0] = 0; // Nollställ händelsen (kvittera)
        button_pressed = 1; // Sätt variabeln till 1 = knapp tryckt
    }
}

// Initierar knapp 1 med interrupt
void init_button_interrupt(void) {
    NRF_P0->PIN_CNF[BUTTON1_PIN] = // Ställ in knapp-pinnen:
        (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) | // som input
        (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) | // kopplad till input
        (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos); // med pull-up motstånd

    NRF_GPIOTE0->CONFIG[0] = // Konfigurera GPIOTE:
        (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) | // för event-läge
        (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos) | // reagera på fallande signal (tryck)
        (BUTTON1_PIN << GPIOTE_CONFIG_PSEL_Pos); // använd knappens pinne

    NRF_GPIOTE0->INTENSET = GPIOTE_INTENSET_IN0_Msk; // Aktivera avbrott för knapp-event

    NVIC_SetPriority(GPIOTE0_IRQn, 1); // Sätt prioritet (1 = ganska hög) . den hanterar interupten!
    NVIC_EnableIRQ(GPIOTE0_IRQn); // Aktivera avbrottet
}

// Initierar alla grundläggande delar av systemet
void init_stuff(void) {
    // UART - Seriell kommunikation
    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(20, 22); // TX = pin 20, RX = pin 22
    nrfx_uarte_init(&instance, &config, NULL); // Starta UART

    // RTC - Real Time Clock
    nrfx_rtc_config_t rtc_config = NRFX_RTC_DEFAULT_CONFIG; // Standardinställningar
    nrfx_err_t err = nrfx_rtc_init(&rtc_instance, &rtc_config, NULL); // Initiera RTC
    nrfx_rtc_init(&rtc_instance, &rtc_config, NULL); // Initiera igen (möjligen onödigt dubbelt)
    nrfx_rtc_enable(&rtc_instance); // Aktivera RTC

    // Systick - Systemtimer
    nrfx_systick_init(); // Starta SysTick-timer
    while (nrf_gpio_pin_read(BUTTON1) == 1); // Vänta tills knapp 1 trycks in
    srand(nrf_systick_val_get()); // Slumpa startvärde baserat på tiden

    nrf_systick_val_clear(); // Nollställ systick
    uint32_t pretime = nrf_systick_val_get(); // Läs tiden efter nollställning

    // LED
    nrf_gpio_cfg_output(LED1); // Gör LED1 till output
    nrf_gpio_pin_write(LED1, LED_OFF); // Släck LED1

    // Aktivera knappar som input med pull-up
    nrf_gpio_cfg_input(BUTTON1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON2, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON3, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON4, NRF_GPIO_PIN_PULLUP); // Används inte men initieras ändå
}

void start_game_grupp6(void){
    init_stuff(); // Starta upp alla delar (UART, RTC, LED, knappar...)
    init_button_interrupt(); // Aktivera knapp-interrupt

    int8_t clearscreen[] = CLEAR_SCREEN; // Sträng som rensar skärmen i Putty
    uarte_write(clearscreen, sizeof(clearscreen)); // Skicka strängen via UART

    while (1) {
        play_reaction_game(); // Kör reaktionsspelet (om och om igen)
    }

    return 0; // Avslutar programmet (kommer aldrig hit)
}
