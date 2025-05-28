#include "grupp8.h"

#define GRID_WIDTH 10
#define GRID_HEIGHT 10

#define PIN_TXD 20
#define PIN_RXD 22

char buffer1[64];
char buffer[64];
char buffer2[100];

static nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
static char rx_char;
char uarte_buffer[1];

static int player_x = 0, player_y = 0;
static bool coins[GRID_HEIGHT][GRID_WIDTH] = {0};
static int score = 0;
static int seconds_left = 150;
static bool quit = false;


static void place_random_coin(void)
{
    int x_rand, y_rand;
    do {
        x_rand = rand() % GRID_WIDTH;
        y_rand = rand() % GRID_HEIGHT;
    } while (coins[y_rand][x_rand] || (x_rand == player_x && y_rand == player_y));

    coins[y_rand][x_rand] = true;
}

void init_game(void)
{
    score = 0;
    quit = false;
    player_x = 0;
    player_y = 0;
    seconds_left = 150;

    memset(coins, 0, sizeof(coins));
    for (int i = 0; i < 3; i++) place_random_coin();
}

void process_input(char c)
{
    if (score >= 10 || quit) return;

    switch (c) {
        case 'w': if (player_y > 0) player_y--; break;
        case 's': if (player_y < GRID_HEIGHT - 1) player_y++; break;
        case 'a': if (player_x > 0) player_x--; break;
        case 'd': if (player_x < GRID_WIDTH - 1) player_x++; break;
        case 'v': quit = true; return;
    }

    if (coins[player_y][player_x]) {
        coins[player_y][player_x] = false;
        score++;
        if (score < 10) place_random_coin();
        else quit = true;
    }

    should_redraw = true;
}

void draw_grid(uint32_t time_left)
{
    char grid_buffer[400];
    int offset = 0;

    offset += sprintf(&grid_buffer[offset], "\033[2J\033[H");

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (y == player_y && x == player_x)
                offset += sprintf(&grid_buffer[offset], "X ");
            else if (coins[y][x])
                offset += sprintf(&grid_buffer[offset], "o ");
            else
                offset += sprintf(&grid_buffer[offset], ". ");
        }
        offset += sprintf(&grid_buffer[offset], "\n\r");
    }

    offset += sprintf(&grid_buffer[offset], "X: %d, Y: %d, Poäng: %d\n\r", player_x, player_y, score);
    offset += sprintf(&grid_buffer[offset], "Tid kvar: %lu Enheter\n\r", time_left);

    uarte_write(grid_buffer, offset);
}

bool game_should_quit(void)
{
    return quit;
}

int get_score(void)
{
    return score;
}

static void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)
{
    if (p_event->type == NRFX_UARTE_EVT_RX_DONE) {
        rx_char = p_event->data.rx.p_data[0];
        new_data = true;
        process_input(rx_char);
    }

    nrfx_uarte_rx(&instance, uarte_buffer, sizeof(uarte_buffer));
}

void init_uart(void)
{
    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(PIN_TXD, PIN_RXD);
    nrfx_uarte_init(&instance, &config, uarte_handler);

    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));

    nrfx_uarte_rx(&instance, uarte_buffer, sizeof(uarte_buffer));
}

void uarte_write(char* data, int length)
{
    nrfx_uarte_tx(&instance, data, length, 0);
}

bool has_new_uart_input(void)
{
    return new_data;
}

char get_uart_char(void)
{
    new_data = false;
    return rx_char;
}

void begin_prints(void)
{
    draw_grid(seconds_left); // Första ritning

    while (seconds_left > 0 && !quit)
    {
        nrfx_systick_delay_ms(100);
        seconds_left--;

        if (should_redraw) {
            draw_grid(seconds_left);
            should_redraw = false;
        }
    }

    nrfx_systick_delay_ms(500);

    if (score >= 10)
        sprintf(buffer, "\n\rDu vann! Grattis!\n\r");
    else
        sprintf(buffer, "\n\rTiden är ute! Du förlorade din snigel!\n\r");

    uarte_write(buffer, strlen(buffer));
}


void start_game_grupp8(void)
{
    init_uart();
    init_game();
    nrfx_systick_init();

    sprintf(buffer2, "\n\rTryck w/a/s/d för att flytta. v för att avsluta. OBS: Spelet är tidsbaserat!\n\r");
    uarte_write(buffer2, strlen(buffer2));
    
    begin_prints();

}