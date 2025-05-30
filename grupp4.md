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

### Keyboard
- Inputs to game
### PuTTY
- UART communication
### nRF card
- Processing inputs
- Sending outputs
- Running logics

```flow
st=>start: Login
op=>operation: Login operation
cond=>condition: Successful Yes or No?
e=>end: To admin

st->op->cond
cond(yes)->e
cond(no)->op
```

```seq
Andrew->China: Says Hello 
Note right of China: China thinks\nabout it 
China-->Andrew: How are you? 
Andrew->>China: I am good thanks!
```

## Contact
Any questions just talk to Johannes or Angelina in class.

## Acknowledgments
We want to thank Mälardalens university and our teachers Jonas and Johan for having been given the opportunity to learn by practical excersises and fun tasks. We also want to thank mr chatGPT for your patience with all our strange questions.


