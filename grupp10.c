#include "grupp10.h"
// Inkluderar nödvändiga bibliotek för slump och tid



extern volatile int button_pressed; // Variabel som ändras vid knapptryckning
nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0); // UART-konfiguration
nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(0); // Realtidsklocka-instans



// Skicka data via UART
void uarte_write(uint8_t *data, uint8_t length) {
    nrfx_uarte_tx(&instance, data, length, 0); // Skicka data via UART
}

// Skicka en text via UART
void uart_send_string(const char* str) {
    char msg[400]; // Temporär lagring
    sprintf(msg, "%s", str); // Kopiera texten
    uarte_write(msg, strlen(msg)); // Skicka via UART
}

// Läs text från UART
bool read_string(char *str, size_t max_len) {
    nrfx_uarte_rx(&instance, (uint8_t*)str, max_len); // Hämta text
    return true;

    // Nedan kod körs aldrig eftersom funktionen redan returnerat
    uint8_t ch;
    int i = 0;

    while (i < max_len - 1) {
        nrfx_uarte_rx(&instance, &ch, 1); // Läs ett tecken
        if (ch == '\r' || ch == 13) break;  // Sluta vid Enter
        str[i++] = (char)ch; // Lagra tecknet
    }

    str[i] = '\0'; // Slutför textsträngen
}

// Läs ett heltal från användaren via UART
int read_int() {
    char buffer[50]; // Temporär lagring
    read_string(buffer, sizeof(buffer)); // Läs in från UART
    return atoi(buffer); // Konvertera text till heltal
}

// Ställ in slumpgenerator
void set_random_seed(void) {
    srand(nrf_rtc_counter_get(rtc_instance.p_reg)); // RTC används som frö
}

// Generera ett slumpmässigt nummer mellan två värden
int get_random_number(int lower, int upper) {
    return lower + rand() % (upper - lower + 1);
}

// Blinkande LED
void light_up_led(int led_pin) {
    nrf_gpio_pin_clear(led_pin);           // Tänd LED (aktiv låg)
    nrfx_systick_delay_ms(300);            // Vänta 0.3 sekunder
    nrf_gpio_pin_set(led_pin);             // Släck LED
}

// Vänta på att rätt knapp trycks
bool wait_for_correct_button(int led_index, int timeout_ms) {
    const int buttons[BUTTON_COUNT] = {BUTTON1, BUTTON2, BUTTON3, BUTTON4};
    nrfx_systick_state_t timer;
    nrfx_systick_get(&timer); // Starta timer

    while (!nrfx_systick_test(&timer, timeout_ms * 250)) { // Kontrollera om tiden har gått ut
        for (int i = 0; i < BUTTON_COUNT; i++) {
            if (nrf_gpio_pin_read(buttons[i]) == 0) { // Kontrollera om knapp tryckts
                return i == led_index; // Returnera sant om rätt knapp tryckts
            }
        }
    }
    return false; // Tiden gick ut eller fel knapp trycktes
}




// Spelets logik
void play_simon_says_game(void) {
    char msg[128];
    int8_t clearscreen[] = CLEAR_SCREEN;
    const int leds[BUTTON_COUNT] = {LED1, LED2, LED3, LED4};

    // Rensa skärmen och visa introduktion
    uarte_write(clearscreen, sizeof(clearscreen));
    uart_send_string("Handen måste ligga på bordet under spelet!\n\r\n\r");
    uart_send_string("Välkommen till Simon Says!\n\rFyra LEDs blinkar slumpmässigt!\n\r\n\r");

    set_random_seed(); // Ställ in slumpgenerator
    int level = 1;

    while (1) {
        uart_send_string("--------------------------------\n\r");
        sprintf(msg, "Nivå %d: Fokusera!\n\r", level);
        uart_send_string(msg);

        int sequence_length = 4;
        int led_sequence[sequence_length];

        // Skapa och visa LED-sekvens
        for (int i = 0; i < sequence_length; i++) {
            led_sequence[i] = get_random_number(0, BUTTON_COUNT - 1); // Slumpa LED-index
            light_up_led(leds[led_sequence[i]]); // Blink LED
            nrfx_systick_delay_ms(200);  // Kort paus
        }

        uart_send_string("Vilken LED blinkade sist? Tryck på rätt knapp!\n\r");

        // Vänta på att användaren trycker knapp
        bool correct = wait_for_correct_button(led_sequence[sequence_length - 1], 3000);

        if (correct) {
            uart_send_string("Bra jobbat!\n\r\n\r");
            level++; // Öka nivån
        } else {
            uart_send_string("Fel knapp! Spelet är slut.\n\r\n\r");
            break; // Avsluta spelet
        }

        uart_send_string("Tryck Enter för nästa nivå...\n\r");
        char next_round[10];
        read_string(next_round, sizeof(next_round)); // Vänta på Enter
    }

    uart_send_string("Spelet är över. Tack för att du spelade!\n\r\n\r");
    uart_send_string("Tryck Enter för att starta om...\n\r");
    char next_round[10];
    read_string(next_round, sizeof(next_round)); // Vänta på Enter innan omstart
}


// UART, RTC, SysTick, LED, knappar
void init_stuff(void) {
    // UART-konfiguration (TX ansluts till pinne 20, RX till pinne 22)
    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(20, 22);
    nrfx_uarte_init(&instance, &config, NULL); // Initiera UART-kommunikation

    // RTC-konfiguration och aktivering
    nrfx_rtc_config_t rtc_config = NRFX_RTC_DEFAULT_CONFIG;
    nrfx_rtc_init(&rtc_instance, &rtc_config, NULL);
    nrfx_rtc_enable(&rtc_instance); // Starta realtidsklockan

    // Initiera systemtick för tidsuppföljning och väntetider
    nrfx_systick_init();

    // Vänta tills knapp 1 trycks ner – används för att generera slumpmässigt frö
    while (nrf_gpio_pin_read(BUTTON1) == 1);
    srand(nrf_systick_val_get()); // Utnyttja systemtick för slumpvärde
    nrf_systick_val_clear(); // Återställ tickern efteråt

    // Konfigurera LED:ar som utgångar och se till att de är släckta
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_cfg_output(LED2);
    nrf_gpio_cfg_output(LED3);
    nrf_gpio_cfg_output(LED4);
    nrf_gpio_pin_write(LED1, LED_OFF);
    nrf_gpio_pin_write(LED2, LED_OFF);
    nrf_gpio_pin_write(LED3, LED_OFF);
    nrf_gpio_pin_write(LED4, LED_OFF);

    // Konfigurera knappar som ingångar med pull-up motstånd
    nrf_gpio_cfg_input(BUTTON1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON2, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON3, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON4, NRF_GPIO_PIN_PULLUP);
}

// Avbrottshanterare för GPIOTE (knapptryck via knapp 1)
void GPIOTE0_IRQHandler(void) {
    if (NRF_GPIOTE0->EVENTS_IN[0]) {              // Kontrollera om avbrottet triggades från kanal 0 (associerad med knapp 1)
        NRF_GPIOTE0->EVENTS_IN[0] = 0;            // Återställ händelseflaggan för att möjliggöra nya avbrott
        button_pressed = 1;                       // Signalera att knappen har aktiverats
    }
}

// När knappen aktiveras vill vi att programmet ska reagera direkt. För detta konfigureras GPIOTE att känna av knapptrycket 
// och utlösa en specifik funktion. När knappen aktiveras körs GPIOTE0_IRQHandler, vilket justerar variabeln button_pressed till 1.
// Detta gör att programmet omedelbart registrerar att knappen har tryckts utan att behöva utföra kontinuerliga kontroller.


// Initialiserar knapp 1 med avbrottshantering
void init_button_interrupt(void) {
    NRF_P0->PIN_CNF[BUTTON1_PIN] = // Konfigurera knappens anslutning:
        (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) | // som ingång
        (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) | // kopplad till signalinläsning
        (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos); // med pull-up motstånd för stabilitet

    NRF_GPIOTE0->CONFIG[0] = // Ställ in GPIOTE:
        (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) | // för att registrera händelser
        (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos) | // reagera på signalfall (nedtryckning)
        (BUTTON1_PIN << GPIOTE_CONFIG_PSEL_Pos); // associera med knappens pinne

    NRF_GPIOTE0->INTENSET = GPIOTE_INTENSET_IN0_Msk; // Aktivera avbrott för knapphändelse

    NVIC_SetPriority(GPIOTE0_IRQn, 1); // Ange prioritet (1 = relativt hög) – hanterar avbrott!
    NVIC_EnableIRQ(GPIOTE0_IRQn); // Slå på avbrott
}


// Programstart
void start_game_grupp10(void){
    init_stuff();                // Starta upp nödvändiga komponenter
    init_button_interrupt();     // Aktivera avbrottshantering för knapp 1

    int8_t clearscreen[] = CLEAR_SCREEN; // ANSI-kommandon för att rensa skärmen i terminalen
    uarte_write(clearscreen, sizeof(clearscreen));

    while (1) {
        play_simon_says_game(); // Starta spelet och håll det igång i en oändlig loop
    }

    return 0;
}