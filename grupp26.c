#include "grupp26.h"

uint8_t uarte_enabled = 0;
volatile int running = 1;
volatile int val;
volatile int score = 0;
volatile int point = 0 ;
volatile int user_answer_ready = 0;
volatile int user_answer = 0;

void __assert_func(const char *file, int line, const char *func, const char *expr)
{
    if(uarte_enabled)
    {
        char errmsg[100];
        sprintf(errmsg, "Error in function %s, line %d. Expression: %s", func, line, expr);
        uarte_write(errmsg, strlen(errmsg));

        __asm("bkpt 1");
    }
}



nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
const nrfx_rtc_t rtc_instace = NRFX_RTC_INSTANCE(0);
char uarte_buffer;


void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)
{
    nrfx_uarte_t * p_inst = p_context;


    if (p_event->type == NRFX_UARTE_EVT_RX_DONE)
    {
        char recived = p_event->data.rx.p_data[0];
        
        if(recived == '1' || recived == '2')
        {
            user_answer = recived - '0';
            user_answer_ready = 1;
        }

        nrfx_uarte_rx(&instance, &uarte_buffer, 1);
    
    }
}   

void inituart(void)
{

    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(20, 22);
    nrfx_err_t errr = nrfx_uarte_init(&instance, &config, uarte_handler);


    if (errr == NRFX_SUCCESS)
    {
      uarte_enabled = 1;
    }

    nrfx_rtc_config_t config_rtc = NRFX_RTC_DEFAULT_CONFIG; 
    nrfx_err_t errr_rtc = nrfx_rtc_init (&rtc_instace, &config_rtc, NULL);
    nrfx_rtc_enable(&rtc_instace);
    


}



void uarte_write(char* data, int length)
{
    nrfx_uarte_tx(&instance, data, length, 0);
    while(nrfx_uarte_tx_in_progress(&instance)){};
}


void Read_String(char *data, int max_length)
{
    int index = 0;
    char received_char;


    while (index < max_length - 1)
    {
        
        nrfx_uarte_rx(&instance, &received_char, sizeof(received_char));

        if (received_char == '\r')
        {
            break;
        }


        data[index++] = received_char;
    }


    data[index] = '\0';

}


int Read_Int( void )
{
    char buffer[20];
    Read_String(buffer , sizeof(buffer));
   
   
    return  atoi(buffer);
     


}


void Send_Int(int value)
{
    char output_buffer[32];


    sprintf(output_buffer, "%d" , value);


    uarte_write(output_buffer, strlen(output_buffer));


}



void printQuestion(void)
{
    int correct = 0;
    user_answer = 0;
    user_answer_ready = 0;
   
    switch (val)
    {
        case 1:
        {
            char q[] = "1: Vad är Johans favorit knapp ?\n\r1: Delete  2: Enter\n\r";
            uarte_write(q, strlen(q));
        
            correct = 1;
            break;
        }
        case 2:
        {
            char q[] = "2: Vad äter Jonas i personalrummet?\n\r1: Cheese Cake  2: Freeze Cake\n\r";
            uarte_write(q, strlen(q));
            
            correct = 2;
            break;
        }
        case 3:
        {
            char q[] = "3: Vilket djur säger 'mjau'?\n\r1: Katt  2: Hund\n\r";
            uarte_write(q, strlen(q));
            
            correct = 1;
            break;
        }
        case 4:
        {
            char q[] = "4: Vad är huvudstaden i Sverige?\n\r1: Göteborg  2: Stockholm\n\r";
            uarte_write(q, strlen(q));

            correct = 2;
            break;
        }
        case 5:
        {
            char q[] = "5: Hur många ben har en spindel?\n\r1: 8  2: 6\n\r";
            uarte_write(q, strlen(q));

            correct = 1;
            break;
        }
        case 6:
        {
            char q[] = "6: Hur ska lill Jonas göra entre på Olles? \n\r1: Häst 2: Enhjuling\n\r";
            uarte_write(q, strlen(q));

            correct = 1;
            break;
        }
        case 7:
        {
            char q[] = "7: Vilket år är det just nu?\n\r1: 2025  2: 2023\n\r";
            uarte_write(q, strlen(q));

            correct = 1;
            break;
        }
        case 8:
        {
            char q[] = "8: Vilken färg får du om du blandar blått och gult?\n\r1: Grön  2: Lila\n\r";
            uarte_write(q, strlen(q));
            

            correct = 1;
            break;
        }
        case 9:
        {
            char q[] = "9: Vad är 9 * 9?\n\r1: 81  2: 72\n\r";
            uarte_write(q, strlen(q));

            correct = 1;
            break;
        }
        case 10:
        {
            char q[] = "10: Vem skrev 'Romeo och Julia'?\n\r1: William Shakespeare  2: August Strindberg\n\r";
            uarte_write(q, strlen(q));

            correct = 1;
            break;
        }
   
    }
    
    while (!user_answer_ready) {}

    if (user_answer == correct)
    {
        point = 1;
    }
    else
    {
        point = 0;
    }
    countScore();
    
}


void countScore(void)
{
    if (point == 1)
    {
        score ++;
    }
}


void start_game_grupp26(void)
{

    inituart();
    nrfx_systick_init();
        NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    


    nrfx_uarte_rx(&instance, &uarte_buffer, 1);

    while (running)
    {
        int sek = nrfx_rtc_counter_get(&rtc_instace)/32768;

            
        if (sek >= 20)
        {
           break;
        }
     
        int rtc_count = nrfx_rtc_counter_get(&rtc_instace)/32.768;
        srand((int)(rtc_count));  

        val = rand() %10 + 1;

        printQuestion();
             

    }
    char msg1[64];
    sprintf(msg1, "Din tid är ute, du fick %d poäng.\n\r", score);
    uarte_write(msg1, strlen(msg1));


}
