#include "grupp13.h"

nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
uint8_t uarte_enabled = 0;
const nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(0);

int matrix[YSIZE][XSIZE];                           // Playing field
int player_xpos = 0, player_ypos = 0;               
int score_xpos, score_ypos;
int score = 0;
int counter = 0;                                    // counter for resetting score_xpos and score_ypos
int cur_attempts = 0;
int pause = OFF;
int difficulty = 1;
extern char uarte_buffer = '0';
uint8_t Clear_screen[] = CLEAR_SCREEN;

//Function: Writes data to screen
void uarte_write(char* data, int length)
{
    nrfx_uarte_tx(&instance, data, length, 0);
    while(nrfx_uarte_tx_in_progress(&instance)){}
}

//Function: Generated value used for randomize seed
//Return: returns value generated from CPU clockspeed over time
int generate_srand_seed(){
    return nrfx_rtc_counter_get(&rtc_instance);
}

//Function: Generates a random number
//Return: Returns a random number
int generate_random_number(int rtc_counter_in_millisec){
    int rand_nmbr;
    srand(rtc_counter_in_millisec);
    rand_nmbr = rand();
    return rand_nmbr;
}

//Function: Takes the CPU clock counter and converts it into millisek based on the clockspeed of the CPU
//Return: Rough estimate of time passed in millisek since CPU clock started
int convert_rtc_counter_to_millisec(int rtc_counter){
    return rtc_counter/kHz;
}

//Function: loops through the matrix and adds 1#
// {    a * for player position if matrix[i][j] == PLAYERPOSITION, 
//      a * for scoreposition if matrix[i][j] == SCOREPOSITION,
//      else it adds a blank space }
// #1 to a string that is then printed out to the screen
static void printArray()
{
        char prntMatrix[1000];
        int msg_length = 0;
        msg_length = sprintf(prntMatrix, "Player : score = %d : xpos = %d : ypos = %d : score xpos = %d, ypos = %d\n\r",score,player_xpos,player_ypos,score_xpos,score_ypos);
        for (int i = 0; i < YSIZE; i++)
        {
            for (int j = 0; j < XSIZE; j++)
            {
                switch (matrix[i][j])
                {
                    case SCOREPOSITION:
                         msg_length += sprintf(prntMatrix + msg_length, "*");
                    break;
                    case PLAYERPOSITION:
                        msg_length += sprintf(prntMatrix + msg_length, "*"); 
                    break;
                    default:
                        msg_length += sprintf(prntMatrix + msg_length, " ");   
                    break;
                }
            }
            msg_length += sprintf(prntMatrix + msg_length, "|\n\r");
        }
        nrfx_uarte_tx(&instance, Clear_screen, sizeof(Clear_screen),0); // Clears the screen
        nrfx_systick_delay_ms(10);                                      // A small delay in order for the board to not freeze, has something to do with the triggering of an event.
        uarte_write(prntMatrix,strlen(prntMatrix));
        nrfx_systick_delay_ms(REFRESHRATE);                             // Delay to limit the refreshrate 
}

//Function: sets the matrix scorepos to EMPTYPOSITION and randomizes the x and y coordinates for the location of where the score is triggered.
// Then it sets the matrix scorepos to SCOREPOSITION
static void generate_score_pos(){
    matrix[score_ypos][score_xpos] = EMPTYPOSITION;
    score_xpos = generate_random_number(convert_rtc_counter_to_millisec(generate_srand_seed())) % XSIZE;
    score_ypos = generate_random_number(convert_rtc_counter_to_millisec(generate_srand_seed())) % YSIZE;
    matrix[score_ypos][score_xpos] = SCOREPOSITION;
}

//Function: Increases the score by a factor of 1 times the difficulty.
// Then it generates a new coordinate for the position of the score position
static void increase_score(){
    score += difficulty;
    generate_score_pos();
}

//Function: Initates the initial values for the game and enteres a loop.
// The loop checks for:
// 1. if the counter has reached its threshold it sets it to 0 and generates new a new coordinate for the score position
// 2. if the game has been paused or not
// 3. if the current attempts is less than the maximum amount of attempts
    // A attempts counts as not getting to the score coordinate before counter has reached REFRESHRATE / difficulty
void game_loop(){
    initiate_game();
    while (cur_attempts < MAX_ATTEMPTS)
    {

        
        // send_int(1);
        if(pause)
        {
            printArray();
            if(counter < (REFRESHRATE / difficulty))
            {  
                counter++;
            }
            else{
                counter = 0;
                cur_attempts++;
                generate_score_pos();
            }
        }
        nrfx_uarte_rx(&instance, &uarte_buffer,sizeof(uarte_buffer));
    }
    game_end_message();
}

//Function: prints the message at the end of the game
static void game_end_message()
{
    char prntMatrix[1000];
    int msg_length = 0;
    msg_length = sprintf(prntMatrix, "Player : final score = %d\n\r",score);
    nrfx_uarte_tx(&instance, Clear_screen, sizeof(Clear_screen),0); // Clears the screen
    nrfx_systick_delay_ms(10);                                      // A small delay in order for the board to not freeze, has something to do with the triggering of an event.
    uarte_write(prntMatrix,strlen(prntMatrix));
}

//Function: Initialzes the:
// 1. Matrix to 0 so that there are no random values in it
// 2. Position of the player
// 3. Position of the score coordinate.
void initiate_game()
{
    for (int i = 0; i < YSIZE; i++)
    {
        for (int j = 0; j < XSIZE; j++)
        {   
            matrix[i][j] = EMPTYPOSITION;
        }
    }
    matrix[player_ypos][player_xpos] = PLAYERPOSITION;
    generate_score_pos();
}

//Function: Event handler for handling read events
// It checks for:
//  1. if a read event has been triggered
//  1.1. if the buffer contains a 'p', it pauses the game
//  1.2. if the game is not paused it checks if the player has pressed any of the keys that changes the player position and updates the position of the player.
//  1.3. if the position of the player matches that of the score position, it increases the score and randomizes the position for the score flag
void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)
{
    nrfx_uarte_t * p_inst = p_context;
    if (p_event->type == NRFX_UARTE_EVT_RX_DONE)
    {
        if(uarte_buffer == 'p')
        {
            pause == OFF ? (pause = ON) : (pause = OFF); //Ternary operation
        }
        if (pause == OFF)
        {
            switch (uarte_buffer)
            {
            case 'i':
                if (player_ypos > 0)
                {
                    matrix[player_ypos][player_xpos]=EMPTYPOSITION;  
                    player_ypos--;
                    matrix[player_ypos][player_xpos]=PLAYERPOSITION; 
                }
                break;
            case 'k':
                if (player_ypos < YSIZE - 1)
                {    
                    matrix[player_ypos][player_xpos]=EMPTYPOSITION;  
                    player_ypos++;
                    matrix[player_ypos][player_xpos]=PLAYERPOSITION;
                }
                break;
            case 'j':
                if (player_xpos > 0)
                {
                    matrix[player_ypos][player_xpos]=EMPTYPOSITION;  
                    player_xpos--;
                    matrix[player_ypos][player_xpos]=PLAYERPOSITION; 
                }
                break;
            case 'l':
                if (player_xpos < XSIZE - 1)
                {
                    matrix[player_ypos][player_xpos]=EMPTYPOSITION;  
                    player_xpos++;
                    matrix[player_ypos][player_xpos]=PLAYERPOSITION; 
                }
                break;
            default:
                break;             
            }
            if(player_xpos == score_xpos && player_ypos == score_ypos)
            {
                increase_score();
            }  
        }

    }
}

//Function: initiates handlers for uarte, rtc, systick and enables event handling. Lastly it starts the game loop.
void start_game_grupp13(void)
{
    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(PIN_TX, PIN_RX);
    nrfx_err_t errr = nrfx_uarte_init(&instance, &config, uarte_handler);
    nrfx_rtc_config_t rtc_config = NRFX_RTC_DEFAULT_CONFIG;
    nrfx_err_t rtc_errr = nrfx_rtc_init(&rtc_instance,&rtc_config, NULL); 

    nrfx_rtc_enable(&rtc_instance);
    nrfx_systick_init();   
    
    if(errr != NRFX_SUCCESS){
        //Handle it!
    }
    else
        uarte_enabled = 1;

    game_loop();
}