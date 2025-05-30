# Grupp4 - Snake Game &  Number catcher
A snake type game and a RNG reaction game in one. You run the game on a nRf card connected to a PC with PuTTY. No interactions with the card is needed for this game.

The Snake Game gives you one attempt, while you can play the Number Catcher Game as many times as you like.

Start the game by entering '4' in main menu.

Enjoy!

### Features:
- Super cool welcome screen
- Two games in one
- Color graphics (ANSI)
- Game timer and length in Snake Game
- Endless gameplay in Number Catcher Game

## Controls
### Snake Game:
| Key | Action           | 
| --- | ---------------- |
| w   | Moves snake up   |
| s   | Moves snake down |
| a   | Moves snake left |
| d   | Moves snake right|
| esc | End Snake Game    |

### Number Catcher Game:
| Key      | Action                 | 
| -------- | ---------------------- |
| esc      | End Number Catcher Game|
| any other key| Stop at number         |

Key setting macros are located in **grupp4.h**

## Gameplay
The Snake Game is a snake that grows and grows inside a rectangle. Try to make the snake as long as possible without going into your own tail or the walls.

The Number Catcher Game is a RNG reaction game where you need to wait for your number to appear, and then react by pushing a key to stop it.

## System

architecture-beta
    group api(logos:aws-lambda)[API]

    service db(logos:aws-aurora)[Database] in api
    service disk1(logos:aws-glacier)[Storage] in api
    service disk2(logos:aws-s3)[Storage] in api
    service server(logos:aws-ec2)[Server] in api

    db:L -- R:server
    disk1:T -- B:server
    disk2:T -- B:db
    
```mermaid
graph system;
    Keyboard-->B;
    A-->C;
    B-->D;
    C-->D;
```

[Tangentbord]
     │ (tangenttryckning via USB)
     ▼
     [PC med PuTTY]
     │ (UART via USB-CDC)
     ▼
   [nRF5340DK]
     │
     ├── Tar emot input via UART
     ├── Skriver ANSI-grafik till PuTTY
     └── Kör spel-logik i C


## Contact
Any questions just talk to Johannes or Angelina in class.

## Acknowledgments
We want to thank Mälardalens university and our teachers Jonas and Johan for having been given the opportunity to learn by practical excersises and fun tasks. We also want to thank mr chatGPT for your patience with all our strange questions.


