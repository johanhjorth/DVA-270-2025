nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
nrfx_rtc_t rtc_instance2 = NRFX_RTC_INSTANCE(0);

int ticks;
static uint8_t uarte_buffer; 
volatile bool stop_timer = true;

void game()
{
    led();
    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));
    timer();
}

void led_of()
{
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_pin_write(LED1, LED_OFF);
}

void uarte_write(char* data, int length)
{
    nrfx_uarte_tx(&instance, data, length, 0);
}

void read_string(char InputString[20])
{
	int i;
	for(i = 0; i<20; i++)
    {
        nrfx_uarte_rx (&instance, &uarte_buffer, sizeof(uarte_buffer));

		if(uarte_buffer != '\r')
        {
			InputString[i] = uarte_buffer;
		}
		else
        {			
		    break;	
		}
    }

    if(i == 20)
    {
        InputString[i-1] = '\0';
    }
	else 
    {
        InputString[i] = '\0';
    }
	nrfx_uarte_tx(&instance, InputString, strlen(InputString), 0);
}

void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)
{
    char too_fast_msg[20]="\n\rtoo fast\n\r", lose_msg[20]="\n\rtoo slow\n\r";
    nrfx_uarte_t * p_inst = p_context;
    if(p_event -> type == NRFX_UARTE_EVT_RX_DONE)
    {
        stop_timer = true; 
        uint8_t input = uarte_buffer;

        if (ticks>=200)
        {
            uarte_write(lose_msg, strlen(lose_msg));
        }
        if(ticks==0)
        {
            uarte_write(too_fast_msg, strlen(too_fast_msg));
        }

        nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));
    }
}

void timer()
{
    ticks = 0;
    stop_timer = false;

    nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));

    char timer_msg[100];

    while(!stop_timer)
    {
        sprintf(timer_msg, "\rtime: %d ms, points: %d", ticks, ticks);
        uarte_write(timer_msg, strlen(timer_msg));

        while (nrfx_uarte_tx_in_progress(&instance)) {}

        nrfx_systick_delay_ms(10);
        ticks++;
    }
}

void led()
{
    nrfx_systick_delay_ms(1000);
    nrf_gpio_pin_write(LED1,0);
    nrfx_systick_delay_ms(300);
    nrf_gpio_pin_write(LED1,1);
}