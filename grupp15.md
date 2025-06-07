# Grupp 15 spel

## Simon Says - Hur man spelar
Spelets princip går ut på att fyra LED lampor blinkar i en slumpmässig sekvens som spelaren ska härma genom att trycka på tangenter som representerar respektive lampa. I detta spel representeras led 1 av 4 tangenten, led 2 av 5 tangenten, led 3 av 1 tangenten och led 4 av 2 tangenten. Spelet startar när spelaren trycker på "knapp 1" på nrf5340DK kortet. Sekvensen ökar i längd efter varje klarad "runda" tills spelaren matar in fel sekvens. När detta händer börjar spelet om på nytt.

## Blockdiagram för spelet - systemöverblick



- [Tangentbord / Användare]
-      → (Input: användaren trycker på knapp för att starta spelet)
-     → [PC / Terminalprogram, PuTTY]
-     → UART via USB
-     → [nRF5340-DK]
-         → Initierar spel & nedräkning:  -            - Visar nedräkning (3, 2, 1) via UART
-             - Genererar slumpmässig LED-sekvens
-             - Blinkar LED:ar (1–4) i följd
-
-     → Väntar på användarens input:
-         → [Knapptryckningar via UART: 4, 5, 1, 2]
-         → Mottas av nRF5340-DK via UART-interrupt
-
-     → Kontroll:
-         - Jämför användarens input med rätt sekvens
-         - Om korrekt:
-             → Skicka "Correct!" via UART till PC
-             → Öka sekvenslängden med 1
-             → Starta ny runda
-         - Om fel:
-             → Skicka "Wrong!" via UART till PC
-             → Skicka antalet rundor användaren klarade
-             → Starta om spelet
-
-     → All feedback visas på PC:n i terminalfönster (PuTTY)
 

## Pseudokod - överblick av hur koden implementeras

void spel_loop() 
{
    
    uarte_write("Press button 1 to start");
   
    while (knapp_inte_nedtryckt());
    
    srand(rtc_countern);
    uarte_write("Game starts in: 3\n");
    delay(1000);
    uarte_write("2\n");
    delay(1000);
    uarte_write("1\n");

    while(1)
    {
        slumptal_till_array();
        leds_blinkar_i_sekvens();

        
        ta_emot_inputs();

        if (inputs_korrekt()) 
        {
            uarte_write("Correct!\n");
            nästa_runda(); // sekvensen blir längre
        } else 
        {
            uarte_write("Wrong! Rounds completed: X\n");
            starta_om_spelet();
        }
    }
}

