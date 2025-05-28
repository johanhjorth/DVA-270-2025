#include "grupp18.h"

int answer;
int state=0;
int now=0;
char temp[100];
uint8_t uarte_buffer;
nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(0);

void uarte_write(char* data, int length)
{
    nrfx_uarte_tx(&instance, data, length, 0);
    while(nrfx_uarte_tx_in_progress(&instance)){};
}

void send_int(int converted_string,char string[])
{
    sprintf(string,"%d",converted_string);
}

int read_int(char input_string[])
{
int converted_string;
converted_string=atoi(input_string);
return converted_string;
}

void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)//sparar inputs tills enter och interuptar genom att ändra state och answer
{
    nrfx_uarte_t * p_inst = p_context;
    
        if (p_event->type == NRFX_UARTE_EVT_RX_DONE)
        {
            if(uarte_buffer=='\r')
            {
                temp[now]='\0';
                answer=read_int(temp);
                state=1;
                now=0;
            }
            else
            {
                temp[now]=uarte_buffer;
                now++;
            }
            nrfx_uarte_rx (&instance, &uarte_buffer, sizeof(uarte_buffer));
        }
        
  
}


void charthers(int choice)//använd för att printa tecken mellan två funktioner
{
    switch(choice)
    {
        case 1:
        char addition[]="+";
        uarte_write(addition,sizeof(addition));
        break; 
        case 2:
        char subtraction[]="-";
        uarte_write(subtraction,sizeof(subtraction));
        break;
        case 3:
       char multiplication[]= "X";
        uarte_write(multiplication,sizeof(multiplication));
        break;  
        
    }
    
    
}
int function_addition(int diff,int time)//genererar random sum uttryck
{
    srand(time);
    int var1,var2;
    char var1_char[3],var2_char[3];
    char parantes1[]="(";
    char parantes2[]=")";
    char addition[]="+";
    var1=rand()%diff+1;
    var2=rand()%diff+1;
    send_int(var1,var1_char);
    send_int(var2,var2_char);

    uarte_write(parantes1,sizeof(parantes1));
    uarte_write(var1_char,sizeof(var1_char));
    uarte_write(addition,sizeof(addition));
    uarte_write(var2_char,sizeof(var2_char));
    uarte_write(parantes2,sizeof(parantes2));

    return var1+var2;
}

int function_subtraction(int diff,int time)//genererar random substracktions uttryck
{
    srand(time);
    int var1,var2;
    char var1_char[3],var2_char[3];
    char parantes1[]="(";
    char parantes2[]=")";
    char subtraction[]="-";
    var1=rand()%diff+1;
    var2=rand()%diff+1; 
    send_int(var1,var1_char);
    send_int(var2,var2_char);

    uarte_write(parantes1,sizeof(parantes1));
    uarte_write(var1_char,sizeof(var1_char));
    uarte_write(subtraction,sizeof(subtraction));
    uarte_write(var2_char,sizeof(var2_char));
    uarte_write(parantes2,sizeof(parantes2));

    return var1-var2;
}

int function_multiplication(int diff,int time)//genererar random multiplications uttryck
{
    srand(time);
    int var1,var2;
    char var1_char[3],var2_char[3];
    char parantes1[]="(";
    char parantes2[]=")";
    char multiplication[]="X";
    var1=rand()%diff+1;
    var2=rand()%diff+1;
    send_int(var1,var1_char);
    send_int(var2,var2_char);

    uarte_write(parantes1,sizeof(parantes1));
    uarte_write(var1_char,sizeof(var1_char));
    uarte_write(multiplication,sizeof(multiplication));
    uarte_write(var2_char,sizeof(var2_char));
    uarte_write(parantes2,sizeof(parantes2));

    return var1*var2;
}

int countdown(int seconds,int level, int facit)// kollar om varet är rätt och räknar ner tiden
{
    int timer;
    char lose_time[]="Your Time has run out, Game Over";
    char lose_answer[]="Wrong Answer, Game Over";
    for(timer=seconds;timer>-1;timer--)
    {
        if(state==1)
        {
            state=0;
            if(answer==facit)
            {
                return level+1;
            }
            else
            {
                uarte_write(lose_answer,sizeof(lose_answer));
                return 0;
            }
        }
        if(timer==0)
        {
            uarte_write(lose_time,sizeof(lose_time));
            return 0;
        }

        nrfx_systick_delay_ms(1000);
    } 
}

void start_game_grupp18(void)
{
    int facit,timer,level,game_on,start,time,next,restart;
    nrfx_systick_init();
    nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(20, 22);//aktiverar uarte och interrupt
    nrfx_err_t errr = nrfx_uarte_init(&instance, &config, uarte_handler);
    nrfx_rtc_config_t config2 = NRFX_RTC_DEFAULT_CONFIG;
    nrfx_err_t errr_2 = nrfx_rtc_init(&rtc_instance, &config2,NULL);
    nrfx_rtc_enable(&rtc_instance);
    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    char enter[]="\n\r";
    char intro[]="welcome to THE MATH Game";
    char win[]="congrats, You WIN";
    char next_level[]="next level!";
    char continue_[]="enter 1 to continue";
    char start_[]="enter 1 to start the Game";
    char restart_[]="Press 0 to restart";
    char clear_screen[]="\033c";
    nrfx_uarte_rx (&instance, &uarte_buffer, sizeof(uarte_buffer));//aktiverar interrupt genom att söka knapptryck
    while(1)
    {
        restart=1;
        level=1;
        start=1;
        game_on=1;
        next=1;
        uarte_write(clear_screen,sizeof(clear_screen));
        uarte_write(intro,sizeof(intro));
        uarte_write(enter,sizeof(enter));
        uarte_write(start_,sizeof(start_));
        uarte_write(enter,sizeof(enter));
        while(start==1)//håller dig vid start skärm
        {
            if(state==1)
            {
                if(answer==1)
                {
                start=0;
                state=0;
                }
            }
        }
        
        while(game_on==1)
        {
            
            if(level>0 && level<8)//kollar om man har förlorat eller vunnit
            {
                next=1;
                uarte_write(clear_screen,sizeof(clear_screen));
                uarte_write(continue_,sizeof(continue_));
                uarte_write(enter,sizeof(enter));
                while(next==1)//håller en kvar tills knapptryck samtidigt som en intern timer
                {
                    time=nrfx_rtc_counter_get(&rtc_instance)/32.768;
                
                    if(state==1)
                    {
                        if(answer==1)
                        {
                            uarte_write(clear_screen,sizeof(clear_screen));
                            next=0;
                        }
                        state=0;
                    }
                }
            }
            switch(level)//innehåller lose/win screen och alla levels
            {
                case 0:
                uarte_write(enter,sizeof(enter));
                uarte_write(restart_,sizeof(restart_));
                uarte_write(enter,sizeof(enter));
                while(restart==1)
                {
                     if(answer==0)
                    {
                        restart=0;
                        game_on=0;
                    }
                    state=0;
                }

                
                break;
                case 1:
                        facit=function_addition(20,time);
                        uarte_write(enter,sizeof(enter));
                        level=countdown(10,level,facit);//tidcountdown nivå och facit

                break;
                case 2:
                        facit=function_subtraction(20,time);
                        uarte_write(enter,sizeof(enter));
                        level=countdown(10,level,facit);
                break;
                
                case 3: 
                        facit=function_multiplication(10,time);
                        uarte_write(enter,sizeof(enter));
                        level=countdown(10,level,facit);
                break;
                case 4:
                        facit=function_addition(20,time);
                        charthers(1);
                        
                        facit=facit+function_subtraction(20,time+1000);
                        uarte_write(enter,sizeof(enter));
                        level=countdown(20,level,facit);
                break;
                case 5:
                        facit=function_addition(20,time);
                        charthers(2);
                        
                        facit=facit-function_subtraction(20,time+1000);
                        uarte_write(enter,sizeof(enter));
                        level=countdown(20,level,facit);
                break;
                case 6:
                        facit=function_addition(20,time);
                        charthers(3);
                        
                        facit=facit*function_subtraction(20,time+1000);
                        uarte_write(enter,sizeof(enter));
                        level=countdown(30,level,facit);
                break;
                case 7:
                        facit=function_addition(20,time);
                        charthers(1);
                        
                        facit=facit+function_multiplication(10,time+1000);
                        uarte_write(enter,sizeof(enter));
                        level=countdown(30,level,facit);
                break;
                case 8:
                        uarte_write(win,sizeof(win));
                        level=0;
                    
                break;
            }
        }
    }
}