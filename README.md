## General

This is the one I am actually using for the ATtiny13a modules.

The ATtiny13a has very limited to 1k flash so you can't fit a lot in it.

## Hardware
The hardware basically works on all the Neep Neep boards, https://gitlab.niffum.net/kicad/neepneep
I need to change the URL for that to include kicad in the name.


## Debugging
I need to write this section but  some of the things:
  - #define BEEP_EVERY_CYCLE

## Neep Neep Selection

You can change what type of neep neep it is by setting defines in the main.h file.

There are two main variations:
1.  Speaker
2.  Buzzer

### Speaker Neep Neeps

Speaker Neep Neeps
  - Don't have IS_BUZZER or IS_SMOKE_ALARM Defined

### Buzzer Neep Neeps
Buzzer Neep neep:
  - #define IS_BUZZER
Smoke Alarm (BUZZER)
  - #define IS_SMOKE_ALARM


## Installation

I use VSCode. and the USBASP programmer.

1.  Set the type of neep neep (see the section for that:
    1.  Standard Speaker
    2.  Buzzer
        1.  Neep Neep Buzzer
        2.  Smoke Alarm Buzzer

2.  For new ATtiny13a's, you will most likely have to set the fuses.
    In VSCode, the platformio button.  Go under attiny13a > Platform > Setfuses

3.  Upload the code.
    In VSCode, the platformio button.  Go under attiny13a > General > Upload


