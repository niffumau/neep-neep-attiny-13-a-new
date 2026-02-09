////// Includes //////////
#include "Arduino.h"

#include "main.h"
#include "tunes.h"
#include "functions.h"
#include "functions-led.h"
//#include "functions-sleep.h"


const uint8_t divisors[] = {142,134,127,120,113,106,100,95,89,84,79,75,71,67,63,59,56,53,50,47,44,42,39,37,35,33,31,29,27,26,24,23,22,20,19,18};




const uint8_t tune_nokia_new[] PROGMEM = {
  (uint8_t) 13,
  NOTE_6E,2, NOTE_6D,2, NOTE_5FS,4, NOTE_5GS,2,
  NOTE_6CS,2, NOTE_5B,2, NOTE_5D,4, NOTE_5E,2,
  NOTE_5B,2, NOTE_5A,2, NOTE_5CS,4, NOTE_5E,4, NOTE_5A,4
};


const uint8_t tune_sms_new[] PROGMEM = {
  (uint8_t) 8,
  NOTE_5A,2, NOTE_5A,2, NOTE_5A,2,
  NOTE_5A,6, NOTE_5A,6, 
  NOTE_5A,2, NOTE_5A,2, NOTE_5A,2
};

const uint8_t tune_iphone_new[] PROGMEM = {
  (uint8_t) 26,
	NOTE_4G,4, NOTE_4G,2, NOTE_4AS,2, NOTE_5C,2,
	NOTE_5C,1, NOTE_4AS,1, NOTE_4G,2, 
	NOTE_5C,2, NOTE_4G,2, 
	NOTE_5C,2, NOTE_4A,2, NOTE_5C,2, NOTE_4F,8,

  NOTE_4G,4, NOTE_4G,2, NOTE_4AS,2, NOTE_5C,2,
	NOTE_5C,1, NOTE_4AS,1, NOTE_4G,2, 
	NOTE_5C,2, NOTE_4G,2, 
	NOTE_5C,2, NOTE_4A,2, NOTE_5C,2, NOTE_4F,8,
};


/////////////// testing stuff //////////////////////////////
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

const tune_t tune_new PROGMEM = {
  .size = 8,
  .notes = { {NOTE_5A,1}, {NOTE_5A,1}, {NOTE_5A,1}, {NOTE_5A,1},}
};

const uint8_t tune_nokia_rep[] PROGMEM = {
  (uint8_t) 13,
  NOTE_6E,1, NOTE_6D,1, NOTE_5FS,2, NOTE_5GS,1,
  NOTE_6CS,1, NOTE_5B,1, NOTE_5D,2, NOTE_5E,1,
  NOTE_5B,1, NOTE_5A,1, NOTE_5CS,2, NOTE_5E,2, NOTE_5A,2

};



/*
const tune_t tune_new2 = {
  .size = 8,
  .notes = tune_newd
};*/


////////////////////////////////////////////////////////////////////




#ifndef IS_BUZZER

/**
 * @brief Generates a musical note on the buzzer using Timer0 PWM at 1.2MHz ATTiny13A clock.
 *
 * Plays a single note by configuring Timer0 for Fast PWM mode with hardware-generated
 * square wave output on OC0B (likely PB4). Note frequencies span ~3 octaves using 3
 * prescalers (N256, N64, N8) and a precomputed divisors table. Duration controls total
 * note time (on + off). Based on note frequency calculations from Suits' formula.
 *
 * @param _note Note index (0-83): Maps to frequencies from ~65Hz (C3) to ~4186Hz (C9).
 *              - 0-23: N256 prescaler, divisors[0-23]
 *              - 24-59: N64 prescaler, divisors[0-35] 
 *              - 60-83: N8 prescaler, divisors[0-23]
 *              Uses global `divisors[]` table (presumably pgm_read_byte() accessible).
 *
 * @param _duration Duration value passed to _mydelay() for note on/off timing.
 *
 * @pre Requires:
 *      - Global `uint8_t divisors[84]` array with precomputed OCR values
 *      - `#define N_256 (CS02|CS00)`, `#define N_64 (CS01|CS00)`, `#define N_8 CS00`
 *      - Timer0 preset to Fast PWM mode: `TCCR0A = _BV(COM0B0) | _BV(WGM01) | _BV(WGM00)`
 *      - `BUZZER_PIN` defined and configured as output
 *      - `_mydelay()` blocking delay function
 *
 * @note 50% duty cycle via OCR0B = OCR0A/2. Silences timer + pin after note.
 * @note Commented bounds checks suggest original 0-83 range validation.
 * @note 1.2MHz clock yields coarse resolution for high notes (N8 max ~15kHz).
 *
 * @warning No frequency silencing (note=0). Direct timer manipulation—callers must not
 *          interfere with Timer0 during playback. Global side effects on TCCR0A/B, OCR0A/B.
 */
void play_note(uint8_t _note, uint8_t _duration) {
	uint8_t _prescaler;
	uint8_t divisor;
	
	if (_note <  24) {		// Work out the prescaler
		_prescaler = N_256;
		divisor = divisors[_note];
	} else if (_note < 60 ) {
		_prescaler = N_64;
		divisor = divisors[_note-24];
	} else {
		_prescaler = N_8;
		divisor = divisors[_note-60];

	}

	TCCR0B = (TCCR0B & ~((1<<CS02)|(1<<CS01)|(1<<CS00))) | _prescaler;

	OCR0A = divisor;		// set count
	OCR0B = divisor/2;		// set count for duty, so duty = OCR0B/OCR0A
	TCCR0A |= _BV(COM0B0);

	_mydelay(_duration);
	TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00)); // stop the timer
	digitalWrite(BUZZER_PIN, LOW); // set the output to low
	_mydelay(_duration);

}



/***************************************************
 *  playtune_scale
 ***************************************************
 *  
 *   
 */
void playtune_scale(void) {

	for (uint8_t octave=3; octave <= 5; octave++) {
		led_status(1,octave);
		for	(uint8_t i=0; i < 12; i++) {
			uint8_t note = octave*12+i;
			play_note(note,2);
		}
		_mydelay(4);
	}
}

#endif

/***************************************************
 *  playtune_melody
 ***************************************************
 *  This should play a tune 
 *   
 */

void playtune_melody(const notes_t *melody,uint8_t _size) {
  //uint8_t _length = pgm_read_byte(&melody[0]);
	//led_status(4,_length);  return;   // debug

	for	(int i=0; i <_size; i++) {
		play_note(pgm_read_byte(&melody[i].note),pgm_read_byte(&melody[i].duration));	
	}
}

/**
 * @brief Plays a complete melody stored in PROGMEM using note/duration pairs.
 *
 * This function reads a compact melody format from flash memory (PROGMEM) and plays
 * each note sequentially. The melody data starts with a length byte followed by
 * note-duration pairs (2 bytes each: note, duration). Notes are frequency values
 * suitable for play_note(), durations control note length/timing.
 *
 * @param melody Pointer to PROGMEM array containing melody data.
 *               Format: `[length, note1, duration1, note2, duration2, ...]`
 *               - melody[0] = number of notes (uint8_t)
 *               - melody[1+2*i] = note frequency (uint8_t) for note i
 *               - melody[2+2*i] = duration (uint8_t) for note i
 *
 * @warning No error checking on length or buffer bounds—caller must ensure valid data.
 */
void playtune_melody_new(const uint8_t *melody) {
	uint8_t _length = pgm_read_byte(&melody[0]);
	//led_status(4,_length);  return;   // debug  
	for	(int i=0; i <_length; i++) {
		play_note(pgm_read_byte(&melody[1+2*i]),pgm_read_byte(&melody[2+2*i]));	
	}
}

