

nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
nrfx_rtc_t rtc_instance2 = NRFX_RTC_INSTANCE(0);

#define MAX_LEN 10
int tick;
static uint8_t uarte_buffer; 
volatile bool stop_timer = true;


void uarte_rtc_error()
{
    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(20, 22);
    nrfx_err_t errr = nrfx_uarte_init(&instance, &config, uarte_handler);//uarte_handler

    nrfx_rtc_config_t config2 = NRFX_RTC_DEFAULT_CONFIG;
    nrfx_err_t errr2 = nrfx_rtc_init(&rtc_instance2, &config2, NULL);
    nrfx_rtc_enable(&rtc_instance2);
}


void led_of()
{   //LED output
    nrf_gpio_cfg_output(LED1);

    //Nollar LED
    nrf_gpio_pin_write(LED1, LED_OFF);

}




void uarte_write( char *data, int length)
{
    nrfx_uarte_tx(&instance, (const uint8_t *)data, length, 0);
}


int uarte_read_line(char *buffer, int max_len)
{
    int index = 0;

    while (index < max_len - 1)
    {
        nrfx_err_t err_code = nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));
        if (err_code != NRFX_SUCCESS)
        {
            break;
        }

        if (uarte_buffer == '\r' || uarte_buffer == '\n')
        {
            break;
        }
        buffer[index++] = (char)uarte_buffer;
    }
    buffer[index] = '\0';
    return index;
}


// läser av nummer 
int read_int(void)
{
    char buffer[MAX_LEN];
    uarte_read_line(buffer, MAX_LEN);
    return atoi(buffer);
}


void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)
{
    char time_messege[50];
    nrfx_uarte_t * p_inst = p_context;
    if(p_event -> type == NRFX_UARTE_EVT_RX_DONE)
    {
        stop_timer = true; 
        uint8_t input = uarte_buffer;


            sprintf(time_messege, "\n\rYour time: %d\n\r", tick);
            uarte_write(time_messege, strlen(time_messege));
    
        

        nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));
    }
}


// Är för timern för se hur snabbt man klikar
void run_timer()
{
    tick = 0; 
    stop_timer = false;

    nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));

    char timer_messege[100];

    while(!stop_timer)
    {
        sprintf(timer_messege, "\rTid: %d ms", tick);
        uarte_write(timer_messege, strlen(timer_messege));

        while (nrfx_uarte_tx_in_progress(&instance)) {}

        nrfx_systick_delay_ms(100);
        tick ++;
    }
}

// spelet aktiverar lampa och sen timern för se hur lång tid det tar
void start_game_grupp7()
{
    nrfx_systick_delay_ms(500);
    led();
    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));

    nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));
    run_timer();
}


void led()
{
    nrf_gpio_pin_write(LED1,0);
    nrfx_systick_delay_ms(300);
    nrf_gpio_pin_write(LED1,1);
    nrfx_systick_delay_ms(150);
}