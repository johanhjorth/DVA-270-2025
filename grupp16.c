#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrf.h>
#include <nrf_gpio.h>
#include <nrfx_uarte.h>
#include <nrfx_systick.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <nrfx_rtc.h>
#include <stdbool.h>
#include "grupp16.h"


#define PIN_TXD 20
#define PIN_RXD 22
#define CLSCR "\033c" // Needed by clear screen

///////////////////Globala variabler\\\\\\\\\\\\\\\\\\\\\\\\\

volatile bool m_new_char_flag = false;
static uint8_t m_rx_single_char_buffer[1];
volatile static char m_char_from_isr;
int health;
char myBuffer[4];

int index = 0;

/*By JW BERG ELIAS*/

nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
nrfx_rtc_t RTCInstance = NRFX_RTC_INSTANCE(0);

void start_game_grupp16(void) {
    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(PIN_TXD, PIN_RXD);
	nrfx_err_t err_code = nrfx_uarte_init(&instance, &config, uarte_handler);
	NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
	NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
	err_code = nrfx_uarte_rx(&instance, m_rx_single_char_buffer, sizeof(m_rx_single_char_buffer));

    nrfx_rtc_config_t config_rtc = NRFX_RTC_DEFAULT_CONFIG;
	nrfx_err_t err = nrfx_rtc_init(&RTCInstance, &config_rtc, NULL);
	nrfx_rtc_enable(&RTCInstance);
	srand(nrfx_rtc_counter_get(&RTCInstance));

	nrfx_systick_init();


    initHealth();
	bootIntro(1);
	clearScreen();

	int splashDimension = 1 + coSS(17, 0);
	char splashScreen[splashDimension];
	cleanArray(splashScreen, splashDimension); 		//remember to add back, kinda important
	gameIntroGUI(splashScreen);

	char targetWordChar[4];

	int gameScreenDimension = 1 + coSS(23, 50);
	char gameScreen[gameScreenDimension];
	int gameState[1] = {0};
	while (1) {

		
		if (gameState[0] == 0) {
			char ez[] = "easy";
			char userInput[4];
			
			interuptStringHandler(ez, gameState, splashScreen);
		}
		else if (gameState[0] == 1) {
			
			clearScreen();
			
			List deck = create_empty_list();
			easyLexicon(&deck);
			int targetWord = shuffleWords(&deck, 20);
			bubbleSort(&deck);
			findingTheOne(deck, targetWord, targetWordChar);
			
			cleanArray(gameScreen, gameScreenDimension);
			loadGameScreen(gameScreen, deck, 1);
			gameState[0] = 2;
		}
		else if (gameState[0] == 2) {
			interuptStringHandler(targetWordChar, gameState, gameScreen);
			healthCheck();
		}
		
		else if (gameState[0] == 3) {
			clearScreen();
			gap(10);
			char winScreen[] = "                           You're Winner!";
			printString(winScreen);
			nrfx_systick_delay_ms(20 + rand() % 200);
			
		}
		
	}



}

///////////////////16.c\\\\\\\\\\\\\\\\\\\\\\\\\

void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context) {
    nrfx_err_t err_code_isr;
    nrfx_uarte_t *p_inst = (nrfx_uarte_t *)p_context;
    NRF_UARTE_Type *p_reg = p_inst->p_reg;
	
    if (p_event->type == NRFX_UARTE_EVT_RX_DONE) {
		// If buffer has more bytes than 0
        if (p_event->data.rx.bytes > 0) { 
			// Saves character from the rx buffer.
            m_char_from_isr = m_rx_single_char_buffer[0];
            m_new_char_flag = true;
            

          // Re arm the rx.
            err_code_isr = nrfx_uarte_rx(p_inst, m_rx_single_char_buffer, sizeof(m_rx_single_char_buffer));
            if (err_code_isr != NRFX_SUCCESS) {
    
            }
        } else {
			// Re arm the rx.
            err_code_isr = nrfx_uarte_rx(p_inst, m_rx_single_char_buffer, sizeof(m_rx_single_char_buffer));
            if (err_code_isr != NRFX_SUCCESS) { }
        }
    } else if (p_event->type == NRFX_UARTE_EVT_ERROR) {

		

       
        uint32_t error_mask_from_event = p_event->data.error.error_mask;
		// Try to clear error register to make rx capable of calling.
        p_reg->ERRORSRC = error_mask_from_event;
		// After cleaned register try to re arm rx.
        err_code_isr = nrfx_uarte_rx(p_inst, m_rx_single_char_buffer, sizeof(m_rx_single_char_buffer));
        
        if (err_code_isr != NRFX_SUCCESS) {
			
        }
    } else if (p_event->type == NRFX_UARTE_EVT_TX_DONE) {
       
    }
    
}

void interuptStringHandler(char chosenOne[], int gameState[], char screen[]) {
	nrfx_err_t err_code;

	char savedChar[4];

	if (m_new_char_flag) {
		
		myBuffer[index++] = m_char_from_isr;
		
		if (index > 4) {
			inscribeString(savedChar, myBuffer, 0);
			int likness = compareWords(myBuffer, chosenOne);
			
			
			if (likness == 4) {
				if (gameState[0] == 2) {
					gameState[0] = 3;
				}else {
					gameState[0] = 1;
				}
			} else {
				removeHealth();
				char likeChar[1];
				intToString(likeChar, likness);
				char msg[] = "/4 Correct";
				gap(1);
				inscribeString(screen, likeChar, coSS(21, 51));
				inscribeString(screen, msg, coSS(21, 52));
				char out4[] = "/4 ";
				if(screen[coSS(20, 53)] == ' ') {
					inscribeString(screen, likeChar, coSS(20, 51));
					inscribeString(screen, out4, coSS(20, 52));
					screen[coSS(20, 55)] = savedChar[0];
					screen[coSS(20, 56)] = savedChar[1];
					screen[coSS(20, 57)] = savedChar[2];
					screen[coSS(20, 58)] = savedChar[3];
					screen[coSS(20, 50)] = '>';
				}
				else if(screen[coSS(19, 53)] == ' ') {
					inscribeString(screen, likeChar, coSS(19, 51));
					inscribeString(screen, out4, coSS(19, 52));
					screen[coSS(19, 55)] = savedChar[0];
					screen[coSS(19, 56)] = savedChar[1];
					screen[coSS(19, 57)] = savedChar[2];
					screen[coSS(19, 58)] = savedChar[3];
					screen[coSS(19, 50)] = '>';
				}
				else if(screen[coSS(18, 53)] == ' ') {
					inscribeString(screen, likeChar, coSS(18, 51));
					inscribeString(screen, out4, coSS(18, 52));
					screen[coSS(18, 55)] = savedChar[0];
					screen[coSS(18, 56)] = savedChar[1];
					screen[coSS(18, 57)] = savedChar[2];
					screen[coSS(18, 58)] = savedChar[3];
					screen[coSS(18, 50)] = '>';
					char uhoh[] = "uh oh, someones";
					char uhoh2[] = " near death...";
					inscribeString(screen, uhoh, coSS(15, 51));
					inscribeString(screen, uhoh2, coSS(16, 51));
				}

				
				clearScreen(screen);
				printString(screen);
			}

			index = 0;
		}
		m_new_char_flag = false;
	}
	// Constant re-arm.
	err_code = nrfx_uarte_rx(&instance, m_rx_single_char_buffer, sizeof(m_rx_single_char_buffer));
}

void printString(char text[]) { 
	nrfx_uarte_tx(&instance, text, strlen(text), 0);
	while(nrfx_uarte_tx_in_progress(&instance)){}
}

void printStringCrawl(char text[], int crawlSpeed) {
	char temp[2];
	int i = 0;
	for (i = 0; i < strlen(text); i++) {
		temp[0] = text[i];

		if (temp[0] != ' ') {
			nrfx_systick_delay_ms(crawlSpeed);
		}
		temp[1] = '\0';
		printString(temp);
	}
}

void clearScreen() {
	char clearMsg[] = CLSCR;
	char msg[] = "\r\n\r\n\r   <================= !!! Innan clearScreen !!! =================> \r";
	//printString(msg);
	printString(clearMsg);
}

void gap(int gaps) {
	char msg[] = "\r\n";

	for (int i = 0; i < gaps; i++)
		printString(msg);
}

void space(int spaces) {
	char msg[] = " ";

	for (int i = 0; i < spaces; i++)
		printString(msg);
}

void bootIntro(int cinematic) { //det m�ste finnas n�got mindre �n en int...

	char line_1[] = "\r<<<<<<<<<<<<<<<<<<<<<<<<< (�) >>>>>>>>>>>>>>>>>>>>>>>>>\r\n";		//"konsten" �r 55 karakt�rer bred
	char line_2[] = "<<>>              _    __                __ �      <<>>\r\n";
	char line_3[] = "<<>>             | |   \\ \\      __      / /        <<>>\r\n";
	char line_4[] = "<<>>             | |    \\ \\    /  \\    / /         <<>>\r\n";
	char line_5[] = "<<>>        __   | |     \\ \\  / /\\ \\  / /          <<>>\r\n";
	char line_6[] = "<<>>        \\ \\__/ /      \\ \\/ /  \\ \\/ /           <<>>\r\n";
	char line_7[] = "<<>>         \\____/        \\__/    \\__/            <<>>\r\n";
	char line_8[] = "<<>>                                               <<>>\r\n";
	char line_9[] = "<<<<<<<<<<<<<<<<<<<<<<<<< (�) >>>>>>>>>>>>>>>>>>>>>>>>>\r\n";
	char line_10[] = "                      Productions\r\n";
	char line_11[] = "\r\n";
	char line_12[] = "           Vilseleder & Förvirrar Sen 2025\r";
	char line_13[] = "   -------------------------------------------------   \r\n";
	
	int printDelay = 250;

	clearScreen();
	
	if (cinematic == 0) {
		printString(line_1);
		printString(line_2);
		printString(line_3);
		printString(line_4);
		printString(line_5);
		printString(line_6);
		printString(line_7);
		printString(line_8);
		printString(line_9);
		printString(line_10);
		printString(line_11);
		printString(line_12);
		printString(line_13);
		
	}
	else {
		nrfx_systick_delay_ms(printDelay);
		printString(line_1);
		nrfx_systick_delay_ms(printDelay);
		printString(line_2);
		nrfx_systick_delay_ms(printDelay);
		printString(line_3);
		nrfx_systick_delay_ms(printDelay);
		printString(line_4);
		nrfx_systick_delay_ms(printDelay);
		printString(line_5);
		nrfx_systick_delay_ms(printDelay);
		printString(line_6);
		nrfx_systick_delay_ms(printDelay);
		printString(line_7);
		nrfx_systick_delay_ms(printDelay);
		printString(line_8);
		nrfx_systick_delay_ms(printDelay);
		printString(line_9);
		nrfx_systick_delay_ms(printDelay);
		printString(line_10);
		nrfx_systick_delay_ms(printDelay);
		printString(line_11);
		nrfx_systick_delay_ms((printDelay * 2));
		printStringCrawl(line_12, 100);
		nrfx_systick_delay_ms((printDelay * 4));
		printString(line_13);
	}


    //used to command target light, t toggles; d disables; e enables; r reads pin outputs, 1 if on, 0 if off.
int comLig(int target, char command) {
	target = target + 27; //just add 27 to get pin number for light

	//certifies that the targeted pin is valid
	if (target > 31 || target < 28) { 
		char err[] = "invalid pin for light: ";
		return;
	}
	

	if (command == 't') {
		nrf_gpio_cfg_output(target);
		nrf_gpio_pin_toggle(target);
		nrfx_systick_delay_ms(200);
		return;
	}
		
	else if (command == 'e') {
		nrf_gpio_cfg_output(target);
		nrf_gpio_pin_clear(target);
		return;
	}
		//nrf_gpio_pin_write(target, 1);
	else if (command == 'd') {
		nrf_gpio_cfg_output(target);
		nrf_gpio_pin_set(target);//nrf_gpio_pin_write(target, 0);
		return;
	}
	else if (command == 's') { //returns status of light, 1 if on, 0 if off
		nrf_gpio_cfg_input(target, NRF_GPIO_PIN_NOPULL);
		if (nrf_gpio_pin_read(target) == 1) {
			return 0;
		}
		else if (nrf_gpio_pin_read(target) == 0) {
			nrf_gpio_cfg_output(target); //att läsa av pinnen verkar få den att släckas om den var på
			nrf_gpio_pin_clear(target);  //därför så tänder vi lampan igen inan vi returnar ett värde.
			return 1;
		}
	}
}

int bubbleSort(List *head) {
	int i = 1;
	while (i != 0) {
		i = 0;
		List *temp = head;
		int nodeCount = number_of_nodes(*head) - 1;
		for (int k = 0; k < nodeCount; k++) {
			
			if ((*temp)->data > (*temp)->next->data) {
				switch_nodes(&(*temp));
				i++;
			}
			temp = &((*temp)->next);
		}
	}
	
}

void inscribeString(char target[], char input[], int insPoint) {

	int strEnd = insPoint + strlen(input);
	int targetLength = strlen(target);

	if (targetLength < strEnd) {
		char new[strEnd];
		for (int i = 0; i < targetLength; i++) {
			new[i] = target[i];
		}
		for (int i = targetLength; i < strEnd; i++) {
			new[i] = ' ';
		}
		new[strEnd] = '\0';
		strcpy(target, new);
	}

	for (int i = insPoint; i < strEnd; i++) {
		target[i] = input[i - insPoint];
	}
}

///////////////////Game.c\\\\\\\\\\\\\\\\\\\\\\\\\
    
void easyLexicon (List *list) {
    char lexicon[] = "hope test work nice corn west east ramp rice neat left yell blue tell talk byte shop cake bake seek quiz jank jape czar jaws caff camp zone zero wiff wimp know knob lump lock numb hemp knew next axis book bomb baby bask bake balm born bowl vice vibe verb view whig wave wolf wove wiki vows amps volt yolk";
    int wordcount = 1 + strlen(lexicon) / 5; //how many words need to be entered into the list
    
    for (int i = 0; i < wordcount; i++) {           //creates nodes and adds them to the list
        char word[WORDLENGTH_EASY];                 //empty char array created
        for (int j = 0; j < WORDLENGTH_EASY; j++)   //fills the array from the lexicon
            word[j] = lexicon[i * 5 + j];        
        
        word[WORDLENGTH_EASY] = '\0';               //caps new word
        add_last(list, i, word);                    //new node is created, i = node ID, word = node Text.
    }
}

//Semi randomly removes words from the deck untill the specified amount remains and returns the chosen one.
int shuffleWords(List* deck, int deckSize) {    //deckSize refers to how many false options are generated
    int highestNum = number_of_nodes(*deck);    //highestNum is size of the deck
    int randSpan = highestNum;                  //randSpan is highest randomly generated number, derived from biggest node ID 

    int theChosenOne = rand() % randSpan;       //this is the number that the player will be looking for, refered to as the chosen one
    while(highestNum > deckSize) {              //while loop randomly selects nodes and removes them
        int chosen = rand() % randSpan;
        if ( search(*deck, chosen) == 1 && chosen != theChosenOne) {    //if the ID referes to the chosen one or isn't in the deck it won't be removed
            remove_element(deck, chosen);                               //removes chosen element
            highestNum--;                                               //deck shrinks by 1
        }
    }
    return theChosenOne;
}


//takes the id of a word and overwrites an char array with correlating word.
char findingTheOne(List deck, int chosen, char output[]) {
	List *deckLoc = &deck;

	if ((*deckLoc)->data == chosen) {
		inscribeString(output, (*deckLoc)->text, 0);
		return;
	}
	else {
		deckLoc = &((*deckLoc)->next);
	}
}


//takes """pixel""" coordinates and returns equivalent array slot
int coSS(int row, int column) {     //window dimensions are 69 x 24
	return row * 69 + column;       
}

//fills specified span of a char array with semi randomly picked "junk" characters one at a time
void genJunk(char arr[], int pos, int length) {
	for (int i = pos; i < pos + length; i++) {  //loops untill specified span has been filled
		int ranVal = 1 + rand() % 15;           //picks a random number from 1 to 15
		switch (ranVal) {                       //array slot i is filled with a "junk" character.
			case 1: arr[i] = '('; break;
			case 2: arr[i] = ')'; break;
			case 3: arr[i] = '['; break;
			case 4: arr[i] = ']'; break;
			case 5: arr[i] = '<'; break;
			case 6: arr[i] = '>'; break;
			case 7: arr[i] = '%'; break;
			case 8: arr[i] = '&'; break;
			case 9: arr[i] = '!'; break;
			case 10: arr[i] = '@'; break;
			case 11: arr[i] = '#'; break;
			case 12: arr[i] = '/'; break;
			case 13: arr[i] = '?'; break;
			case 14: arr[i] = '{'; break;
			case 15: arr[i] = '}'; break;
		}
		if (i == pos + (length / 2) || i == pos + (length / 2) - 1)     //middle of junkspan is replaced with spaces
			arr[i] = ' ';                                               //this is just for astetics 
	}
}

//takes a char array and fills every slot with spaces
void cleanArray (char array[], int size) {
    for (int i = 0; i < size; i++) {
		array[i] = ' ';
	}
	array[size - 1] = '\0';
}

//views a specific span of a char array and returns a random slot number if it's suited to "seed" a word 
int wordPlow (char screen[], int seedZoneStart, int seedZoneStop, int difficulty) { //difficulty refers to word length

    int seedAreaSize = seedZoneStop - seedZoneStart - difficulty;   //potential seed span
    int seedSpot = seedZoneStart + (rand() % seedAreaSize);         //will try to seed this slot

    int flag = 0;                                                   //if flag is zero then slot is unsuited for seeding 
    for(int i = seedSpot - 1; i < seedSpot + difficulty + 1; i++) { //makes sure that seedSpot is appropriate by checking
                                                                    //every relevant slot
        if (screen[i] == '(') { flag = 1; }                         
        else if (screen[i] == ')') { flag = 1; }                    //if every relevant slot isn't one of these characters
        else if (screen[i] == '[') { flag = 1; }                    //then the seed candidate is inappropriate
        else if (screen[i] == ']') { flag = 1; }
        else if (screen[i] == '<') { flag = 1; }
        else if (screen[i] == '>') { flag = 1; }
        else if (screen[i] == '%') { flag = 1; }
        else if (screen[i] == '&') { flag = 1; }
        else if (screen[i] == '!') { flag = 1; }
        else if (screen[i] == '@') { flag = 1; }
        else if (screen[i] == '#') { flag = 1; }
        else if (screen[i] == '/') { flag = 1; }
        else if (screen[i] == '?') { flag = 1; }
        else if (screen[i] == '{') { flag = 1; }
        else if (screen[i] == '}') { flag = 1; }
        else if (screen[i] == ' ' && (i == seedSpot - 1 || i == seedSpot + difficulty + 1)){ flag = 1; }    //the slot before or after seeded word containing a space isn't an issue
        else {flag = 0;}                                            

        if (flag == 0) {        //if seed candidate is deemed inappropriate function exits loop and returns zero
            return 0;
        }
    }
    return seedSpot;            //position of seed Spot slot is returned if deemed worthy
}

//fills the array that hosts the gameplay area
void loadGameScreen (char screen[], List deck, int difficulty) {

    int dedicatedJunkArea = 48; //coloumns devoted to junk area
    int junkWidth = 0;          //how wide junk coloumns will be
    int difficultyEffect = 0;   //word length
    difficulty = 1; //remove if we add difficulties
    if (difficulty == 1) {      //junkWidth & difficultyEffect are subject to player selected difficulty
        junkWidth = 34;
        difficultyEffect = 4;
    }
        
    int junkClearence = (dedicatedJunkArea - junkWidth) / 2; //coloumns left and right of junk coloumn that will be blank

    for (int i = 2; i < 22; i++) {                           //two central junk pillars are generated
        genJunk(screen, coSS(i, junkClearence), junkWidth);  //20 rows tall, with width subject to difficulty
    }                                                        //higher difficulty = wider pillars 
    
    int seededSlots = 0;                    //amount of words seeded to junk pillars
    int deckSize = number_of_nodes(deck);   //number of seeds to distribute
    List *deckLoc = &deck;					//pointer to our deck, used to go navigate the deck harmlessly
    int i = 2;								
    while (seededSlots < deckSize) {		//loops through pillars row by row and adds words from the deck untill every word has been used
        int tempSeedbed = 0;				//contains seed or rejection
        tempSeedbed = wordPlow(screen, coSS(i, junkClearence), coSS(i, junkClearence + junkWidth), difficultyEffect);
        			//wordPlow attempts to seed a word slot, if sucessful returns slot number, otherwise 0
        if (tempSeedbed != 0) {			
            inscribeString(screen, (*deckLoc)->text, tempSeedbed);  //word is inscrbed into a junk pillar
            deckLoc = &((*deckLoc)->next);							//deck pointer points to next word in the deck
            seededSlots++;											
        }
        else {
            i++;
        }
        if (i > 22) {	//if while loop reaches the bottom row of the pillars it will return
            i = 2;		//to the top row and keep trying to seed untill every word has been seeded
        }
    }

	for (int i = 1; i < 24; i++) {
		int coloumn = 48;
		char colPart = '|';
		screen[coSS(i, coloumn)] = colPart;
		screen[coSS(i, coloumn + 1)] = colPart;
	}

	char roof[] = "============JW-Model-Rob-Co-Terminal=================================";
	char textBoxRoof[] = "===================";  //19 coloumns long, giving 18 coloumns of printing space
	char bottom[] = "================================================||";
	char disclamerA[] = "Please Prefix";
	char disclamerB[] = "Input With A >";
	inscribeString(screen, roof, coSS(0, 0));
	inscribeString(screen, textBoxRoof, coSS(22, 50));
	inscribeString(screen, bottom, coSS(23, 0));
	inscribeString(screen, disclamerA, coSS(1, 52));
	inscribeString(screen, disclamerB, coSS(2, 52));
    printString(screen);
}

//just like clear screen, just used for different astetics
void wash() {
    char wash[] = "\033c";
    char line[] = "\r\n=====================================================================";
    
    gap(5);
    printString(line);
    printString(wash);
}

//Intro splashscreen
void gameIntroGUI(char screenArray[]) {
	int speedDown = 150;
    int dramaticDelay = 500;
    int speedSide = 100;

	//nöjd över att jag inte behöver ha kvar det här meddelandet.
    //char "          OM DU LIDER AV GROV EPILEPSI \r\n          TITTA BORT I 7 SEKUNDER";

	//figures used to build the splash screen
	char line_1[] = "=====================================================================";
	char line_2[] = "         _    __                __                                   ";
	char line_3[] = "        | |   \\ \\      __      / /                                   ";
	char line_4[] = "        | |    \\ \\    /  \\    / /   Productions & Rob Co Industries  ";
	char line_5[] = "   __   | |     \\ \\  / /\\ \\  / /                                     ";
	char line_6[] = "   \\ \\__/ /      \\ \\/ /  \\ \\/ /                 Present              ";
	char line_7[] = "    \\____/        \\__/    \\__/                                       ";
	char line_8[] = "                                                                     ";
	char line_09[] = "    _      _        __          _____    _   __  ||                  ";
	char line_10[] = "   | |    | |      /  \\        / ____|  | | / /  ||                  ";
	char line_11[] = "   | |    | |     / /\\ \\      / /       | |/ /   ||                  ";
	char line_12[] = "   | |____| |    / /__\\ \\    | |        |   |    ||                  ";
	char line_13[] = "   |  ____  |   / ______ \\   | |        | |\\ \\   ||                  ";
	char line_14[] = "   | |    | |  / /      \\ \\   \\ \\____   | | \\ \\  ||                  ";
	char line_15[] = "   |_|    |_| /_/        \\_\\   \\_____|  |_|  \\_\\ ||                  ";
	char line_16[] = "                                                 ||                 ";
    char line_16S[] = "                                                 || "; //its supposed to be shorter

	/////////////////////////////////////////////////logo roll, writes to screen array from top to bottom

    wash();												//moves previous text of the screen
	inscribeString(screenArray, line_2, coSS(1, 0));	//inscribes data from line_X into the screen array
	printString(screenArray);							//prints current version of screenArray to putty
    nrfx_systick_delay_ms(speedDown);

    wash();
	inscribeString(screenArray, line_3, coSS(2, 0));
	printString(screenArray);
    nrfx_systick_delay_ms(speedDown);

    wash();
	inscribeString(screenArray, line_4, coSS(3, 0));
	printString(screenArray);
    nrfx_systick_delay_ms(speedDown);

    wash();
	inscribeString(screenArray, line_5, coSS(4, 0));
	printString(screenArray);
    nrfx_systick_delay_ms(speedDown);

    wash();
	inscribeString(screenArray, line_6, coSS(5, 0));
	printString(screenArray);
    nrfx_systick_delay_ms(speedDown);

    wash();
	inscribeString(screenArray, line_7, coSS(6, 0));
	printString(screenArray);
    nrfx_systick_delay_ms(speedDown);

    wash();
	inscribeString(screenArray, line_8, coSS(7, 0));
    printString(screenArray);
    nrfx_systick_delay_ms(speedDown);

    nrfx_systick_delay_ms(dramaticDelay);
	///////////////////////////////////////side roll, writes to screen array from left to right
	for (int i = 0; i < 68; i++) {
        screenArray[coSS(0, i)] = line_1[i];		//data from list_X coloumn i is inscribed to screen array 
		screenArray[coSS(8, i)] = line_1[i];
		screenArray[coSS(9, i)] = line_16[i];
		screenArray[coSS(10, i)] = line_09[i];
		screenArray[coSS(11, i)] = line_10[i];
		screenArray[coSS(12, i)] = line_11[i];
		screenArray[coSS(13, i)] = line_12[i];
		screenArray[coSS(14, i)] = line_13[i];
		screenArray[coSS(15, i)] = line_14[i];
		screenArray[coSS(16, i)] = line_15[i];
		screenArray[coSS(17, i)] = line_16S[i];
		
        if (i == 13 || i == 27 || i == 37 || i == 47) { //splash screen is uppdated when a letter has been transcribed
            wash();
            printString(screenArray);
            nrfx_systick_delay_ms(speedSide);
        }
	}
    wash();
    printString(screenArray);
	//////////////////////////////Options

	char greetingA[] = "  Please Enter   ";
	char greetingB[] = "   Difficulty    ";
	char greetingC[] = "   Easy Easy";
	char greetingD[] = "   Easy Easy";
	char greetingE[] = ">";

	char invalidA[] = "    error!   ";
	char invalidB[] = "Invalid Input";

	int coloumn = 51;
	inscribeString(screenArray, greetingA, coSS(9, coloumn));		//text is transcribed
	inscribeString(screenArray, greetingB, coSS(10, coloumn));
	inscribeString(screenArray, greetingC, coSS(12, coloumn));
	inscribeString(screenArray, greetingD, coSS(13, coloumn));
	inscribeString(screenArray, greetingE, coSS(17, coloumn));
    wash();
	printString(screenArray);
}

///////////////////GameLogic.c\\\\\\\\\\\\\\\\\\\\\\\\\

// Compares words and return how similar they are.
int compareWords(char* userGuess, char* choosenWord) {
    int userSize = strlen(userGuess);
    int choosenSize = strlen(choosenWord);
    int Likness = 0;
    for (int i = 0; i < userSize; i++) {
        for (int j = 0; j < choosenSize; j++) {
            if (userGuess[i] == choosenWord[j]) {
                userGuess[i] = '0';
                Likness = Likness + 1;
                break;
            }
        }
    }
return Likness;
}

// removes one health.
void removeHealth() {
    if (health <= 0) {
        return;
    }
    health = health - 1;

}

// gets the current health.
int gethealth() {
    return health;
}

// Initializes the health.
void initHealth() {
    health = MAX_HEALTH;
}

// Handles the health lights and death.
void healthCheck() {
    switch (health) {
        case 0: comLig(1, 'd'); comLig(2, 'd'); comLig(3, 'd'); comLig(4, 'd'); break;
		case 1:	comLig(1, 't'); comLig(2, 'd'); comLig(3, 'd'); comLig(4, 'd'); break;
		case 2:	comLig(1, 't'); comLig(2, 't'); comLig(3, 'd'); comLig(4, 'd'); break;
		case 3:	comLig(1, 't'); comLig(2, 't'); comLig(3, 't'); comLig(4, 'd'); break;
		case 4:	comLig(1, 't'); comLig(2, 't'); comLig(3, 't'); comLig(4, 't'); break;
	}

    if (health == 0) {
        clearScreen();
		gap(10);
		space(23);
		char b[] = "You are dead lel";
		printString(b);
        nrfx_systick_delay_ms(20 + rand() % 200);
    }
}

///////////////////List.c\\\\\\\\\\\\\\\\\\\\\\\\\


//Returnera en tom lista - funktionen är färdig
List create_empty_list(void)
{
    return NULL;
}

static Node* create_list_node(const Data data, char string[])
{
	Node* newNode = malloc(sizeof(Node));
	
	if (newNode == NULL) {
		return NULL;
	}

	newNode->data = data;
	strcpy(newNode->text, string);
	//newNode->text = string;
	newNode->next = NULL;
	newNode->previous = NULL;



	return newNode;

	//glöm inte att kolla så att malloc inte returnerade NULL
}

//lägg till nod sist i listan
void add_last(List *list, const Data data, char string[])
{
	List newList = create_list_node(data, string);
	if (*list == NULL) {
		*list = newList;
	}
	else if ((*list)->next == NULL) {
		(newList)->previous = *list;
		(*list)->next = newList;
	}
	else {
		add_last(&(*list)->next, data, string);
	}
	
}

void switch_nodes(List *list){

    if ((*list)->next != NULL) {
        List temp1 = *list;
        *list = (*list)->next;
        List temp2 = (*list)->next;
        (*list)->next = temp1;
        temp1->next = temp2;
    }
}

int number_of_nodes(const List list)
{
	if (list->next == NULL)  {
		return 1;
	}

	int number = number_of_nodes(list->next);	//gets return from previous iterations. 
	return number + 1; 							//return grows by one for every iteration.

}

//Sök efter data i listan, returnera 1 om datat finns, annars 0.
int search(const List list, const Data data)
{
	if (list->data == data) {
		return 1;
	}
	else if (list->next == NULL) {
		return 0;
	}
	search(list->next, data);
	
}

//Ta bort data ur listan (första förekomsten), returnera 0 om datat inte finns, annars 1
int remove_element(List *list, const Data data)
{
	List temp = *list;
	if (search(temp, data) == 0) {
		return 0;
	}
	if ((*list)->next->data != data) {
		remove_element(&(*list)->next, data);
	}
	else {
		remove_first(&(*list)->next);
	}


	return 1;


}