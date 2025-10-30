#include "grupp21.h"
#include "list.h"
#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrf.h>
#include <nrfx_uarte.h>
#include <nrf_gpio.h>
#include <nrfx_systick.h>
#include <stdlib.h>
#include <string.h>

#include <nrfx_gpiote.h>
#include <nrfx_glue.h>
#include <nrfx_rtc.h>
#include <stdio.h>
//#include "BStree.h"

#include <nrfx_rtc.h>
#include <math.h>


#define PIN_TXD 20
#define PIN_RXD 22

#define LED1 28
#define LED2 29
#define LED3 30
#define LED4 31

#define BUTTON1 23
#define BUTTON2 24
#define BUTTON3 8
#define BUTTON4 9

#define OFF 1
#define ON 0


#define OFF 1
#define ON 0

#define MOLE_1 1
#define MOLE_2 2
#define MOLE_3 3
#define MOLE_4 4
    

char Points_add[] = "+";
char Points_curr[] = "Points: ";    
char win[] = "gg";
char hit[] = "HIT!";

#define CLEAR_SCREEN "\033c"

nrfx_uarte_t instance2 = NRFX_UARTE_INSTANCE(0);
nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(0);



void uarte_write(char* data, int length)
{
    nrfx_uarte_tx(&instance2, data, length, 0);
    while(nrfx_uarte_tx_in_progress(&instance2)){
    };
}  

void read_string(char *buffer, int max_len) {
    uint8_t ch;
    int i = 0;

    while (i < max_len - 1) {
        nrfx_uarte_rx(&instance2, &ch, 1);

        if (ch == '\r' || ch == 13) {
            break;
        }

        buffer[i++] = (char)ch;
    }

    buffer[i] = '\0';
}

int read_int() {
    char buffer[50];
    read_string(buffer, sizeof(buffer));
    return atoi(buffer);
}

void send_int(int tal) {


    char buffer[50];
    

    sprintf(buffer, " %d ", tal);

    uarte_write(buffer, strlen(buffer));
}

void print_n(){
    char n[] = "\n\r";
    uarte_write(n, strlen(n));
}

int Rand_int(int range){
    return rand()%range;
}
//storlek på array och intervallet på slump talen.

int* Fill_array(int size, int range){
    int* arr = malloc(size * sizeof(int));
    if (!arr) return NULL;

    for (int i = 0; i < size; i++){
        arr[i] = Rand_int(range);
    }
    return arr;
}

int is_sorted_arr(int arr[], int size){
    int i=0;
    for(i = 0; i<sizeof(arr); i++){
        if (arr[i]>arr[i+1]){
            return 0;
        }
    }

    return 1;
}

void Fill_list(int size, int range, List *list){
    for (int i = 0; i < size; i++){
        add_first(&(*list), Rand_int(range));
    }
}

int is_sorted_list(List *list){
    if ((*list)->data > (*list)->next->data && (*list)->next != NULL){
        return 0;
    }
    if((*list)->next == NULL){
        return 1;
    }
    is_sorted_list(&((*list)->next));
}



void Switch_Nodes(List *list, Data data){

    if (search(*list, data) == 0){
        char NF[] = "Data not found!";
        uarte_write(NF, strlen(NF));
        return;
    }
    if (is_empty(*list) == 1){
        return;
    }
    if (number_of_nodes(*list)<1){
        return;
    }
    if ((*list)->data == data){
        (*list)->next = (*list)->next->next;
        (*list)->next->next = *list;
    }

    Node *before = *list;
    Node *current = *list;
    while (current->data != data && current->next != NULL){
        before = current;
        current = (current)->next;
        
    }
    Node *after = current->next;

    current->next = after->next;
    after->next = current;
    before->next = after;

    return;

}

void bubble_sort(List *list){
    if (is_empty(*list) == 1 || is_sorted_list(&(*list)) == 1){
        return;
    }

    Node *current = *list;

    while (current->next != NULL){
        if (current->data > current->next->data){
            Switch_Nodes(list, current->data);
        }
        else{
            current = current->next;
        }

    }
    
    if (is_sorted_list(&(*list)) == 0){
        Bubble_sort(&(*list));
    }

}

int quick_sort(int arr[], int start, int end){
    if(end<= start){
        return;
    }
    int pivot = partition(arr, start, end);
    quick_sort(arr, start, pivot - 1);
    quick_sort(arr, pivot + 1, end);

}

int partition(int arr[], int start, int end){
    int pivot = arr[end];
    int i = start -1;
    for (int j = start; j<= end - 1; j++){
        if (arr[j]<pivot){
            i = i +1;
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            
        }
    }
    i++;
    int temp = arr[i];
    arr[i] = arr[end];
    arr[end] = temp;
            
    

    return i;
}

insertion_sort(int arr[], int size){
    
    for (int i = 1; i < size; i++){
        int temp = arr[i];
        int j = i -1;

        while (j>= 0 && arr[j] > temp){
            arr[j+1] = arr[j];
            j = j-1;
        }
        arr[j+1] = temp;
    }
}

void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context) {
    if (p_event->type == NRFX_UARTE_EVT_RX_DONE) {
        char chachunga[] = "GAME EXIT";
        print_n();
        uarte_write(chachunga, strlen(chachunga));
        uint8_t ch = p_event->data.rx.p_data[0];
        if (ch == 'q'){
            exit(0);
            
        }

        
        nrfx_uarte_rx(&instance2, (uint8_t*)&ch, 1);
    }
}

//////////////////////////////////////////////////////GAME////////////////////////////////////////////////////////////

//this function handles the lights, each mole is assigned a light and a corresponding button needs to be pressed for the mole to get whacked. Points are given for how fast you are. The faster you are the more points you gain. You win by accumalating 1500 points.

int Mole_Light(int Mole, int Delay){
    int i = 0, pin_down = 0;
    pin_down = 0;
    i = 0;
    switch(Mole)
    {
    case MOLE_1:
        nrf_gpio_pin_write(LED1,ON);
        while(i < Delay && pin_down != 1){

         if (nrf_gpio_pin_read(BUTTON1) == ON){
          nrf_gpio_pin_write(LED1,OFF);
          pin_down = 1;
          uarte_write(hit, strlen(hit));

        }
        nrfx_systick_delay_ms(1);
        i = i +1;
    }
    nrf_gpio_pin_write(LED1,OFF);
    if (pin_down == 1){
        return Delay-i;
    }
    else{
        return 0;
    }   
    break;

        case MOLE_2:
        nrf_gpio_pin_write(LED2,ON);
        while(i < Delay && pin_down != 1){

         if (nrf_gpio_pin_read(BUTTON2) == ON){
          nrf_gpio_pin_write(LED2,OFF);
          pin_down = 1;
          uarte_write(hit, strlen(hit));

        }
        nrfx_systick_delay_ms(1);
        i = i +1;
    }
    nrf_gpio_pin_write(LED2,OFF);
    if (pin_down == 1){
        return Delay-i;
    }
    else{
        return 0;
    }   
        break;

        case MOLE_3:
                nrf_gpio_pin_write(LED3,ON);
        while(i < Delay && pin_down != 1){

         if (nrf_gpio_pin_read(BUTTON3) == ON){
          nrf_gpio_pin_write(LED3,OFF);
          pin_down = 1;
          uarte_write(hit, strlen(hit));

        }
        nrfx_systick_delay_ms(1);
        i = i +1;
    }
    nrf_gpio_pin_write(LED3,OFF);
    if (pin_down == 1){
        return Delay-i;
    }
    else{
        return 0;
    }   
        break;
        
        case MOLE_4:
        nrf_gpio_pin_write(LED4,ON);
        while(i < Delay && pin_down != 1){

         if (nrf_gpio_pin_read(BUTTON4) == ON){
          nrf_gpio_pin_write(LED4,OFF);
          pin_down = 1;
          uarte_write(hit, strlen(hit));

        }
        nrfx_systick_delay_ms(1);
        i = i +1;
    }
    nrf_gpio_pin_write(LED4,OFF);
    if (pin_down == 1){
        return Delay-i;
    }
    else{
        return 0;
    }   
        break;
    }
    
}

//this is the main function for the game, this is what you call when you want to start it
void start_game_grupp21(){
    char exit;
    int Rand_Mole, Rand_Delay, points = 0, points_added;
    uint8_t clearscreen[] = CLEAR_SCREEN;

    //aktiverar en uarte interuppt. Tryck på q om du vill lämna spelet.
    nrfx_uarte_rx(&instance2, &exit, 1);

    do{

        // The game chooses a random mole and then chooses a random delay
        Rand_Mole = Rand_int(4)+1;
        Rand_Delay = Rand_int(300)+350;
        points_added = Mole_Light(Rand_Mole, Rand_Delay);
        points = points +  points_added;

        //display points added, current points and if you whacked the mole
        print_n();
        uarte_write(Points_add, strlen(Points_add));
        send_int(points_added);

        print_n();
        uarte_write(Points_curr, strlen(Points_curr));
        send_int(points);
        print_n();

        //1 second delay per mole to get ready to whack!
        nrfx_systick_delay_ms(1000);
        uarte_write(clearscreen, sizeof(clearscreen));

    } while (points < 1500);
    //displays that you reached mroe than 1500 points
    uarte_write(win, strlen(win));
    print_n();

    uarte_write(Points_curr, strlen(Points_curr));
    send_int(points);

return;



}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void init(){

  const nrfx_uarte_config_t config2 = NRFX_UARTE_DEFAULT_CONFIG(PIN_TXD, PIN_RXD);
  nrfx_err_t errr2 = nrfx_uarte_init(&instance2, &config2, uarte_handler);

  NVIC_ClearPendingIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));
  NVIC_EnableIRQ(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(0)));

  nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;
  nrfx_err_t errr = nrfx_rtc_init(&rtc_instance, &config, NULL);
  nrfx_rtc_enable(&rtc_instance);

    int time;
    time = nrfx_rtc_counter_get(&rtc_instance);
    srand(time);

  nrfx_systick_init();

  
  nrf_gpio_cfg_input(23, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_input(24, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_input(8, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_input(9, NRF_GPIO_PIN_PULLUP);

  nrf_gpio_cfg_output(LED1);
  nrf_gpio_cfg_output(LED2);
  nrf_gpio_cfg_output(LED3);
  nrf_gpio_cfg_output(LED4);


  nrf_gpio_pin_write(LED1,1);
  nrf_gpio_pin_write(LED2,1);
  nrf_gpio_pin_write(LED3,1);
  nrf_gpio_pin_write(LED4,1);
  

}