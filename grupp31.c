#include <31.h>

// Globala instanser
static nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);
static const nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(0);

// Globala variabler
uint8_t buffer;
int up = 0;
int side = 0;
volatile bool quit = false; // Flagga för att avsluta spelet

uint32_t read_int(nrfx_uarte_t *instance, uint8_t num[]) {
    int i = 0;

    do {
        nrfx_uarte_rx(instance, &buffer, sizeof(buffer));
        num[i] = buffer;
        i += 1;
    } while (num[i - 1] != '\r');
    num[i - 1] = '\0';

    return atoi((char *)num);
}

void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context) {
    if (p_event->type == NRFX_UARTE_EVT_RX_DONE) {
        if (buffer == 'q') {
            quit = true;
        }

        // Starta RX igen
        nrfx_uarte_rx(&instance, &buffer, sizeof(buffer));
    }
}

void start_game(); {
    uint8_t clear_screen[] = CLEAR_SCREEN;
    char grid[10][10];
    char new_line[] = "\n\r";

    uint8_t x = rand() % 10;
    uint8_t y = rand() % 10;
    bool running = true;

    // Nollställ grid
    for (int i = 0; i < 10; i++) {
        memset(grid[i], '*', 10);
    }

    grid[up][side] = 'x';
    grid[y][x] = 'o';

    // Skriv ut första griden
    nrfx_uarte_tx(&instance, clear_screen, strlen((char *)clear_screen), 0);
    while (nrfx_uarte_tx_in_progress(&instance));

    for (int z = 0; z < 10; z++) {
        char line[11];
        memcpy(line, grid[z], 10);
        line[10] = '\0';

        nrfx_uarte_tx(&instance, (uint8_t *)line, strlen(line), 0);
        while (nrfx_uarte_tx_in_progress(&instance));

        nrfx_uarte_tx(&instance, (uint8_t *)new_line, strlen(new_line), 0);
        while (nrfx_uarte_tx_in_progress(&instance));
    }

    // Spelloopen
    while (running && !quit) {
        nrfx_uarte_rx(&instance, &buffer, sizeof(buffer));

        // Ta bort föregående position
        grid[up][side] = '*';

        switch (buffer) {
            case 'w': up = (up == 0) ? 9 : up - 1; break;
            case 'a': side = (side == 0) ? 9 : side - 1; break;
            case 's': up = (up + 1) % 10; break;
            case 'd': side = (side + 1) % 10; break;
            default: continue; // hoppa över om ogiltig tangent
        }

        grid[up][side] = 'x';

        // Skriv ut ny grid
        nrfx_uarte_tx(&instance, clear_screen, strlen((char *)clear_screen), 0);
        while (nrfx_uarte_tx_in_progress(&instance));

        for (int z = 0; z < 10; z++) {
            char line[11];
            memcpy(line, grid[z], 10);
            line[10] = '\0';

            nrfx_uarte_tx(&instance, (uint8_t *)line, strlen(line), 0);
            while (nrfx_uarte_tx_in_progress(&instance));

            nrfx_uarte_tx(&instance, (uint8_t *)new_line, strlen(new_line), 0);
            while (nrfx_uarte_tx_in_progress(&instance));
        }

        // Kontrollera vinst
        if (up == y && side == x) {
            uint8_t msg[] = "You Won!\n\r";
            nrfx_uarte_tx(&instance, msg, strlen((char *)msg), 0);
            while (nrfx_uarte_tx_in_progress(&instance));
            running = false;
        }
    }

    if (quit) {
        uint8_t msg[] = "Game aborted.\n\r";
        nrfx_uarte_tx(&instance, msg, strlen((char *)msg), 0);
        while (nrfx_uarte_tx_in_progress(&instance));
    }
}


void start_game_grupp31() {
    NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
    NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));

    const nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(20, 22); // Anpassa pinnar

    nrfx_systick_init();

    nrfx_err_t errr = nrfx_uarte_init(&instance, &config, uarte_handler);
    if (errr != NRFX_SUCCESS) {
        while (1); // Felhantering
    }

    nrfx_uarte_rx(&instance, &buffer, sizeof(buffer));

    uint8_t msg[] = "Welcome to Apple Game!\n\rUse WASD to move. Press q to quit.\n\r";
    nrfx_uarte_tx(&instance, msg, sizeof(msg), 0);
    while (nrfx_uarte_tx_in_progress(&instance));

    start_game();
}
