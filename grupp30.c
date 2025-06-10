#include "grupp30.h"                          // UARTE-funktioner


static int board[3][3];                     // Spelbrädet: 0 = tom, 1 = X, 2 = O
static int current_player = 1;              // Startar med spelare 1 (X)
static int selected_row = 0;                // Markerad rad
static int selected_col = 0;                // Markerad kolumn
char rx_char;                               // Inkommande tecken från UART

nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(PIN_TXD, PIN_RXD);

void start_game_grupp30(void)               // Startar spel
{
    uarte_init();
    init_game();
}

void uarte_init(void)                       // Initialiserar UART
{
    nrfx_uarte_init(&instance, &config, uarte_handler);
    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
	NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
}

void uarte_write(uint8_t* data, uint8_t length) // Skriver ut via UART
{
    nrfx_uarte_tx(&instance, data, length, 0);
    delay_simple();
}

void init_game(void) 
{
    for (int i = 0; i < 3; i++)              // Nollställ spelbrädet
        for (int j = 0; j < 3; j++)
            board[i][j] = 0;

    current_player = 1;                      // Starta med spelare 1
    selected_row = 0;                        // Återställ markör
    selected_col = 0;

    char msg[50];                                                                       // Instruktioner
    sprintf(msg, "\033cW = up, S = down, D = right, A = left, Space = select\r\n");
    uarte_write((uint8_t*)msg, strlen(msg));
    print_board();                           // Visa spelplanen

    nrfx_uarte_rx(&instance, (uint8_t *)&rx_char, sizeof(rx_char)); // Starta mottagning av första tangent
}

void delay_simple(void)
{
    for (volatile int i = 0; i < 200000; i++); // enkel fördröjning
}

void print_board(void)                      // Printar spelplan
{
       char row[64];
        int idx = 0;
    for (int i = 0; i < 3; i++) 
    {            // Gå igenom varje rad

        for (int j = 0; j < 3; j++) 
        {        // Gå igenom varje kolumn
            char symbol = board[i][j] == 1 ? 'X' : board[i][j] == 2 ? 'O' : '-'; // Välj symbol

            if (i == selected_row && j == selected_col) 
            {
                row[idx++] = '[';            // Markera vald ruta
                row[idx++] = symbol;
                row[idx++] = ']';
            } 
            else 
            {
                row[idx++] = ' ';            // Vanlig ruta med mellanslag
                row[idx++] = symbol;
                row[idx++] = ' ';
            }
        }
        row[idx++] = '\r';                  // Radbrytning
        row[idx++] = '\n';
    }
    uarte_write((uint8_t *)row, idx);    // Skicka raden till UART
}

int make_move(int row, int col)               // Gör ett drag
{
    if (row < 0 || row > 2 || col < 0 || col > 2)
    {
        return 0;                            // Ogiltig position
    }

    if (board[row][col] == 0) 
    {              // Om rutan är tom
        board[row][col] = current_player;    // Placera spelarens symbol
        return 1;
    }
    return 0;                                // Rutan upptagen
}

void switch_player(void)                     // Växlar spelare
{
    current_player = (current_player == 1) ? 2 : 1; // Tittar om spelaren är 1, om den är det, växla till 2. Om inte, växla till 1
}

int get_current_player(void)                 // tar den nuvarande spelaren
{
    return current_player;                   // Returnera aktiv spelare
}

int check_winner(void)                       // Tittar om det finns en vinnare
{
    for (int i = 0; i < 3; i++) 
    {
        if (board[i][0] != 0 && board[i][0] == board[i][1] && board[i][1] == board[i][2])
        {
            return board[i][0];             // Horisontell rad
        }
        if (board[0][i] != 0 && board[0][i] == board[1][i] && board[1][i] == board[2][i])
        {
            return board[0][i];             // Vertikal kolumn
        }
    }
    if (board[0][0] != 0 && board[0][0] == board[1][1] && board[1][1] == board[2][2])
    {
        return board[0][0];                 // Diagonal från övre vänster
    }
    if (board[0][2] != 0 && board[0][2] == board[1][1] && board[1][1] == board[2][0])
    {
        return board[0][2];                 // Diagonal från övre höger
    }
    return 0;                                // Ingen vinst
}

void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)   // Hanterar interrupts och uppdaterar spelplanen
{

    if (p_event->type == NRFX_UARTE_EVT_RX_DONE) 
    {
        char input = rx_char;                // Läs tecken

        if (input == 'w') 
        {
            selected_row = (selected_row + 2) % 3; // Flytta upp
        } 
        else if (input == 's') 
        {
            selected_row = (selected_row + 1) % 3; // Flytta ner
        } 
        else if (input == 'a') 
        {
            selected_col = (selected_col + 2) % 3; // Vänster
        } 
        else if (input == 'd') 
        {
            selected_col = (selected_col + 1) % 3; // Höger
        } 
        else if (input == ' ') 
        {           // Mellanslag = gör drag
            if (make_move(selected_row, selected_col)) 
            {
                int winner = check_winner();
                print_board();
                if (winner) 
                {
                    init_game();            // Starta om spelet
                    return;
                }
                switch_player();             // Nästa spelare
            } 
            else 
            {
                // Ogiltigt drag
            }
        }

        print_board();                       // Visa uppdaterat bräde
        nrfx_uarte_rx(&instance, (uint8_t*)&rx_char, 1); // Lyssna igen
    } 
    else if (p_event->type == NRFX_UARTE_EVT_ERROR) 
    {

    }
}