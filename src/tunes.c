////// Includes //////////
#include "Arduino.h"

#include "main.h"
#include "tunes.h"
//#include "functions.h"
//#include "functions-led.h"
//#include "functions-sleep.h"



const notes_t tune_newd[] PROGMEM = {
  {NOTE_5A,1},
  {NOTE_5A,1},
  {NOTE_5A,1},
  {NOTE_5A,3},
  {NOTE_5A,3},
  {NOTE_5A,1},
  {NOTE_5A,1},
  {NOTE_5A,1}
};

const tune_t tune_new = {
  .size = 8,
  .notes = { {NOTE_5A,1}, {NOTE_5A,1}, {NOTE_5A,1}, {NOTE_5A,1},}
};

/*
const tune_t tune_new2 = {
  .size = 8,
  .notes = tune_newd
};*/


