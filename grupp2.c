#include "grupp2.h"

nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
nrfx_rtc_t rtc_instance2 = NRFX_RTC_INSTANCE(0);

int arrRand[NUMBER_OF_LEDS];//array med slumpade tal som ska slumpa leds.
int arrTemp[NUMBER_OF_LEDS];//temp array som tar användarens inmatning som sedan omvandlas till arrInput.
int arrInput[NUMBER_OF_LEDS];//Array som ska stämma överens med slumpad array.

int BEST_TIME = -1;
int tick;
int correct;
char msgTimes[50];
char msgTotalWrong[] = "Wrong in total, try again!";
char msgWrong[] = "\n\rWrong led";

static uint8_t uarte_buffer; 
volatile bool timerStop = true;

//*************************************************** */
//*************************************************** */
void uarte_rtc_error() 
{
    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(20, 22);
    nrfx_err_t errr = nrfx_uarte_init(&instance, &config, uarte_handler);//uarte_handler

    nrfx_rtc_config_t config2 = NRFX_RTC_DEFAULT_CONFIG;
    nrfx_err_t errr2 = nrfx_rtc_init(&rtc_instance2, &config2, NULL);
    nrfx_rtc_enable(&rtc_instance2);
}
void leds_stuff()
{   //LED som output
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_cfg_output(LED2);
    nrf_gpio_cfg_output(LED3); 
    nrf_gpio_cfg_output(LED4);
    //Nollar LEDs
    nrf_gpio_pin_write(LED1, LED_OFF);
    nrf_gpio_pin_write(LED2, LED_OFF);
    nrf_gpio_pin_write(LED3, LED_OFF);
    nrf_gpio_pin_write(LED4, LED_OFF);
}
void uarte_write(char* data, int length)
{
    nrfx_uarte_tx(&instance, data, length, 0);
}

//*************************************************** */
//*************************************************** */
//------------------- Interupt --------------------- 
void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)
{
    correct = 0;

    nrfx_uarte_t * p_inst = p_context;
    if(p_event -> type == NRFX_UARTE_EVT_RX_DONE)
    {
        timerStop = true; 
        uint8_t input = uarte_buffer;
        
        switch(input)
        {
            case '4': if(arrRand[NUMBER_OF_LEDS-1] == 1)
                    {
                        correct = 1;
                    }break;

            case '5': if(arrRand[NUMBER_OF_LEDS-1] == 2)
                    {
                        correct = 1;
                    }break;

            case '1':if(arrRand[NUMBER_OF_LEDS - 1] == 3)
                    {
                        correct = 1;
                    }break;

            case '2':if(arrRand[NUMBER_OF_LEDS-1] == 4)
                    {
                        correct = 1;
                    } break;

            case'\r': correct = -1; 
            break;
            
            default:
                    uarte_write(msgTotalWrong, strlen(msgTotalWrong));
                    
                    break;
        }

        if (correct == 1)
        {
            if((tick > 0 && tick <= BEST_TIME)  || BEST_TIME == -1)//Ser till att bästa tid är bästa tid
            {
                   BEST_TIME = tick;
            }

            sprintf(msgTimes, "\n\rYour time: %d\n\rBest time: %d", tick, BEST_TIME);
            uarte_write(msgTimes, strlen(msgTimes));
        }
        else if(correct == 0)
        {
            
            uarte_write(msgWrong, strlen(msgWrong)); 
        }

        nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));
    }
}

void run_timer()
{
    tick = 0;
    timerStop = false;

    nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));

    char msgTimer[100];

    while(!timerStop)
    {
        sprintf(msgTimer, "\rTid: %d ms", tick);
        uarte_write(msgTimer, strlen(msgTimer));

        while (nrfx_uarte_tx_in_progress(&instance)) {}

        nrfx_systick_delay_ms(100);
        tick ++;
    }
}
    
//*************************************************** */
//*************************************************** */
//------------------- Spelet --------------------- 
void start_game_grupp2(void);
{
    random_array();
    lit_leds_in_order();

    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));

    char msgEnter[] = "\n\rEnter the last led\n\r";
    uarte_write(msgEnter, strlen(msgEnter));

    nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));//startar första rx
    run_timer();
}

void random_array()
{
    int i;
	for(i = 0; i < NUMBER_OF_LEDS; i++)
	{
		arrRand[i] = (rand() % (NUMBER_OF_LEDS)) + 1;
	}
}

void lit_leds_in_order()
{
    for(int i = 0; i < NUMBER_OF_LEDS; i++)
    {
        if(arrRand[i] == 1) lit_led(LED1);
        if(arrRand[i] == 2) lit_led(LED2);
        if(arrRand[i] == 3) lit_led(LED3);
        if(arrRand[i] == 4) lit_led(LED4);
    }
} 

void lit_led(int pin)
{
    nrf_gpio_pin_write(pin,0);
    nrfx_systick_delay_ms(300);
    nrf_gpio_pin_write(pin,1);
    nrfx_systick_delay_ms(150);
}
