/*
 *
 * 
 * https://github.com/pedroliska/ATtiny85/blob/master/watchdog-wake/watchdog-wake.ino
 * 
 * 
 * 
 * 
 */



#include <Arduino.h>
#include <avr/io.h>
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <avr/interrupt.h>

#include <avr/power.h>
#include <avr/sleep.h>

//#include <math.h>		// fuck me this can use up a lot of flash
//#include <avr/wdt.h>

#ifndef ARDUINO
#include <stdint.h>
#include "millionUtil.h"         //not needed if compiled with Arduino & Arduino-Tiny
#endif

#include <attiny13a-sleep.h>
#include <sleepfunction.h>





uint8_t mcucr1, mcucr2;

int REGULAR_HI_MS = 100;        // was 800
int WAKE_INDICATOR_HI_MS = 0; //200;
int INITIAL_BEEP_COUNT = 3;   // number of "test" beeps before we go into the real loop



/* We then just need a table of divisors for the notes within one octave. To calculate the divisor for a given note frequency we first work out:

divisor = clock / frequency

For example, C4 (middle C) is 261.63Hz, so we get:

divisor = 1000000 / 261.63

in our case thats
divisor = 1 200 000 / 261.63 = 4586.62
but thats way more than 256, so divide by 2
4586.62 / 2 = 2293.3 
too large, divide by 2 again
1146, too large again, divide by 2
too large..
options are 1, 8, 64,
divide by 8, and its 71.6


*/

//uint8_t mcucr1, mcucr2;

//int notes[12]={239,225,213,201,190,179,169,159,150,142,134,127};
//int notes[12]={72,225,213,201,190,179,169,159,150,142,134,127};    



typedef struct s_note {
	uint8_t OCRxn; // 0..255
	uint8_t N;
} note_t;

typedef struct s_octave {
	note_t note_C;
	note_t note_CS;
	note_t note_D;
	note_t note_DS;
	note_t note_E;
	note_t note_F;
	note_t note_FS;
	note_t note_G;
	note_t note_GS;
	note_t note_A;
	note_t note_AS;
	note_t note_B;
} octave_t;

/*
// these all need to be multiplied by 8 because the clock is 8x faster
//24 bytes per octave
//PROGMEM const octave_t octaves[4] = {
	{ // octave 0
	.note_C = {142, N_256}, // 16.35 Hz
	.note_CS = {134, N_256}, // 17.32 Hz
	.note_D = {127, N_256}, // 18.35 Hz
	.note_DS = {120, N_256}, // 19.45 Hz
	.note_E = {113, N_256}, // 20.60 Hz
	.note_F = {106, N_256}, // 21.83 Hz
	.note_FS = {100, N_256}, // 23.12 Hz
	.note_G = {95, N_256}, // 24.50 Hz
	.note_GS = {89, N_256}, // 25.96 Hz
	.note_A = {84, N_256}, // 27.50 Hz
	.note_AS = {79, N_256}, // 29.14 Hz
	.note_B = {75, N_256} // 30.87 Hz
	},
	{ // octave 1
	.note_C = {71, N_256}, // 32.70 Hz
	.note_CS = {67, N_256}, // 34.65 Hz
	.note_D = {63, N_256}, // 36.71 Hz
	.note_DS = {59, N_256}, // 38.89 Hz
	.note_E = {56, N_256}, // 41.20 Hz
	.note_F = {53, N_256}, // 43.65 Hz
	.note_FS = {50, N_256}, // 46.25 Hz
	.note_G = {47, N_256}, // 49.00 Hz
	.note_GS = {44, N_256}, // 51.91 Hz
	.note_A = {42, N_256}, // 55.00 Hz
	.note_AS = {39, N_256}, // 58.27 Hz
	.note_B = {37, N_256} // 61.74 Hz
	},
	{ // octave 2
	.note_C = {142, N_64}, // 65.41 Hz
	.note_CS = {134, N_64}, // 69.30 Hz
	.note_D = {127, N_64}, // 73.42 Hz
	.note_DS = {120, N_64}, // 77.78 Hz
	.note_E = {113, N_64}, // 82.41 Hz
	.note_F = {106, N_64}, // 87.31 Hz
	.note_FS = {100, N_64}, // 92.50 Hz
	.note_G = {95, N_64}, // 98.00 Hz
	.note_GS = {89, N_64}, // 103.83 Hz
	.note_A = {84, N_64}, // 110.00 Hz
	.note_AS = {79, N_64}, // 116.54 Hz
	.note_B = {75, N_64} // 123.47 Hz
	},
	{ // octave 3
	.note_C = {71, N_64}, // 130.81 Hz
	.note_CS = {67, N_64}, // 138.59 Hz
	.note_D = {63, N_64}, // 146.83 Hz
	.note_DS = {59, N_64}, // 155.56 Hz
	.note_E = {56, N_64}, // 164.81 Hz
	.note_F = {53, N_64}, // 174.61 Hz
	.note_FS = {50, N_64}, // 185.00 Hz
	.note_G = {47, N_64}, // 196.00 Hz
	.note_GS = {44, N_64}, // 207.65 Hz
	.note_A = {42, N_64}, // 220.00 Hz
	.note_AS = {39, N_64}, // 233.08 Hz
	.note_B = {37, N_64} // 246.94 Hz
	},
	{ // octave 4
	.note_C = {35, N_64}, // 261.63 Hz
	.note_CS = {33, N_64}, // 277.18 Hz
	.note_D = {31, N_64}, // 293.66 Hz
	.note_DS = {29, N_64}, // 311.13 Hz
	.note_E = {27, N_64}, // 329.63 Hz
	.note_F = {26, N_64}, // 349.23 Hz
	.note_FS = {24, N_64}, // 369.99 Hz
	.note_G = {23, N_64}, // 392.00 Hz
	.note_GS = {22, N_64}, // 415.30 Hz
	.note_A = {20, N_64}, // 440.00 Hz
	.note_AS = {19, N_64}, // 466.16 Hz
	.note_B = {18, N_64} // 493.88 Hz
	},
	{  // octave 5
	.note_C = {142, N_8}, // 523.25 Hz
	.note_CS = {134, N_8}, // 554.37 Hz
	.note_D = {127, N_8}, // 587.33 Hz
	.note_DS = {120, N_8}, // 622.25 Hz
	.note_E = {113, N_8}, // 659.25 Hz
	.note_F = {106, N_8}, // 349.23 Hz
	.note_FS = {100, N_8}, // 369.99 Hz
	.note_G = {95, N_8}, // 392.00 Hz
	.note_GS = {89, N_8}, // 415.30 Hz
	.note_A = {84, N_8}, // 440.00 Hz
	.note_AS = {79, N_8}, // 466.16 Hz
	.note_B = {75, N_8} // 493.88 Hz
	},
	{  // octave 6
	.note_C = {71, N_8}, // 1046.50 Hz
	.note_CS = {67, N_8}, // 1108.73 Hz
	.note_D = {63, N_8}, // 1174.66 Hz
	.note_DS = {59, N_8}, // 1244.51 Hz
	.note_E = {56, N_8}, // 1318.51 Hz
	.note_F = {53, N_8}, // 1396.91 Hz
	.note_FS = {50, N_8}, // 1479.98 Hz
	.note_G = {47, N_8}, // 1567.98 Hz
	.note_GS = {44, N_8}, // 1661.22 Hz
	.note_A = {42, N_8}, // 1760.00 Hz
	.note_AS = {39, N_8}, // 1864.66 Hz
	.note_B = {37, N_8} // 1975.53 Hz
	},
	{  // octave 7	// all N_8
	.note_C = {35, N_8}, // 2093.00 Hz
	.note_CS = {33, N_8}, // 2217.46 Hz
	.note_D = {31, N_8}, // 2349.32 Hz
	.note_DS = {29, N_8}, // 2489.02 Hz
	.note_E = {27, N_8}, // 2637.02 Hz
	.note_F = {26, N_8}, // 2793.83 Hz
	.note_FS = {24, N_8}, // 2959.96 Hz
	.note_G = {23, N_8}, // 3135.96 Hz
	.note_GS = {22, N_8}, // 3322.44 Hz
	.note_A = {20, N_8}, // 3520.00 Hz
	.note_AS = {19, N_8}, // 3729.31 Hz
	.note_B = {18, N_8} // 3951.07 Hz
	}
};*/

void _mydelay(uint8_t _delay) {
	for (uint8_t i=0;i < _delay;i++ ){ 
		_delay_ms(80);
	}

}

/*******************************************************************************************************************************
 *  LED Functions
 *******************************************************************************************************************************/

/***************************************************
 *  led_setup
 ***************************************************
 * 
 */
void led_setup(void){
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  //DDRB |= (1 << LED_PIN);	// set as output
  //DDRB |= (1 << LED_PIN2);	// set as output
  PORTB = 0b00000000;	// Set all of port b to 0
  //digitalWrite(LED_PIN, LOW); 
  //digitalWrite(LED_PIN2, LOW); 

}
/***************************************************
 *  led_toggle
 ***************************************************
 * 
 */
void led_toggle(uint8_t _PIN){
  //PORTB ^= _BV(LED_PIN2);  // toggle LED pin		//_BV returns the byte value of some shit
  PORTB ^= _BV(_PIN);  // toggle LED pin		//_BV returns the byte value of some shit
  //digitalWrite(LED_PIN2, HIGH); 

}
/***************************************************
 *  led_on
 ***************************************************
 * 
 */
void led_on(int _PIN){
	digitalWrite(_PIN, HIGH); 
	//DDRB |= (1 << _PIN);		// these result in the same shit

}

/***************************************************
 *  led_off
 ***************************************************
 * 
 */
void led_off(int _PIN){
  	digitalWrite(_PIN, LOW); 
	//DDRB &= ~(0 << _PIN);
}
void led_blink(int _PIN,int _number) {
	for (uint8_t i=0;i < _number;i++ ){ 
		//PORTB ^= _BV(_PIN);
		digitalWrite(_PIN, HIGH);
		_delay_ms(BLINK_INTERVAL_ON);
		digitalWrite(_PIN, LOW);
		_delay_ms(BLINK_INTERVAL_OFF);
	}
	_delay_ms(BLINK_INTERVAL_END);
}

void led_status(int _first,int _second) {
	led_blink(LED_RED,_first);
	led_blink(LED_GREEN,_second);
	_delay_ms(BLINK_INTERVAL_STATUS_END);
}



/***************************************************
 * check_random 
 ***************************************************
 * Check that the random number is between our limits
 */
void check_random(uint16_t _return) {

	if (_return < RANDOM_SLEEP_MIN*8) {
		//led_blink(LED_RED,4);
		led_status(4,2);
	}
	if (_return > RANDOM_SLEEP_MAX*8 + 2){
		//led_blink(LED_GREEN,4);
		led_status(4,4);
	}
}
/*******************************************************************************************************************************
 *  Tone Functions
 *******************************************************************************************************************************/
/***************************************************
 *  
 ***************************************************
 * 
 */
#ifndef IS_BUZZER	
static void _setuptone(void){

	pinMode(BUZZER_PIN, OUTPUT);
	//PORTB = 0b00000000; // set all pins to LOW

	// Set Timer Mode to Fast PWM
	// Below, it looks like they are just setting WGM01 to 1,, the manual seems to say that WGM01 and WGM00 should be 1 ?

//	 // set timer mode to Fast PWM
	


	// fucking really doesn't make sense

	// notes https://forum.arduino.cc/t/attiny13a-fast-pwm/855861
	/* When WGM02, WGM01, WGM00 is set, the TOP value of the timer is controlled by OCRA.
	That is, the timer count increments from 0 to the value of OCR2A and returns to 0.
	This allows you to control the frequency, but OCRB must be used for waveform output.
	The OCRB value must be between 0 and OCRA.
	Because it is the PWM duty. */

	// Following that
	TCCR0A |= ((1<<WGM02)|(1<<WGM01)|(1<<WGM00));		// this should really be mode 7
//	TCCR0A |= (1<<WGM00);	// Waveform generator as PWM, Phase correct

//	TCCR0A |= ((1<<WGM01)|(1<<WGM00)); // set fast PWM mode 7 - page 79 - uses OCR0A for TOP, PWM signal comes out on OCR0B
	TCCR0A |= (1<<COM0B1)|(1<<COM0B0); // define inverted

	TCCR0B |= (1<<WGM02); // to use TOP as OCR0A rather than 0xFF
	




	// Select which pin connects to Timer 0
	//TCCR0A |= (1<<COM0A0); // connect PWM pin to Channel A of Timer0

	// So the COM0B1 and COM0B0 contyrol what it does... 
	// both 0 means disconnected
	TCCR0A |= _BV(COM0B0); // connect PWM pin to Channel A of Timer0... PB1 ?
}
#endif

/***************************************************
 *  Stop tones
 ***************************************************
 * 
 */
#ifndef IS_BUZZER	
static void stop(void)
{

	TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00)); // stop the timer.... This should absoloutly stop the timer
	
	// TCCR1 = 0x90;              // stop the counter		// dont' think this works...
  
  	// maybe this is a better stop the couter thing..

	//pinMode(BUZZER_PIN, OUTPUT);

//	TCCR0A |= _BV(COM0B0);	//////////maybe this ??? NOPE, not just that
//	TCCR0A |= ~((1<<COM0B0)|(1<<COM0B1));	// maybe both of these?  NOPE

  	TCCR0A = 0; // stop the counter    // fuck knows why... fuck knos why the other one dind't work				////////////////disable this if it stops working

	digitalWrite(BUZZER_PIN, LOW); // set the output to low
}

#endif

#ifndef IS_BUZZER		


static void _tonenew(uint8_t _divisor, uint8_t _prescaler,uint16_t _delay)
{
	// So, TCCR0B is TCCR0B AND...
	// Setting the prescaler...
	// so we take TCCR0B as it currently stands, I think the ~ makes it zero the prescaler bits first
	// then we OR it with what prescaler bits we want
	// so set prescaler then set the count...

	TCCR0B = (TCCR0B & ~((1<<CS02)|(1<<CS01)|(1<<CS00))) | _prescaler;

	OCR0A = _divisor;		// set count
	OCR0B = _divisor/2;		// set count for duty, so duty = OCR0B/OCR0A
	TCCR0A |= _BV(COM0B0);

	_mydelay(_delay);
	TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00)); // stop the timer
	_mydelay(_delay);
}


#define NOTE_2A		-24
#define NOTE_3A		-12

#define NOTE_4A		0
#define NOTE_4AS	1
#define NOTE_4B		2

#define NOTE_5C		3
#define NOTE_5CS	4
#define NOTE_5D		5
#define NOTE_5DS	6
#define NOTE_5E		7
#define NOTE_5F		8
#define NOTE_5FS	9
#define NOTE_5G		10
#define NOTE_5GS	11
#define NOTE_5A		12
#define NOTE_5AS	13
#define NOTE_5B		14



#define NOTE_5A		12
#define NOTE_5AS	13
#define NOTE_5B		14

#define NOTE_6C		15
#define NOTE_6CS	16
#define NOTE_6D		17
#define NOTE_6DS	18
#define NOTE_6E		19
#define NOTE_6F		20
#define NOTE_6FS	21
#define NOTE_6G		22
#define NOTE_6GS	23
#define NOTE_6A		24
#define NOTE_6AS	25
#define NOTE_6B		26

/***************************************************
 *  play_frequency
 ***************************************************
 *   Play a particular frequency for a duration
 * 
 * 
 * 	
	*
	*  now based on the prescaler, we calculate the count...
	* 
	* so, lets say 440hz.  at 440hz, 
	* Processor clock: 9.6MHz
	* prescaler of N_64
	* so clock becomes 150kHz.  
	* 
	*
 *   
 *   
 */

void play_frequency(uint16_t freq_hz, int _duration) {

	uint8_t _prescaler,prescaler_number;
	uint8_t divisor;

	// Math routines
	// 1200000L converts to (long)1200000


	if (freq_hz <  64) {		// Work out the prescaler
		_prescaler = N_256;
		//prescaler_number = 256;		// maybe i have to calculate the divisor here?
		divisor = 1200000/(256 * freq_hz );
	} else if (freq_hz < 500 ) {
		_prescaler = N_64;
		//prescaler_number = 64;
		divisor = 1200000/(64 * freq_hz );
	} else {
		_prescaler = N_8;
		//prescaler_number = 8;
		divisor = 1200000/(8 * freq_hz );
	}

	
	TCCR0B = (TCCR0B & ~((1<<CS02)|(1<<CS01)|(1<<CS00))) | _prescaler;
	OCR0A = divisor;		// set count
	OCR0B = divisor/2;		// set count for duty, so duty = OCR0B/OCR0A
	TCCR0A |= _BV(COM0B0);

	_mydelay(_duration);
	
	TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00)); // stop the timer
	_mydelay(_duration);	


}

/***************************************************
 *  play_note
 ***************************************************
 *   I am not really sure how this is going to pan out
 *   I'll start by trying to work out what the frequency of the note is
 *   https://pages.mtu.edu/~suits/NoteFreqCalcs.html
 */
void play_note(int _semitone, int _duration) {

	//if (_semitone < 0) led_status(2,-_semitone);
	//else led_status(3,_semitone);

	uint16_t F0 = 440	; 			// frequency of known note, we are talking A4
	uint16_t note_frequency = F0;
	//F0 = 16.35;		// this is C-0

	double a = 1.059463094359;

	if (_semitone < 0) {
		
		for (int i=0;i--;i>_semitone) {
			a = a / a;
		}
		note_frequency = F0 * a;
	} else if (_semitone > 0) {
		note_frequency = F0;
		for (int i=0;i++;i<_semitone) {
			a = a * a;
		}
		note_frequency = F0 * a;
	} else {
		note_frequency = F0;
	}

	

	//note_frequency = 440 + 10*_semitone;


	//float a = 2^(1/12);
	//double a = pow(2,(1/12));

	//double a = 1.059463094359;
	//double a = pow(2,0.08333);
	//float a = 1.059463094359;		// or hard code it...

	//note_frequency = F0 * (a)^(semitonesaway);
	//note_frequency = F0 * pow(a,_semitone);

	//note_frequency = F0 * pow(1.0594,_semitone);
	//note_frequency = pow(a,_semitone);
	//double somethingstrange = pow(a,_semitone);


	play_frequency(note_frequency,_duration);


}

void playtune_a4(void) {

//	play_frequency(1,20);
	//play_frequency(10,20);
//	play_frequency(10000,20);
//	play_frequency((float)100,20);
	//play_note(0, 20);

	play_note(NOTE_5G,1);
	play_note(NOTE_5G,1);
	play_note(NOTE_5A,2);
	play_note(NOTE_5G,2);
	play_note(NOTE_6C,1);
	play_note(NOTE_5B,2);

/*	for	(int n=0;n<10;n+=3) {
		play_note(n,1);

	}
*/

/*	for	(int n=0;n<100;n+=10) {
		play_frequency(440+n,1);
	}
*/

}


/*
 * I think that in future, the following should be reduced into maybe an array of notes and duration.. so
 * NOTE_C5,1,NOTE_G3,2
 * 
*/

void playtune_happybirthday(void){
	_tonenew(95, N_8,1);		//G 5
	_tonenew(95, N_8,1);		//G 5
	_tonenew(84, N_8,2);		//A 5
	_tonenew(95, N_8,2);		//G 5
	_tonenew(71, N_8,1);		//C 6
	_tonenew(75, N_8,2);		//B 5
	_mydelay(4);
	_tonenew(95, N_8,1);		//G 5
	_tonenew(95, N_8,1);		//G 5
	_tonenew(84, N_8,2);		//A 5
	_tonenew(95, N_8,2);		//G 5
	_tonenew(63, N_8,2);		//D 6
	_tonenew(71, N_8,1);		//C 6
	_mydelay(4);
	_tonenew(95, N_8,1);		//G 5
	_tonenew(95, N_8,1);		//G 5
	_tonenew(47, N_8,1);		//G 6
	_tonenew(56, N_8,1);		//E 6
	_tonenew(71, N_8,1);		//C 6
	_tonenew(75, N_8,1);		//B 5
	_tonenew(84, N_8,1);		//A 5
	_mydelay(2);
	_tonenew(53, N_8,1);		//F	6
	_tonenew(53, N_8,1);		//F 6
	_tonenew(56, N_8,1);		//E 6
	_tonenew(71, N_8,1);		//C 6
	_tonenew(63, N_8,2);		//D 6
	_tonenew(71, N_8,1);		//C 6
}

void playtune_nokia(void){
	// E D f# g# 
	// c# B D E
	// B A c# E A
	_tonenew(56, N_8,1);		//E 6
	_tonenew(63, N_8,1);		//D 6
	_tonenew(100, N_8,2);		//FS 5
	_tonenew( 89, N_8,1);		//GS 5

	_tonenew( 67, N_8,1);		//CS 6
	_tonenew( 75, N_8,1);		//B  5
	_tonenew(127, N_8,2);		//D	 5
	_tonenew(113, N_8,1);		//E	 5

	_tonenew( 75, N_8,1);		//B  5
	_tonenew( 84, N_8,1);		//A  5
	_tonenew(134, N_8,2);		//CS 5
	_tonenew(113, N_8,2);		//E	 5
	_tonenew( 84, N_8,2);		//A  5

/*	_tonenew(134, N_8,1);		//CS 5
	_tonenew(127, N_8,1);		//D	 5
	_tonenew( 75, N_8,1);		//B 5
	_tonenew(142, N_8,2);		//E	 5*/

}



#endif






/***************************************************
 *  Play Tones
 ***************************************************
 * 
 */
void _playtones(void){
//	led_status(4,1);

#ifdef IS_BUZZER	
	pinMode(BUZZER_PIN, OUTPUT);

	for	(uint8_t n=0;n<2;n++) {
		for (int i = 0; i < 2; i++) {
			led_on(BUZZER_PIN);
			_delay_ms(30);
			led_off(BUZZER_PIN);
			_delay_ms(10);
		}
		_delay_ms(40);
	}

#else						// Otherwise we are using PWM

	// 9.6MHz internal oscilator... lol, no, i think this is 1.2MHz for the ATTiny13a
	
	_setuptone();			// Set up the attiny to play tones


	//playtune_happybirthday();
	//playtune_nokia();

	playtune_a4();

	stop();
#endif
	//led_off(LED_RED);
}




/*******************************************************************************************************************************
 *  Random Number Generator that probably doesn't work cunt
 *******************************************************************************************************************************/
long countSleep=0;
long countSleepLimit=0;


/***************************************************
 * Random Number Generator 
 ***************************************************
 * 
 */

static uint16_t random_number = 0;
static uint16_t lfsr16_next(uint16_t n) {
    return (n >> 0x01U) ^ (-(n & 0x01U) & 0xB400U);    
}

void random_init(void) {		// fuck the seed?
	pinMode(PB3, INPUT);
	uint8_t Rand1 = analogRead(PB3);
	uint8_t Rand2 = analogRead(PB3);
	random_number = Rand1 + (Rand1<<8);
}

uint16_t _random( uint16_t _min, uint16_t _max) {
	uint16_t _return=0;
guessagain:
	//remainder = dividend % divisor;
	uint16_t _modulus = (_max - _min);
	_return =   lfsr16_next(random_number);
	_return =   _return % _modulus;
	_return += _min;


	if (_return < _min) {
		led_blink(LED_RED,4);
		goto guessagain;
	}
	if (_return > _max){
		led_blink(LED_RED,8);
		goto guessagain;
	}
	return _return;
}

/*******************************************************************************************************************************
 *  Random number generator based I think on fuck.. i forget
 *******************************************************************************************************************************/
uint16_t _randomsystem( uint16_t _min, uint16_t _max) {
	uint16_t _modulus = (_max - _min);
	uint16_t _number = 0;
	
	_number = rand();					// I assume this makes it postiive??

	//_number = AnalogRead(PB0);

	if (_number < 0) {		// lets call this out as an error
		led_blink(LED_RED,4);
		led_blink(LED_RED,4);
		_number = 0 - _number;	// make it positive
	}
	_number =   _number % _modulus;			// we only want the remainder
	_number += _min;						// we add the minimum number to it

	//check_random(_number);					// we check its within the limit
	return _number;
}



//EMPTY_INTERRUPT(ADC_vect)		//Discard adc interrupt	// i'm not sure what this is for?
ISR(WDT_vect) {}				// without this it reboots




/*******************************************************************************************************************************
 *  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP
 *******************************************************************************************************************************/

#define CUNT (1<<WDP3) | (1<<WDP0)

void updateWatchDog(uint8_t c) {
    cli(); // No interrupts; timed sequence

    if (c == (SLEEP_016MS)) WDTCR &= SLEEP_016MS;//~(1<<WDP3)|~(1<<WDP2)|~(1<<WDP1)|~(1<<WDP0);
	else WDTCR |= c;    // Set watchdog timer
		

/*
	WDTCR |= (1<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(1<<WDP0); // 8s
	is setting the timers configuration and 8s is the max Other values are listed:

	16MS   (0<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(0<<WDP0)
	32MS   (0<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(1<<WDP0)
	64MS   (0<<WDP3 )|(0<<WDP2 )|(1<<WDP1)|(0<<WDP0)
	125MS  (0<<WDP3 )|(0<<WDP2 )|(1<<WDP1)|(1<<WDP0)
	250MS  (0<<WDP3 )|(1<<WDP2 )|(0<<WDP1)|(0<<WDP0)
	500MS  (0<<WDP3 )|(1<<WDP2 )|(0<<WDP1)|(1<<WDP0)
	1S     (0<<WDP3 )|(1<<WDP2 )|(1<<WDP1)|(0<<WDP0)
	2S     (0<<WDP3 )|(1<<WDP2 )|(1<<WDP1)|(1<<WDP0)
	4S     (1<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(0<<WDP0)
	8S     (1<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(1<<WDP0)
*/

	//WDTCR |= (1<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(1<<WDP0); // 8s			hard code it... i think thats wrong
	//WDTCR |= (1<<WDP3) | (1<<WDP0);
	//WDTCR |= CUNT;
//	WDTCR |= SLEEP_8SEC;


    WDTCR |= (1 << WDTIE); // Enable watchdog timer interrupts


    sei(); // Enable global interrupts or we never wake
}



void updateOCR(uint8_t var) {
  OCR0A = OCR0B = var;
}

// set system into the sleep state
// system wakes up when watchdog times out
void system_sleep(byte b) {   
	ACSR = ADMUX = ADCSRA = 0;  
	ACSR |= (1 << ACD);                  // Analog comparator off
	ADCSRA &= ~(1<<ADEN);                // switch Analog to Digitalconverter OFF
	PRR |= (1<<PRTIM0) | (1<<PRADC);

	updateWatchDog(SLEEP_8SEC);
		
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
	power_all_disable();
	sleep_enable();
	sleep_bod_disable(); //Disable BOD

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	//sleep_mode();   // go to sleep and wait for interrupt...
	sleep_cpu();  //Do not use sleep_mode(), contains sleep_enable(), sleep_cpu(),sleep_disable()


	sleep_disable();
	power_all_enable();
	//updateWatchDog(SLEEP_016MS); // Set watchdog interrupt cycle to lowest for millis()	is this fucking it ?


	ACSR &= ~(1 << ACD);   // Analog comparator on
	ADCSRA |= (1<<ADEN);   // switch Analog to Digitalconverter ON
	PRR &= ~(1<<PRTIM0) | ~(1<<PRADC);
  
}




/*******************************************************************************************************************************
 *  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP
 *******************************************************************************************************************************/


void setup() {
	// setup random shit
	random_init(); // initialize 16 bit seed

	pinMode(BUZZER_PIN, OUTPUT);

	led_setup();
//	led_blink(LED_RED,2);

	led_status(1,1);
	  
	_playtones();


}


/*******************************************************************************************************************************
 *  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP
 *******************************************************************************************************************************/

void loop() {

	led_blink(LED_GREEN,1);

#ifdef BEEP_EVERY_CYCLE
	countSleep = countSleepLimit;
#else
	countSleep++;
#endif

	if ( !(countSleep < countSleepLimit) )	{
		if (countSleepLimit != 0) {		// if its not our first run
			_playtones();
		} else {
			//led_status(1,2);
		}

		countSleep = 0;	// reset countsleep
#ifdef FIXED_INTERVAL
		countSleepLimit = FIXED_INTERVAL
#else
		countSleepLimit = _random( RANDOM_SLEEP_MIN*SLEEP_FACTOR, RANDOM_SLEEP_MAX*SLEEP_FACTOR + 1) ;
#endif
		//check_random(countSleepLimit);
	}

	system_sleep(SLEEP_8SEC);
	
}