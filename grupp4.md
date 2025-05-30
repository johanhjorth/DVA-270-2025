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

## The technicals

### System parmeters
```
[Keyboard]
- Inputs to game

[PuTTY]
- UART communication

[nRF card]
- Processing inputs
- Sending outputs
- Running logics
```

### Code
```
start_game_grupp4()
    initialize INTERUPTS
    initialize TIMERS
    initialize UART // PuTTY communication

    LOOP PLAYFIELD_MATRIX
        IF is_border 1
        else 0
    
    print SNAKE_IMAGE_SCREEN
    wait KEYPRESS

    //SNAKE GAME
    print PLAYGFIELD
    print PLAYER
    WHILE not_collided && not_esc_key:
        move_direction PLAYER
        check_collided
        wait DELAY
        on_keypress:
            change_direction PLAYER
        
    show_points SNAKE
    wait KEYPRESS

    //NUMBER CATCHER
    print TARGET_RND_NUM
    WHILE not_esc_key:
        print CURRENT_RND_NUM
        on_keypress:
            IF correct_num:
                print "Bra jobbat!"
            ELSE
                print "FAIL!"
    
    print "Hej då!"
```


## Contact
Any questions just talk to Johannes or Angelina in class.

## Acknowledgments
We want to thank Mälardalens university and our teachers Jonas and Johan for having been given the opportunity to learn by practical excersises and fun tasks. We also want to thank mr chatGPT for your patience with all our strange questions.


