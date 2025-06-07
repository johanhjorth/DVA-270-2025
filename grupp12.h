#ifndef GRUPP12_H
#define GRUPP12_H

#include <stdlib.h>
#include <string.h>
#include "nrfx_uarte.h"

//Ormen är uppdelad i två typer av structs. Ett SnakeHead som styr ormen och en SnakeBody för varje
//ruta som ormen är lång. 

//Styr ormen och håller koll på huvudets position, aktuel riktning, ormens längd och om ormen har krockat. 
struct SnakeHead
{
    int xCoord;
    int yCoord;

    //0 = höger, 1 = ner, 2 = vänster, 3 = upp
    int direction;

    int length;

    int alive;
};

//alla SnakeBody sparas i en array i main.c
struct SnakeBody
{
    int xCoord;
    int yCoord;

    //Bodyns plats i arrayen för ormens kropp. place = 0 är delen som är närmast huvudet. 
    int place;
};

//Hela spelbrädet är uppbygt av rutor. Dessa läggs i en array i main vid spelets start. 
struct Square
{
    //för rutanas position
    int xCoord;
    int yCoord;

    //för om rutan ska skrivas ut och med vilken färg
    //0 är tom ruta, 1 är ram, 2 är orm, 3 är äpple
    int occupier;
};

//för att hantera inputs för styrning. 
void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context);

void send_int(int number);

void sendText(char *str);

void send_breakline();

//skapar fyller en array med ett tomt spelbärde och placerar ut ramen. 
void createGameBoard(struct Square *gameBoardArray);

//byter vad som finns i en ruta. Tar arrayen med alla rutor, rutans x och y koordinat samt vilken typ rutan ska bli. 0 = tom ruta, 1 = ram, 2 = orm, 3 = äpple.
void changeOccupier(struct Square *gameBoardArray, int x, int y, int type);

//returnar vilket index en ruta i gameBoardArray har utifån dens koordinater. 
int findBoardIndex(struct Square *gameBoardArray, int x, int y);

//skapar ett äpple på en slumpmässig plats av brädet. 
void appleSpawner(struct Square *gameBoardArray);

//skapar en ny SnakeHead utifrån önskad startposition. 
struct SnakeHead createSnakeHead(int startX, int startY);

//skapar en ny SnakeBody utifrån koordinater. Sammt vilken plats den ska ha i arrayen. 
void createSnakeBody(struct SnakeBody *bodyArr, int x, int y, int place);

//byter riktning på ormen (WASD)
struct SnakeHead changeDirection(struct SnakeHead head);

//gör ormen längre när den äter ett äpple. 
struct SnakeHead eatApple(struct SnakeHead head, struct SnakeBody *body);

//flyttar ormen en ruta i nuvarande riktning. Hanterar även kollisioner. 
struct SnakeHead moveSnake(struct Square *gameBoardArray, struct SnakeHead head, struct SnakeBody *body);

//Skriver ut spelets logga
void printLogo();

//skriver ut Game over texten
void printGameOver();

//ritar ut spelplanet. 
void printGameBoard(struct Square *gameBoardArray);

void start_game_grupp12(void);

#endif
