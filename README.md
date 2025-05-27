# Programmering för inbyggda system, DVA270

Detta repository används i kursen **DVA270 - Inbyggda System** för att samla och bygga studenters spelutvecklingsuppgifter baserade på **Nordic Semiconductor NRF5340 DK**.

## Översikt

I kursen har vi övat på:
- Att använda utvecklingsmiljö och verktyg för programmering av inbyggda system
- Att felsöka och testa inbyggd mjukvara
- Introducerat git och versionshantering

## Filformat

Varje grupp ska skapa sina filer enligt följande struktur:
- **gruppX.c** och **gruppX.h** (där X är ert gruppnummer 1–31)
  - Funktionen i `.c`-filen ska heta `void start_game_gruppX(void);`
  - `.h`-filen ska innehålla deklarationen av funktionen samt eventuella `#include`, `#define`, och `typedef`
  - Använd **header guards** enligt:
    ```c
    #ifndef GRUPPX_H
    #define GRUPPX_H
    ...
    #endif
    ```

Testa er kod genom att inkludera er `.h`-fil i main och anropa `start_game_gruppX();`.

## Bygga och köra

Använd den medföljande **Makefile** för att bygga projektet. Byggkommandot är:

```bash
make
```

För att flasha programmet till brädan:

```bash
make jflash
```

## Arbetsflöde för inlämning

1. **skapa en kopia (fork)** av detta repository.
2. **Skapa** era egna filer `gruppX.c` och `gruppX.h`.
3. **Ändra** Makefile (om det behövs) för att lägga till era filer i byggkedjan (se till att de finns i `SRC_FILES`).
4. **Testa** att er kod bygger och kör.
5. **Gör en Merge Request** till detta repository för inlämning.

## Lycka till och glöm inte:
- Arbeta tillsammans
- Redovisa muntligt så båda kan visa förståelse
- Gör det lagom enkelt – och framförallt fungerande
