Hur spelar man

Spelet är ett tidsbegränsat frågesportspel där spelaren får 10 slumpmässigt valda flervalsfrågor. Spelaren svarar genom att trycka på tangent 1 eller 2 på sitt tangentbord (via UART-kommunikation).

Spelet startar automatiskt och pågår i 20 sekunder.

En fråga i taget presenteras via terminalen (UART).

Spelaren svarar med '1' eller '2'.

Efter 20 sekunder avslutas spelet automatiskt, och den totala poängen skrivs ut.

Blockdiagram

[Tangentbord]
      |
      |
  [Putty]
      |
      |
[nrfx kortet]
      |
      |
[Spela frågesport]

Kod översikt

start_game_grupp26() – Startar spelet, loopar tills 20 sekunder passerat.

printQuestion() – Skriver ut en slumpmässigt vald fråga.

uarte_handler() – Tar emot och tolkar spelarens svar.

countScore() – Uppdaterar poängen (definieras ej här men används).

Pseudokod

Funktion start_game:
    Initiera UART och RTC
    Aktivera UART-interupt
    Starta mottagning (UART RX)
    Starta spel-timer (20 sekunder)

    loop medan spel pågår:
        Generera slumpmässigt fråge-ID (1–10)
        Anropa printQuestion()

    När tiden är slut:
        Skriv ut spelarens slutpoäng





    Funktion printQuestion:
    Sätt korrekt svar beroende på fråge-ID
    Skicka fråga till terminal via UART



    Vänta tills användarens svar är mottaget

    Om svar är korrekt:
        point = 1
    annars:
        point = 0

    Anropa countScore()



    UARTE Interrupt Handler:
    Om mottagning är klar:
        Läs tecken
        Om tecknet är '1' eller '2':
            Spara svar
            Sätt att svar är mottaget
        Starta om mottagning