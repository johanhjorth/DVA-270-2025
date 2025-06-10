
#ifndef GRUPP16_H
#define GRUPP16_H
#include <nrfx_uarte.h>
#include <nrfx_rtc.h>
#define MAX_HEALTH 4

//struktdefinition för dubbellänkad lista
typedef int Data;
typedef struct node
{
	Data data;
	char text[64];
	struct node* next;
	struct node* previous;
}Node;

typedef Node* List; //Listan representeras av en nodpekare. 

///////////////////////////////////////Externa variabler\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

extern nrfx_uarte_t localInstance;
extern nrfx_rtc_t localRTCInstance;


/////////////////////////////////////Funktion Deklarationer\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void instantInstance(nrfx_uarte_t* inst);
void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context); // call back interupt function
void randInit(nrfx_rtc_t *inst);
void printString(char text[]); //Dependencies: instantInstance
void printStringCrawl(char text[], int crawlSpeed); //Dependencies: instantInstance, nrfx_systick_init
void clearScreen(); //Dependencies: printString
void inscribeString(char target[], char input[], int insPoint); //Dependencies: <string.h>
void bootIntro(int cinematic); //Dependencies: printString, printStringCrawl, nrfx_systick_init
void interuptStringHandler(char chosenOne[], int gameState[], char screen[]);
void space(int spaces);  //Dependencies: printString
void gap(int gaps);     //Dependencies: printString

//sorts a list from smallest to largest in a bubble fashion
int bubbleSort(List *head);
//used to command a light, t toggles; d disables; e enables; s status of pin, 1 if on, 0 if off.
int comLig(int target, char command); //Dependencies: printInt

//Returnera en tom lista
List create_empty_list(void);

//Lägg till nod sist i listan
void add_last(List *list, const Data data, char string[]);

// Byter plats på noder
void switch_nodes(List *list);
//Returnera hur många noder som finns i listan
int number_of_nodes(const List list);

//Sök efter data i listan, returnera 1 om datat finns, annars 0.
int search(const List list, const Data data);

//Ta bort data ur listan (första förekomsten), returnera 0 om datat inte finns, annars 1
int remove_element(List *list, const Data data);


/*

Function to compare and then return the ammount of correct letters guessed.

Post: How many letters match is returned as a integer.
Pre: Two words of the same size. 

Args:
    userGuess -> char array 
    choosenWord -> char array
*/
int compareWords(char* userGuess, char* choosenWord);


/*
 Function to remove one point of health when the player guesses wrong.

Post: Health is negated by one and returned.
Pre: NoN

*/
void removeHealth();

/*
 Function to initialize the player with 4 health.

Post: Gives the player a initial health of 4.
Pre: NoN

*/
void initHealth();

/*
 Function to handle health status lamps and death screen.

Post: Lights fliker depending on health and if dead playes death screen.
Pre: NoN

*/
void healthCheck();


//fills list with four letter words
void easyLexicon(List *list);

//takes "pixel" coordinates and returns equivalent array slot
int coSS(int row, int column);

//clear screen but with different astetics
void wash();

//Semi randomly removes words from the deck untill the specified amount remains and returns the chosen one.
int shuffleWords(List* deck, int deckSize);

//takes the id of a word and overwrites an char array with correlating word.
char findingTheOne(List deck, int chosen, char output[]);

//fills specified span of a char array with semi randomly picked "junk" characters one at a time
void genJunk(char arr[], int pos, int length);

//views a specific span of a char array and returns a random slot number if it's suited to "seed" a word 
int wordPlow (char screen[], int seedZoneStart, int seedZoneStop, int difficulty);

//fills a array with blank spaces
void cleanArray (char array[], int size);

//fills the array that hosts the gameplay area
void loadGameScreen (char screen[], List deck, int difficulty);

//Intro splashscreen
void gameIntroGUI(char screenArray[]); //användbart att veta att av puTTYns fönster är 69 x 24 karaktärer stor


void start_game_grupp16(void);

#endif