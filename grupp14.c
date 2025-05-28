#include "LEDgame.h"
nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(0);
nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);

/*************** Functions Game ****************/

const int led_pins[NUM_LEDS] = {LED1, LED2, LED3, LED4};
const int button_pins[NUM_LEDS] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
int sequence[MAX_SEQUENCE_LENGTH];
int game_round = 0;

char uarte_buffer[1];
char buffer1[100];
static char rx_char;
static bool new_data = false;
int simulated_button = -1;

// Delay function (assuming you have nrfx-based delay)
void delay_ms(uint32_t ms) {
    nrfx_systick_delay_ms(ms);
}

// Light up an LED briefly
void flash_led(int index, uint32_t duration) {
    nrf_gpio_pin_set(led_pins[index]);
    delay_ms(duration);
    nrf_gpio_pin_clear(led_pins[index]);
    delay_ms(INTERVAL_TIME);
}

// Get button press index, or -1 if none
int get_button_press() {
    for (int i = 0; i < NUM_LEDS; i++) {
        if (nrf_gpio_pin_read(button_pins[i]) == 0) {
            delay_ms(BUTTON_DEBOUNCE_DELAY); // debounce
            while (nrf_gpio_pin_read(button_pins[i]) == 0); // wait for release
            return i;
        }
    }
    return -1;
}

// Main game loop
void memory_game() {
    
    game_round = 0;
    uint32_t led_duration = LED_ON_TIME;
    char msg[] = "Welcome to Memory Master!\n\r";
    char msg2[] = "Wrong! Game Over.\n\r";
    char msg3[] = "Correct!\n\r";
    char msg4[] = "Difficulty: Medium\n\r";
    char msg5[] = "Difficulty: Hard\n\r";
    char msg6[] = "Congratulations! You won!\n\r";

    uarte_write(msg, strlen(msg));

    while (game_round < MAX_SEQUENCE_LENGTH) {
        sequence[game_round] = getRand() % NUM_LEDS;

        // Announce game_round
        char msg1[100];
        sprintf(msg1, "game_Round %d                       (Press h for hint)\n", game_round + 1);
        uarte_write(msg1, strlen(msg1));

        // Show the full sequence
        for (int i = 0; i <= game_round; i++) {
            flash_led(sequence[i], led_duration);
        }

        // Get user input
        for (int i = 0; i <= game_round; i++) {
            int input = -1;
            while ((input = get_button_press()) == -1);

            if (input != sequence[i]) {
                uarte_write(msg2, strlen(msg2));
                return;
            }
        }

        // Next game_round
        uarte_write(msg3, strlen(msg3));
        delay_ms(100);
        game_round++;

        // Adjust difficulty
        if (game_round == 4) {
            led_duration = 400;
            uarte_write(msg4, strlen(msg4));
        } else if (game_round == 8) {
            led_duration = 300;
            uarte_write(msg5, strlen(msg5));
        }

        delay_ms(1000);
    }

    uarte_write(msg6, strlen(msg6));
}


/*************** UARTE ****************/

void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)
{
    if (p_event->type == NRFX_UARTE_EVT_RX_DONE)
    {
        rx_char = p_event->data.rx.p_data[0];
        new_data = true;

        if (rx_char == 'z') 
        {
            sprintf(buffer1, "\n\rMy name is the real slim shady\n\r");
            uarte_write(buffer1, strlen(buffer1));
        }
else if (rx_char == 'H' || rx_char == 'h')
{
    uarte_write("\n\rHint: Press buttons in this order:\n\r", strlen("\n\rHint: Press buttons in this order:\n\r"));
    for (int i = 0; i <= game_round; i++) {
        sprintf(buffer1, "B%d\n\r", sequence[i] + 1);  // sequence[i] is 0-based index
        uarte_write(buffer1, strlen(buffer1));
    }
}

        else if (rx_char >= '0' && rx_char <= '3')
        {
            simulated_button = rx_char - '0';
        }

        nrfx_uarte_rx(&instance, uarte_buffer, sizeof(uarte_buffer));
    }
}



void init_uart(void) { 
    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(PIN_TX, PIN_RX);
    nrfx_uarte_init(&instance, &config, uarte_handler); // uarte_handler switch with NULL
     
    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    
    nrfx_uarte_rx(&instance, uarte_buffer, sizeof(uarte_buffer));
}

void uarte_write(char* data, int length) {
    nrfx_uarte_tx(&instance, data, length, 0);
}


/*************** Other Helper functions ****************/

void initialize(){
 // Initiera GPIO för knapp
    nrf_gpio_cfg_input(BUTTON_1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON_2, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON_3, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON_4, NRF_GPIO_PIN_PULLUP);

    //Initiera RTC
    nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;
    nrfx_err_t errr = nrfx_rtc_init(&rtc_instance, &config, NULL);
    assert(errr == NRFX_SUCCESS);
    nrfx_rtc_enable(&rtc_instance);
}


void initializeRTC(void) {
    while (nrf_gpio_pin_read(BUTTON_1) == 1) {
    // Gör inget, bara vänta
    }
    // Läs av RTC-counter
    int rtc_value = nrfx_rtc_counter_get(&rtc_instance)/32.768f; //Divide to get milli sekunder
    // Initiera slumptalsgeneratorn med RTC-värdet
    srand(rtc_value);
}

int getRand(){
    int r = rand();
    return r % 100;// 0-99
}

void start_game_grupp14(void){
initialize();
initializeRTC();
init_uart();
nrfx_systick_init();

nrf_gpio_cfg_output(LED1);
nrf_gpio_cfg_output(LED2);
nrf_gpio_cfg_output(LED3);
nrf_gpio_cfg_output(LED4);

memory_game();

return 0;
}