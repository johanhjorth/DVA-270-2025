#include "grupp15.h"
#include <nrfx_systick.h>
#include <stdio.h>
#include <nrf.h>

const nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(0);

void start_game_grupp15(void)
{
    init_stuff();
    char intro_msg[] = "Press button 1 to start\n\r";
    
    nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;
    nrfx_err_t errr = nrfx_rtc_init(&rtc_instance, &config, NULL);
    nrfx_rtc_enable(&rtc_instance);
    nrf_gpio_cfg_input(BUTTON1, NRF_GPIO_PIN_PULLUP);
    
    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    static uint8_t rx_buffer;
    nrfx_uarte_rx(&uarte_instance, &rx_buffer, 1);


    uarte_write(intro_msg, strlen(intro_msg));

    while(nrf_gpio_pin_read(BUTTON1) == 1);
    
    
    srand(nrfx_rtc_counter_get(&rtc_instance));
    
    int game_array[MAX];
    int round_counter = 0;
    int seed_check = 0;

    char win_msg[] = "Correct!\n\r";
    char lose_msg[] = "Wrong! Rounds completed: ";
    
    
    start_counter();
    new_line();
    while(1)
    {
        input_index = 0;
        round_counter = pattern_handler(game_array, round_counter);
        blinking_phase(game_array, round_counter);

       

        while(input_index < round_counter);

        for(int i = 0; i < round_counter; i++)
        {
            if(player_array[i] != game_array[i])
            {
                uarte_write(lose_msg, strlen(lose_msg));
                send_int(round_counter - 1);
                new_line();
                round_counter = 0;
                start_counter();
                new_line();
                break;
            }
            else if(i == round_counter - 1 && game_array[i] == player_array[i])
            {
            
                uarte_write(win_msg, strlen(win_msg));
        
            
            }
        }
        
        
    }
   
}

nrfx_uarte_t uarte_instance = NRFX_UARTE_INSTANCE(0); 


int player_array[MAX];
int input_index = 0;
int tx_check = 0;
void uarte_write(char* data, int length)
{
    tx_check = 1;
    nrfx_uarte_tx(&uarte_instance, data, length, 0);
    while(tx_check == 1);
}

void send_int(int input_value)
{
    char buffer[30];
    sprintf(buffer, "%d", input_value);
    uarte_write(buffer, strlen(buffer));
}

void init_stuff()
{
    
   
    const nrfx_uarte_config_t uarte_config = NRFX_UARTE_DEFAULT_CONFIG(PIN_TXD, PIN_RXD);
    nrfx_err_t err = nrfx_uarte_init(&uarte_instance, &uarte_config, uarte_handler);
    
   
    nrfx_systick_init();
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_cfg_output(LED2);
    nrf_gpio_cfg_output(LED3);
    nrf_gpio_cfg_output(LED4);

    
    
    nrf_gpio_pin_set(LED1);
    nrf_gpio_pin_set(LED2);
    nrf_gpio_pin_set(LED3);
    nrf_gpio_pin_set(LED4);


   

    if (err != 0){
    }
}

int pattern_handler(int* game_array, int round_counter)
{
   
    game_array[round_counter] = rand()%4 + 1;
    return round_counter + 1;

}
void feedback_led(int led_number)
{
    nrf_gpio_pin_clear(led_number);
    nrfx_systick_delay_ms(100);
    nrf_gpio_pin_set(led_number);
    nrfx_systick_delay_ms(300);
}

void blink_led(int led_number)
{
    nrf_gpio_pin_clear(led_number);
    nrfx_systick_delay_ms(500);
    nrf_gpio_pin_set(led_number);
    nrfx_systick_delay_ms(500);
}

void blinking_phase(int* game_array, int round_counter)
{
    for (int i = 0; i < round_counter; i++) 
    {
        switch (game_array[i]) 
        {
            case 1: blink_led(LED1); break;
            case 2: blink_led(LED2); break;
            case 3: blink_led(LED3); break;
            case 4: blink_led(LED4); break;
        }
    }
}

void start_counter()
{
    char msg[] = "Game starts in: ";
    uarte_write(msg, strlen(msg));
    send_int(3);
    nrfx_systick_delay_ms(1000);
    send_int(2);
    nrfx_systick_delay_ms(1000);
    send_int(1);
    nrfx_systick_delay_ms(1000);
}

void new_line()
{
    char buffer[] = " \n\r";
    uarte_write(buffer, strlen(buffer));
}



void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)
{
    nrfx_uarte_t * p_inst = p_context;
    if (p_event->type == NRFX_UARTE_EVT_RX_DONE)
    {
        tx_check = 1;
        char input = p_event->data.rx.p_data[0];
        switch(input)
        {
            case '4':
                player_array[input_index] = 1;
                feedback_led(LED1);
                input_index++;
                break;
            case '5':
                player_array[input_index] = 2;
                feedback_led(LED2);
                input_index++;
                break;
            case '1':
                player_array[input_index] = 3;
                feedback_led(LED3);
                input_index++;
                break;
            case '2':
                player_array[input_index] = 4;
                feedback_led(LED4);
                input_index++;
                break;
            default:
                break;
        }
        static uint8_t rx_buffer;
        nrfx_uarte_rx(&uarte_instance, &rx_buffer, 1);
    }
if (p_event->type == NRFX_UARTE_EVT_TX_DONE)
    {
        tx_check = 0;
    }
}





