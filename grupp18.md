# The math game
## By grupp 18
### How to play

- Start by entering the requested number
- then do it again
    - After that you will get a equation so tolve and press in the the answer you have followed by enter.
    - If your answer was correct you will continue the game by pressing the requested number. 
    - If your answer was wrong a gave over text will apear followed by a the abilty to restart by pressing the reqested number and enter.
    - If you time out answering the question a game over text will also apear followed by the abilty to restart by pressing the requested number and enter.
- By answering correctly and pressing the right number you can continue though the levels until you reach the final level and after answering correctly a win message will apear followed by a abilty to restart the game.    

### Block diagram

```mermaid
tangentbord -> PC -> uarte -> nrf5340DK -> uarte -> pc
```

### Implementation
```c
#include "grupp18.h"

void uarte_handler(nrfx_uarte_event_t const *p_event, void *p_context)//sparar inputs tills enter och interuptar genom att ändra state och answer

void charthers(int choice)//använd för att printa tecken mellan två funktioner

int function_addition(int diff,int time)//genererar random sum uttryck

int function_subtraction(int diff,int time)//genererar random substracktions uttryck

int function_multiplication(int diff,int time)//genererar random multiplications uttryck

int countdown(int seconds,int level, int facit)// kollar om varet är rätt och räknar ner tiden

void start_game_grupp18(void)
{
 
    nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG(20, 22);//aktiverar uarte och interrupt
   
    nrfx_uarte_rx (&instance, &uarte_buffer, sizeof(uarte_buffer));//aktiverar interrupt genom att söka knapptryck
    while(1)
    {

       
        
        while(game_on==1)
        {
            
            if(level>0 && level<8)//har inre funktioner som kollar om man har förlorat eller vunnit och väntar på svar inan
            {
                // skriver ut clear screen, continue och enter 
                while(next==1)//håller en kvar tills knapptryck samtidigt som en intern timer
                {
                    // en intern timer som vänar på knapptryck
                }
            }
            switch(level)//innehåller lose/win screen och alla levels
            {
               // olika cases som nivåner, genereras uttryck, kontrollerar svar, vid högsta cases nått skriv vinstmeddeladen och vid fel svar game over och en chans att bärja om 
            }
        }
    }
}
```

0
**bold**
*italic*

