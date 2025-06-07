#include <grupp3.h>

static nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
static const nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(0);

uint8_t buffer;
int up = 0;
int side = 0;


uint32_t ReadInt(nrfx_uarte_t *instance, uint8_t num[]){

    int i = 0;

    do{
        nrfx_uarte_rx(instance, &buffer, sizeof(buffer));
        num[i] = buffer;
        i+=1;
    } while(num[i-1] != '\r');
    num[i-1] = '\0';


    uint32_t Result = atoi(num);

    return Result; 
}

//initierar grejer samt kört start_game()

void start_game_grupp3() {
    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));

    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(20, 22);
    nrfx_systick_init();

    nrfx_err_t errr = nrfx_uarte_init(&instance, &config, &UarteHandler);
    if (errr != NRFX_SUCCESS){
        // Lägg till felhantering här om du vill
        while (1);
    }

    nrfx_uarte_rx(&instance, &buffer, sizeof(buffer));

    uint8_t msg[] = "Welcome to THE Game!\n\rUse WASD to move. Press q to quit.\n\r";
    nrfx_uarte_tx(&instance, msg, sizeof(msg), 0);
    while(nrfx_uarte_tx_in_progress(&instance));
    StartGame();
}


void StartGame() {
    uint8_t clear_screen[] = CLEAR_SCREEN;
    char grid[10][10];
    char new_line[] = "\n\r";
    nrfx_rtc_enable(&rtc_instance);

    srand(nrfx_rtc_counter_get(&rtc_instance));

    int x = rand() % 10;
    int y = rand() % 10;
    uint8_t check = 1;

    while (check) {
        nrfx_uarte_rx(&instance, &buffer, sizeof(buffer));

        // Clear screen
        nrfx_uarte_tx(&instance, clear_screen, strlen((char*)clear_screen), 0);
        while(nrfx_uarte_tx_in_progress(&instance));

        // Reset grid
        for (int i = 0; i < 10; i++) {
            memset(grid[i], '*', 10);
        }

        grid[up][side] = 'X';
        grid[y][x] = 'C';

        for (int z = 0; z < 10; z++) {
            char line[11];
            memcpy(line, grid[z], 10);
            line[10] = '\0';

            nrfx_uarte_tx(&instance, (uint8_t*)line, strlen(line), 0);
            while(nrfx_uarte_tx_in_progress(&instance));

            nrfx_uarte_tx(&instance, (uint8_t*)new_line, strlen(new_line), 0);
            while(nrfx_uarte_tx_in_progress(&instance));
        }

        if (up == y && side == x) {
            uint8_t msg[] = "You Won!\n\r";
            nrfx_uarte_tx(&instance, msg, strlen((char*)msg), 0);
            while(nrfx_uarte_tx_in_progress(&instance));
            check = 0;
        }
    }
}


void UarteHandler(nrfx_uarte_event_t const *p_event, void *p_context)
{
    if (p_event->type == NRFX_UARTE_EVT_RX_DONE) {
        switch(buffer) {
            case 'w': up = (up == 0) ? 9 : up - 1; break;
            case 'a': side = (side == 0) ? 9 : side - 1; break;
            case 's': up = (up + 1) % 10; break;
            case 'd': side = (side + 1) % 10; break;
            case 'q': abort();
        }
        // Restart RX
        nrfx_uarte_rx(&instance, &buffer, sizeof(buffer));
    }
}