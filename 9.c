#include "9.h"

nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
nrfx_rtc_t rtc_instance2 = NRFX_RTC_INSTANCE(0);
nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(0);


int tick;
static uint8_t uarte_buffer; 
volatile bool stop_timer = true;
void uarte_rtc_error()
{
    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(20, 22);
    nrfx_err_t errr = nrfx_uarte_init(&instance, &config, uarte_handler);
    //uarte_handler

    nrfx_rtc_config_t config2 = NRFX_RTC_DEFAULT_CONFIG;
    nrfx_err_t errr2 = nrfx_rtc_init(&rtc_instance2, &config2, NULL);
    nrfx_rtc_enable(&rtc_instance2);
    nrfx_rtc_enable(&rtc_instance);
}
void led_of()
{   //LED som output
    nrf_gpio_cfg_output(LED1);

    //Nollar LEDs
    nrf_gpio_pin_write(LED1, LED_OFF);

}
void uarte_write(char* data, int length)
{
    nrfx_uarte_tx(&instance, data, length, 0);
}
void read_string(char string[])
{
    int i;
    char uarte_buffer;
    
    for(i=0; i<20; i++)
    {
        //Väntar på ett tecken över UARTE
        nrfx_uarte_rx (&instance, &uarte_buffer, sizeof(uarte_buffer));
        
        if(uarte_buffer != '\r' && i < 19)
        {
            string[i]= uarte_buffer;
        }
        else 
        {
            string[i] = '\0';
            break;
        }
    }
}
int read_int ()
{
    char conversion_int[20];
    read_string(conversion_int);
    return atoi(conversion_int);
}


void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)
{
    char tiden[50];
    nrfx_uarte_t * p_inst = p_context;
    if(p_event -> type == NRFX_UARTE_EVT_RX_DONE)
    {
        stop_timer = true; 
        uint8_t input = uarte_buffer;

            sprintf(tiden, "\n\rDin tid: %d\n\r", tick);
            uarte_write(tiden, strlen(tiden));
    
        nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));
    }
}

void run_timer()
{
    tick = 0;
    stop_timer = false;

    nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));

    char timer_msg[100];

    while(!stop_timer)
    {
        sprintf(timer_msg, "\rTid: %d milisekunder", tick);
        uarte_write(timer_msg, strlen(timer_msg));

        while (nrfx_uarte_tx_in_progress(&instance)) {}

        nrfx_systick_delay_ms(1);
        tick ++;
    }
}

void game()
{
    
    nrfx_systick_delay_ms(rand() % 10000);
    
    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    led_indicator();
    nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));
    
    run_timer();
}

void led_indicator()
{
    nrf_gpio_pin_write(LED1,0);
    nrfx_systick_delay_ms(10);
    nrf_gpio_pin_write(LED1,1);
}

void start_game_grupp9()
{
    uarte_rtc_error();
    nrfx_systick_init();
    led_of(); 
   
    nrf_gpio_cfg_input(BUTTON1, NRF_GPIO_PIN_PULLUP);

    int choice = 0;

    char msg[] ="\n\n\rKlicka på första knappen för att starta och space på tangetbordet när lampan lyser för att vinna och se din reaktions tid\n\n\r";

    do
    {
        uarte_write(msg, strlen(msg));
        do
        {
            if(nrf_gpio_pin_read(BUTTON1) == 0)
            {
                int time = nrfx_rtc_counter_get(&rtc_instance2);
                srand(time);
                game();

                break;
            } 

        }while(nrf_gpio_pin_read(BUTTON1) != 0);
    }while(1);
}

