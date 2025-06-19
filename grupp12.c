#include <stdio.h>
#include <stdint.h>
#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrf.h>
#include <nrf_gpio.h>
//#include <nrfx_uarte.h>
#include <nrfx_systick.h>
#include <nrfx_rtc.h>
#include "nrfx_uarte.h"
#include "grupp12.h"

//hur stort spelbrädet ska vara
#define BOARD_WIDTH (22) //22
#define BOARD_HEIGHT (13) //13

#define UPDATE_TIME (250)

//För grafik
#define CC_DEFAUTL_BACKGROUND_COLOR "\x1B[49m"
#define CC_DARK_GREY_BACKGROUND "\x1B[100m"
#define CC_LIGHT_BLUE_BACKGROUND "\x1B[104m"
#define CC_LIGHT_GREEN_BACKGROUND "\x1B[102m"
#define CC_LIGHT_RED_BACKGROUND "\x1B[101m"

#define CC_LIGHT_RED "\x1B[91m"
#define CC_LIGHT_GREEN "\x1B[92m"
#define CC_LIGHT_WHITE "\x1B[97m"


nrfx_rtc_t rtcInstance = NRFX_RTC_INSTANCE(0);

static char uarte_buffer_interupt;

int currentApples = 0;

int eatedIt = 0;


void send_int(int number)
{
    char str[50];

    sprintf(str, "%d", number);

    nrfx_uarte_tx(&instance, str, strlen(str), 0);
    while(nrfx_uarte_tx_in_progress(&instance)){};
}

void sendText(char *str)
{
    int length = 0;

    while(*(str + length) != 0) length++;

    char buffer[256];
    sprintf(buffer,"%s", str);

    nrfx_uarte_tx(&instance, (uint8_t *)buffer, length, 0);
    while(nrfx_uarte_tx_in_progress(&instance)){};
}

void send_breakline()
{
    uint8_t Ugp_breakline[] = "\n\r";
    nrfx_uarte_tx(&instance, Ugp_breakline, sizeof(Ugp_breakline), 0);
    while(nrfx_uarte_tx_in_progress(&instance)){};
}

void createGameBoard(struct Square *gameBoardArray)
{
    int i = 0;

    for(int y = 0; y < BOARD_HEIGHT; y++)
    {
        for(int x = 0; x < BOARD_WIDTH; x++)
        {
            gameBoardArray[i].xCoord = x;
            gameBoardArray[i].yCoord = y;

            //för att skapa "ramen" runt spel brädet. 
            if(y == 0 || x == 0 || y == BOARD_HEIGHT - 1 || x == BOARD_WIDTH - 1)
                gameBoardArray[i].occupier = 1;
            else
                gameBoardArray[i].occupier = 0;

            i++;
        }
    }
}

int findBoardIndex(struct Square *gameBoardArray, int x, int y)
{
    for(int i = 0; i < BOARD_HEIGHT * BOARD_WIDTH; i++)
    {
        if(gameBoardArray[i].xCoord == x && gameBoardArray[i].yCoord == y)
            return i;
    }
     
}

void changeOccupier(struct Square *gameBoardArray, int x, int y, int type)
{
    int i = findBoardIndex(gameBoardArray, x, y);
    switch(type)
    {
    //tom ruta
    case 0:
        gameBoardArray[i].occupier = 0;
        break;
    //ram
    case 1:
        gameBoardArray[i].occupier = 1;
        break;
    //orm
    case 2:
        gameBoardArray[i].occupier = 2;
        break;
    //äpple 
    case 3:
        gameBoardArray[i].occupier = 3;
        break;
    }
}

void appleSpawner(struct Square *gameBoardArray)
{    
    while(1)
    {
        int boardArraySize = BOARD_HEIGHT * BOARD_WIDTH;
        int i = rand() % boardArraySize;

        if(gameBoardArray[i].occupier == 0)
        {
            //spawan äpple
            changeOccupier(gameBoardArray, gameBoardArray[i].xCoord, gameBoardArray[i].yCoord, 3);
            return;
        }
    }
}

static struct SnakeHead checkCollision(struct Square *gameBoardArray, struct SnakeHead head, struct SnakeBody *body)
{
    switch(head.direction)
    {
        //höger
        case 0:
            //krock
            if(gameBoardArray[findBoardIndex(gameBoardArray, head.xCoord + 1, head.yCoord)].occupier == 1 || gameBoardArray[findBoardIndex(gameBoardArray, head.xCoord + 1, head.yCoord)].occupier == 2)
                head.alive = 0;
            //äta äpple
            else if(gameBoardArray[findBoardIndex(gameBoardArray, head.xCoord + 1, head.yCoord)].occupier == 3)
            {
                head = eatApple(head, body);
                eatedIt = 1;
            }
            break;
        //ner
        case 1:
            //krock
            if(gameBoardArray[findBoardIndex(gameBoardArray, head.xCoord, head.yCoord + 1)].occupier == 1 || gameBoardArray[findBoardIndex(gameBoardArray, head.xCoord, head.yCoord + 1)].occupier == 2)
                head.alive = 0;
            //äta äpple
            else if(gameBoardArray[findBoardIndex(gameBoardArray, head.xCoord, head.yCoord + 1)].occupier == 3)
            {
                head = eatApple(head, body);
                eatedIt = 1;
            }
            break;
        //vänster
        case 2:
            //krock
            if(gameBoardArray[findBoardIndex(gameBoardArray, head.xCoord - 1, head.yCoord)].occupier == 1 || gameBoardArray[findBoardIndex(gameBoardArray, head.xCoord - 1, head.yCoord)].occupier == 2)
                head.alive = 0;
            //äta äpple
            else if(gameBoardArray[findBoardIndex(gameBoardArray, head.xCoord - 1, head.yCoord)].occupier == 3)
            {
                head = eatApple(head, body);
                eatedIt = 1;
            }
            break;
        //upp
        case 3:
            //krock
            if(gameBoardArray[findBoardIndex(gameBoardArray, head.xCoord, head.yCoord - 1)].occupier == 1 || gameBoardArray[findBoardIndex(gameBoardArray, head.xCoord, head.yCoord - 1)].occupier == 2)
                head.alive = 0;
            //äta äpple
            else if(gameBoardArray[findBoardIndex(gameBoardArray, head.xCoord, head.yCoord - 1)].occupier == 3)
            {
                head = eatApple(head, body);
                eatedIt = 1;
            }
            break;
    }
    return head;
}

static struct SnakeHead moveBody(struct Square *gameBoardArray, struct SnakeHead head, struct SnakeBody *body)
{
    for(int i = head.length - 1; i >= 0; i--)
    {
        if(body[i].place == head.length - 1)
        {
            changeOccupier(gameBoardArray, body[i].xCoord, body[i].yCoord, 0);
        }

        if(body[i].place == 0)
        {
            body[i].xCoord = head.xCoord;
            body[i].yCoord = head.yCoord;
        }
        else if(i == (head.length - 1) && eatedIt == 1)
        {
            eatedIt = 0;
        }
        else
        {
            body[i].xCoord = body[i - 1].xCoord;
            body[i].yCoord = body[i - 1].yCoord;
        }         
    }

    return head;
}

static struct SnakeHead moveHead(struct Square *gameBoardArray, struct SnakeHead head)
{
    switch(head.direction)
    {
        //höger
        case 0:
        
            if(head.length < 1)
                changeOccupier(gameBoardArray, head.xCoord, head.yCoord, 0);
            head.xCoord++;
            changeOccupier(gameBoardArray, head.xCoord, head.yCoord, 2);    
            break;
        //ner
        case 1:
            if(head.length < 1)
                changeOccupier(gameBoardArray, head.xCoord, head.yCoord, 0);
            head.yCoord++;
            changeOccupier(gameBoardArray, head.xCoord, head.yCoord, 2);
            break;
        //vänster
        case 2:
            if(head.length < 1)
                changeOccupier(gameBoardArray, head.xCoord, head.yCoord, 0);
            head.xCoord--;
            changeOccupier(gameBoardArray, head.xCoord, head.yCoord, 2);
            break;
        //upp
        case 3:
            if(head.length < 1)
                changeOccupier(gameBoardArray, head.xCoord, head.yCoord, 0);
            head.yCoord--;
            changeOccupier(gameBoardArray, head.xCoord, head.yCoord, 2);
            break;
    }
    return head;
}

struct SnakeHead createSnakeHead(int startX, int startY)
{
    struct SnakeHead head;
    head.xCoord = startX;
    head.yCoord = startY;

    head.direction = 0;

    head.length = 0;    
    head.alive = 1;

    return head;
}

void createSnakeBody(struct SnakeBody *bodyArr, int x, int y, int place)
{
    struct SnakeBody body;
    body.xCoord = x;
    body.yCoord = y;

    body.place = place;

    bodyArr[place] = body;
}

struct SnakeHead changeDirection(struct SnakeHead head)
{
    switch(uarte_buffer_interupt)
    {
        case 'w':
            if(head.direction != 1)
                head.direction = 3;
            break;
        case 's':
            if(head.direction != 3)
                head.direction = 1;
            break;
        case 'a':
            if(head.direction != 0)
                head.direction = 2;
            break;
        case 'd':
            if(head.direction != 2)
                head.direction = 0;
            break;
    }
    return head;
}

struct SnakeHead eatApple(struct SnakeHead head, struct SnakeBody *body)
{
    createSnakeBody(body, body[head.length - 1].xCoord, body[head.length - 1].yCoord, head.length);
    head.length++;
    currentApples--;
    return head;
}

struct SnakeHead moveSnake(struct Square *gameBoardArray, struct SnakeHead head, struct SnakeBody *body)
{
    head = checkCollision(gameBoardArray, head, body);

    if(head.alive == 1)
    {
        //flytta kroppen
        if(head.length > 0)
        {
            head = moveBody(gameBoardArray, head, body);
        }
        
        //flyttar huvudet.
        head = moveHead(gameBoardArray, head);
    }  
    
    return head;
}

void printLogo()
{
    sendText(CC_LIGHT_RED "                _  __");
    send_breakline();
    sendText(CC_LIGHT_GREEN " ___ _ __   ___");
    sendText(CC_LIGHT_RED "| |/ /");
    send_breakline();
    sendText(CC_LIGHT_GREEN "/ __| '_ \\ / _ \\");
    sendText(CC_LIGHT_RED " ' / ");
    send_breakline();
    sendText(CC_LIGHT_GREEN "\\__ \\ | | |  __/");
    sendText(CC_LIGHT_RED " . \\ ");
    send_breakline();
    sendText(CC_LIGHT_GREEN "|___/_| |_|\\___" );
    sendText(CC_LIGHT_RED "|_|\\_\\" CC_LIGHT_WHITE);
    send_breakline();
}

void printGameOver()
{
    sendText(CC_LIGHT_RED"  ____    _    __  __ _____    _____     _______ ____  _ ");
    send_breakline();
    sendText(" / ___|  / \\  |  \\/  | ____|  / _ \\ \\   / / ____|  _ \\| |");
    send_breakline();
    sendText("| |  _  / _ \\ | |\\/| |  _|   | | | \\ \\ / /|  _| | |_) | |");
    send_breakline();
    sendText("| |_| |/ ___ \\| |  | | |___  | |_| |\\ V / | |___|  _ <|_|");
    send_breakline();
    sendText(" \\____/_/   \\_\\_|  |_|_____|  \\___/  \\_/  |_____|_| \\_(_)"CC_LIGHT_WHITE);
    send_breakline();
}

void printGameBoard(struct Square *gameBoardArray)
{
    uint8_t background[] = CC_DARK_GREY_BACKGROUND "  " CC_DEFAUTL_BACKGROUND_COLOR;
    uint8_t border[] = CC_LIGHT_BLUE_BACKGROUND "  " CC_DEFAUTL_BACKGROUND_COLOR;
    uint8_t snake[] = CC_LIGHT_GREEN_BACKGROUND "  " CC_DEFAUTL_BACKGROUND_COLOR;
    uint8_t apple[] = CC_LIGHT_RED_BACKGROUND "  " CC_DEFAUTL_BACKGROUND_COLOR;
    

    for(int i = 0; i < BOARD_HEIGHT * BOARD_WIDTH; i++)
    {
        switch(gameBoardArray[i].occupier)
        {
            //tom ruta - svart
            case 0:
                sendText(background);
                break;

            //ram - vit
            case 1: 
                sendText(border);
                break;

            //orm - grön
            case 2:
                sendText(snake);
                break;

            //äpple - röd
            case 3:
                sendText(apple);
                break;
        }
    
        if(i % BOARD_WIDTH == BOARD_WIDTH - 1)
            send_breakline();
    }
}


void start_game_grupp12(void);
{
    //NRFX startup
    nrfx_rtc_config_t config_rtc = NRFX_RTC_DEFAULT_CONFIG;
    nrfx_err_t errrr =nrfx_rtc_init(&rtcInstance, &config_rtc, NULL);
    nrfx_rtc_enable(&rtcInstance);

    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));

    nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(20, 22);
    config.baudrate = NRF_UARTE_BAUDRATE_1000000;
    nrfx_err_t errr = nrfx_uarte_init(&instance, &config, uarte_handler);
    if (errr != 0)
    {
        // Do something... perchance?
    }

    nrfx_systick_init();
        
    nrfx_uarte_rx(&instance, &uarte_buffer_interupt, 1);

    int playAgain = 1;
    int highScore = 0;
    
    //spellogik
    while(1)
    {
        if(playAgain == 1)
        {
            //Variabel defineitioner och skapa arrayer
            int maxApples = 1;

            int startPosX = BOARD_WIDTH/2 - 4;
            int startPosY = BOARD_HEIGHT/2;

            uint8_t Clear_screen[] = CLEAR_SCREEN;
            nrfx_uarte_tx(&instance, Clear_screen, sizeof(Clear_screen), 0);

            struct Square gameBoardArray[BOARD_WIDTH * BOARD_HEIGHT];
            createGameBoard(gameBoardArray);

            struct SnakeHead snakeHead;
            snakeHead = createSnakeHead(startPosX, startPosY);
            changeOccupier(gameBoardArray, snakeHead.xCoord, snakeHead.yCoord, 2);

            struct SnakeBody snakeBody[BOARD_WIDTH * BOARD_HEIGHT - (2 * BOARD_WIDTH + 2 * BOARD_WIDTH) - 4];

            //skapar ormens kropp
            createSnakeBody(snakeBody, startPosX - 1, startPosY, 0);
            snakeHead.length = 1;
            changeOccupier(gameBoardArray, startPosX - 1, startPosY, 2);
            createSnakeBody(snakeBody, startPosX - 2, startPosY, 1);
            snakeHead.length = 2;
            changeOccupier(gameBoardArray, startPosX - 2, startPosY, 2);

            //spel logiken. 
            while (snakeHead.alive == 1)
            {
                nrfx_uarte_rx(&instance, &uarte_buffer_interupt, 1);

                nrfx_uarte_tx(&instance, Clear_screen, sizeof(Clear_screen), 0);

                printLogo();

                sendText("Score: ");
                send_int(snakeHead.length - 2);
                sendText("    Current highscore: ");
                send_int(highScore);
                send_breakline();

                snakeHead = changeDirection(snakeHead);

                snakeHead = moveSnake(gameBoardArray, snakeHead, snakeBody);
                
                if(currentApples < maxApples)
                {
                    srand(nrfx_rtc_counter_get(&rtcInstance));
                    appleSpawner(gameBoardArray);
                    currentApples++;
                }            

                printGameBoard(gameBoardArray);

                nrfx_systick_delay_ms(UPDATE_TIME);
            }

            playAgain = 0;
            printGameOver();    
            send_breakline();
            sendText("Score: ");
            send_int(snakeHead.length - 2);
            sendText("    High Score: ");
            if(highScore < (snakeHead.length - 2))
                highScore = (snakeHead.length - 2);
            send_int(highScore);
            send_breakline();
            sendText("Press 'r' to play again");
            
        }
        nrfx_uarte_rx(&instance, &uarte_buffer_interupt, 1);
        if(uarte_buffer_interupt == 'r')
        {
            playAgain = 1;
            currentApples = 0;
        }
    }
}