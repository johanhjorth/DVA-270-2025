#include "grupp4.h"
#include <nrfx_rtc.h> 

const nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(0);
nrfx_uarte_t uarte_instance = NRFX_UARTE_INSTANCE(0); 
int uarte_enabled = 0;
volatile bool tx_done = false; //Volatile gör att den kan hanteras utanför normala flödet
static uint8_t rx_byte; 

int snake, numgame; // Används för att se vilket spel man är i
int run = 0; // Används för att pausa

// Variabler snake
int play_direction = UP_KEY; 
int player_posXY[2] = {MTX_ROWS-1, 2};

// Variabler nummerfångaren
int target;
int num_showing;
int num_game_win = 0;
int num_game_fail = 0;

static void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context);

static void init_NRF()
{
    uint32_t irq = NRFX_IRQ_NUMBER_GET(uarte_instance.p_reg);
    NVIC_ClearPendingIRQ(irq);
    NVIC_EnableIRQ(irq);
    nrfx_systick_init();
    nrfx_rtc_enable(&rtc_instance);
    
    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(PIN_TXD, PIN_RXD);

    nrfx_err_t errr = nrfx_uarte_init(&uarte_instance, &config, uarte_handler);
    if (errr != 0) uarte_enabled = 1;   
    else uarte_enabled = 0;
}

static void print(const char *msg) 
{
    char msgprint[200];
    tx_done = false;
    sprintf(msgprint, "%s", msg);
    nrfx_uarte_tx(&uarte_instance, (unsigned char *)msgprint, strlen(msgprint), 0);   
    while(!tx_done) 
    {
        __WFE();  // Vänta på event – energisnålt -- ChatGPTlösning för att whilelösning från uppgift ibland hamnade i loop
    }
}

static void print_num(const char *msg, int inputnum) 
{
    char msgprint[200];
    sprintf(msgprint, "%s %d\n\r", msg, inputnum);
    print(msgprint);
}

static void snake_handler()
{
    if ((play_direction != UP_KEY && rx_byte == DOWN_KEY) || (play_direction != DOWN_KEY && rx_byte == UP_KEY) || (play_direction != LEFT_KEY && rx_byte == RIGHT_KEY) || (play_direction != RIGHT_KEY && rx_byte == LEFT_KEY)) play_direction = rx_byte;
}

static void numgame_handler()
{
    if (target== num_showing)
    {
        num_game_win = 1;
        num_showing = -1;
    }
    else num_game_fail = 1;
}

static void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context) //Går in i denna varje gång kommunikation sker till eller från Putty
{
    nrfx_uarte_t * p_inst = p_context;
    if (p_event->type == NRFX_UARTE_EVT_RX_DONE) //Om händelsetyp är av typen Recieve transmission
    {
        //Kollar vad som sker utifrån våra globala variabler
        if (rx_byte == 27)
        {
            snake = 0;      
            numgame = 0;
        }
        if (!run) run = 1;
        else if (snake) snake_handler();
        else if (numgame) numgame_handler();

    }
    rx_byte = 0;
    nrfx_uarte_rx(&uarte_instance, &rx_byte, 1);
    if (p_event->type == NRFX_UARTE_EVT_TX_DONE) {
        tx_done = true;
    }
}

static void print_matrix(int **matrix, int pos_xy[2])
{
    print(CLEAR_SCREEN);
    for (int i = 0; i < MTX_ROWS; i++)
    {
        for (int j = 0; j < MTX_COLS; j++)
        {
            char output[20];
            sprintf(output, matrix[i][j] ? "\033[32m█\033[0m" : " ");
            print(output);
        }
        print("\n\r");
    }
}

static void print_player(int **matrix, int pos_xy[2])
{
    char output[30];
    sprintf(output, "\033[%d;%dH\033[95m█\033[%d;%dH", pos_xy[0], pos_xy[1], pos_xy[0], pos_xy[1]);
    print(output);
}

static void start_screen()
{
    print(CLEAR_SCREEN "Tryck på en tangent för att starta\n\r"); 
    print("  \033[92m█████\r\n"); 
    print(" █\033[97m██\033[92m█\033[97m██\033[92m██\r\n"); 
    print(" █ \033[97m█\033[92m█ \033[97m█\033[92m███\r\n"); 
    print("  ████████\r\n"); 
    print(" █████████\r\n"); 
    print("██████████\r\n"); 
    print("██████\033[97m█\033[92m███\r\n"); 
    print(" \033[92m█\033[97m████\033[92m████\r\n"); 
    print("  \033[92m███████\r\n"); 
    print("  \033[32m██\033[92m████\r\n"); 
    print(" \033[32m███\033[92m███    \033[32m██\033[92m██\033[32m█\r\n"); 
    print(" ██\033[92m███   \033[32m█\033[92m██\033[32m█\033[92m██\033[32m██\r\n"); 
    print(" ██\033[92m█████\033[32m██\033[92m█████\033[32m█\033[92m██      \033[92m█\033[32m█\033[92m██\033[32m██\r\n"); 
    print(" ██\033[92m█████\033[32m█\033[92m█████████\033[32m█"); 
    print("    \033[92m█\033[32m██\033[92m████\033[32m█\r\n"); 
    print(" \033[32m███\033[92m██████████████\033[32m██  \033[32m█\033[92m█\033[32m█\033[92m███████\r\n"); 
    print("  \033[32m██\033[92m████████   ███\033[32m█\033[92m██\033[32m██\033[92m███    ██\r\n"); 
    print("  \033[32m███\033[92m█████      █████\033[32m█\033[92m███\r\n"); 
    print("   \033[32m███\033[92m██         ███████\r\n"); 
    print("                   ███\r\n"); 
}

void start_game_grupp4(void)
{ 
    init_NRF();

    int **display_matrix = malloc(MTX_ROWS * sizeof(int *));
    for (int i = 0; i < MTX_ROWS; i++)
    {
        display_matrix[i] = malloc(MTX_COLS * sizeof(int));
        for (int j = 0; j < MTX_COLS; j++)
        {
            display_matrix[i][j] = (i == 0 || i == MTX_ROWS-1 || j == 0 || j == MTX_COLS - 1)? 1 : 0;
        }
    }
    start_screen();

    while (!run){}
    srand(nrf_systick_val_get());

    // ##################################################               SNAKE             ############################################################################
    nrfx_systick_delay_ms(100);
    print_matrix(display_matrix, player_posXY);

    int timer = nrfx_rtc_counter_get(&rtc_instance);
    int length = 1;
    snake = 1;
    while (snake)
    {   
        display_matrix[player_posXY[0]][player_posXY[1]] = 1;
        switch (play_direction)
        {
        case UP_KEY:
            player_posXY[0]--;
            break;
        case DOWN_KEY:
            player_posXY[0]++;
            break;
        case LEFT_KEY:
            player_posXY[1]--;
            break;
        case RIGHT_KEY:
            player_posXY[1]++;
            break;  
        }
        nrfx_systick_delay_ms(100);
        print_player(display_matrix, player_posXY);
        length++;
        if ((display_matrix[player_posXY[0]][player_posXY[1]] == 1 && player_posXY[0] != MTX_ROWS-1 && player_posXY[1] != MTX_COLS-1) || player_posXY[0] == 1 || player_posXY[1] == 1 || player_posXY[0] == MTX_ROWS || player_posXY[1] == MTX_COLS)
        {
            snake = 0;
            print("\033[0m\033[21;0HFAIL!!!!\n");
        }
    }
    timer = (nrfx_rtc_counter_get(&rtc_instance) - timer)/32.768;
    print_num("\rTid (millisek): ", timer);
    print_num("\n\rLängd: ", length);
    nrfx_systick_delay_ms(1000); //Skydd mot att man trycker förbi när man vill styra masken

    print("\nTryck på en tangent för att gå vidare till nästa spel"); 
    run = 0;
    while (!run){}

    // ##################################################           Nummerfångaren             ############################################################################
    numgame = 1;
    target = rand()%10;
    {
        char msgprint[50];
        sprintf(msgprint, CLEAR_SCREEN "Tryck på en tangent när nummer \033[92m%d\033[37m visas (escape för att avsluta) \n\r",  target);
        print(msgprint);
    }

    while (numgame)
    {
        for (int i = 0; i < 100; i++) // Loop för att få snabbare respons på om man gjort rätt eller fel (annars skulle den vara efter tiden gått ut)
        {
            nrfx_systick_delay_ms(10);
            if (num_game_win)
            {
                print("Bra jobbat!\n\r");
                num_game_win = 0;
            } 
            else if (num_game_fail)
            {
                print("Fail!\n\r");
                num_game_fail = 0;
            }
        }
        num_showing = rand()%10;
        char msgprint[10];
        sprintf(msgprint, "\r%d    ",  num_showing);
        print(msgprint);
    }
    print(CLEAR_SCREEN "Hej då!");
    nrfx_systick_delay_ms(1000);
}