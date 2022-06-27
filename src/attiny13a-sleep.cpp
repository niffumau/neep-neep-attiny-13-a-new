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
#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <avr/interrupt.h>

#include <avr/power.h>
#include <avr/sleep.h>
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

int notes[12]={239,225,213,201,190,179,169,159,150,142,134,127};
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

// these all need to be multiplied by 8 because the clock is 8x faster
//24 bytes per octave
PROGMEM const octave_t octaves[6] = {
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
	}/*,
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
	}*/
};



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

#ifndef IS_BUZZER		
/***************************************************
 *  _tone
 ***************************************************
 *   I really need to break down this function
 */

static void _tone(uint8_t octave, uint8_t note)
{
	uint8_t OCR0A_value, OCR0B_value, _duty;

	uint32_t ret;
	note_t *val;
	ret = pgm_read_word_near((uint8_t *)&octaves + sizeof(octave_t) * octave + sizeof(note_t) * note);
	val = (note_t *)&ret;



	// https://electronics.stackexchange.com/questions/387383/attiny13a-cant-generate-software-pwm-with-ctc-mode



	// So, TCCR0B is TCCR0B AND...
	// Setting the prescaler...
	// so we take TCCR0B as it currently stands, I think the ~ makes it zero the prescaler bits first
	// then we OR it with what prescaler bits we want
	// so set prescaler then set the count...


	TCCR0B = (TCCR0B & ~((1<<CS02)|(1<<CS01)|(1<<CS00))) | val->N;
	//TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00));		// Clear the Timer.... probalby n ot needed
	//TCCR0B |= val->N;

//	TCCR0B = (TCCR0B & ~((1<<CS02)|(1<<CS01)|(1<<CS00))) | N_8;		// Set Prescaler	/// not sure about this one

	OCR0A_value = val->OCRxn - 1;

	uint16_t _temp;
	_duty = 50;
	//_temp = (OCR0A_value * _duty)/100;
	//OCR0B_value =	_temp;				// _duty is the duty cycle out of 255.
	//OCR0B_value = OCR0A_value/2;		// I think this is 50% duty cycle ??
	OCR0B_value = (OCR0A_value * _duty )/100;

	OCR0A = OCR0A_value;		// set count
	OCR0B = OCR0B_value;		// set count for duty, so duty = OCR0B/OCR0A


	



}

static void _tonenew(uint8_t _count, uint8_t _prescaler,uint16_t _delay)
{
	uint8_t OCR0A_value, OCR0B_value, _duty;

	// So, TCCR0B is TCCR0B AND...
	// Setting the prescaler...
	// so we take TCCR0B as it currently stands, I think the ~ makes it zero the prescaler bits first
	// then we OR it with what prescaler bits we want
	// so set prescaler then set the count...


	TCCR0B = (TCCR0B & ~((1<<CS02)|(1<<CS01)|(1<<CS00))) | _prescaler;


	//TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00));		// Clear the Timer.... probalby n ot needed
	//TCCR0B |= val->N;

//	TCCR0B = (TCCR0B & ~((1<<CS02)|(1<<CS01)|(1<<CS00))) | N_8;		// Set Prescaler	/// not sure about this one

	OCR0A_value = _count;

	uint16_t _temp;
	_duty = 50;
	//_temp = (OCR0A_value * _duty)/100;
	//OCR0B_value =	_temp;				// _duty is the duty cycle out of 255.
	OCR0B_value = OCR0A_value/2;		// I think this is 50% duty cycle ??
	//OCR0B_value = (OCR0A_value * _duty )/100;

	OCR0A = OCR0A_value;		// set count
	OCR0B = OCR0B_value;		// set count for duty, so duty = OCR0B/OCR0A


	

	_delay_ms(_delay);

}

#endif

static void _tone_OLD(uint8_t octave, uint8_t note)
{
	uint8_t OCR0A_value, OCR0B_value, _duty;

	uint32_t ret;
	note_t *val;
	ret = pgm_read_word_near((uint8_t *)&octaves + sizeof(octave_t) * octave + sizeof(note_t) * note);
	val = (note_t *)&ret;

// notes https://forum.arduino.cc/t/attiny13a-fast-pwm/855861
/* When WGM02, WGM01, WGM00 is set, the TOP value of the timer is controlled by OCRA.
That is, the timer count increments from 0 to the value of OCR2A and returns to 0.
This allows you to control the frequency, but OCRB must be used for waveform output.
The OCRB value must be between 0 and OCRA.
Because it is the PWM duty. */

	// Following that
//	TCCR0A |= ((1<<WGM01)|(1<<WGM00)); // set fast PWM mode 7 - page 79 - uses OCR0A for TOP, PWM signal comes out on OCR0B
//	TCCR0B |= (1<<WGM02); // to use TOP as OCR0A rather than 0xFF
//	TCCR0A |= (1<<COM0B1)|(1<<COM0B0); // define inverted

	// https://electronics.stackexchange.com/questions/387383/attiny13a-cant-generate-software-pwm-with-ctc-mode



	// So, TCCR0B is TCCR0B AND...
	// Setting the prescaler...
	// so we take TCCR0B as it currently stands, I think the ~ makes it zero the prescaler bits first
	// then we OR it with what prescaler bits we want
	// so set prescaler then set the count...


	//TCCR0B = (TCCR0B & ~((1<<CS02)|(1<<CS01)|(1<<CS00))) | val->N;
	TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00));		// Clear the Timer.... probalby n ot needed
	TCCR0B |= val->N;

//	TCCR0B = (TCCR0B & ~((1<<CS02)|(1<<CS01)|(1<<CS00))) | N_8;		// Set Prescaler	/// not sure about this one




  	OCR0A = val->OCRxn - 1; // set the OCRnx
	//OCR0A_value = val->OCRxn - 1;


	uint16_t _temp;
	_duty = 50;
	//_temp = (OCR0A_value * _duty)/100;
	//OCR0B_value =	_temp;				// _duty is the duty cycle out of 255.
	//OCR0B_value = OCR0A_value/2;		// I think this is 50% duty cycle ??
	//OCR0B_value = (OCR0A * _duty )/100;

	//OCR0A = OCR0A_value;
	OCR0B = (OCR0A * _duty )/100;
	//OCR0B = OCR0B_value;


	// Can I use OCR0B to create the duty cycle???
	// so i beleive i can probably set OCR0B to half and get 50%? might need to scope this

	// so i belive i set 
	
	// OCR0A =  	// set count 
	// OCR0B = 	// set count for duty, so duty = OCR0B/OCR0A

	
	
	//OCR0A_value = val->OCRxn - 1;
	//OCR0A = OCR0A_value;							// set TOP for PWM frequency
	//OCR0B = OCR0B_value1;							// controls the PWM


}


/*
static void _tone_250(uint8_t octave, uint8_t note)
{
	uint8_t OCR0A_value, OCR0B_value, _duty;

	uint32_t ret;
	note_t *val;

	ret = pgm_read_word_near((uint8_t *)&octaves + sizeof(octave_t) * octave + sizeof(note_t) * note);
	val = (note_t *)&ret;



	TCCR0A |= ((1<<WGM01)|(1<<WGM00)); // set fast PWM mode 7 - page 79 - uses OCR0A for TOP, PWM signal comes out on OCR0B
	TCCR0B |= (1<<WGM02); // to use TOP as OCR0A rather than 0xFF
	TCCR0A |= (1<<COM0B1)|(1<<COM0B0); // define inverted

	TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00));		// Clear the Timer.... probalby n ot needed
	TCCR0B |= val->N;								// Set the timer value



}*/

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
//  	TCCR0A = 0; // stop the counter    // fuck knows why... fuck knos why the other one dind't work				////////////////disable this if it stops working
	//pinMode(BUZZER_PIN, OUTPUT);

//	TCCR0A |= _BV(COM0B0);	//////////maybe this ???
	TCCR0A |= ~(1<<COM0B0);

	digitalWrite(BUZZER_PIN, LOW); // set the output to low
}

#endif

/*
void _playchirp(void){
	_setuptone();
	_tonenew(247, N_64,4000);
	stop();
}*/


/***************************************************
 *  Play Tones
 ***************************************************
 * 
 */
void _playtones(void){
//	led_status(4,1);

#ifdef IS_BUZZER	
	pinMode(BUZZER_PIN, OUTPUT);
	led_on(BUZZER_PIN);
	_delay_ms(100);
	led_off(BUZZER_PIN);
	_delay_ms(100);
	led_on(BUZZER_PIN);
	_delay_ms(100);
	led_off(BUZZER_PIN);
#else						// Otherwise we are using PWM

	// 9.6MHz internal oscilator...

	// test long tone...
	_setuptone();			// Set up the attiny to play tones

	// Checking frequencies
	//for (int i = 10;)

//	_tonenew(100, N_1024,4000);	//		// doesn't work very well
//	_tonenew(200, N_1024,4000);	//		// doesn't work very well

	
	
	//_tonenew(247, N_64,4000);	//	2972
	//_tonenew(246, N_64,4000);	//	2972
	//_tonenew(244, N_64,4000);	//	3015
	//_tonenew(240, N_64,4000);	//	3058 	

	//_tonenew(128, N_256,4000);	//	
	//_tonenew(200, N_256,4000);	//		
	//_tonenew(notes[0], N_8,4000);

	//_tonenew(72, N_8,4000);

/*
	for (uint8_t i = 0;i<12;i++) {
		_tonenew(notes[i], N_64,400);
	}
*/
/*
	for (uint8_t i = 250;i<255;i-=20) {
		_tonenew(i, N_64,400);
	}
/*

	for (uint8_t i = 250;i<255;i-=20) {
		_tonenew(i, N_256,400);
	}
*/

/*
	for (int j=7;j>0;j--) {
		for (int i=11;i>0;i--) {
			_tone(i, j);	//note_C 
			_delay_ms(400);
		}
		_delay_ms(500);
	}*/


/*	// this one works great
	for (int i = 3; i < 5; ++i) {
		led_blink(LED_GREEN,i+1);
		for (int j = 0; j < 12; ++j) {
			_tone(i, j);
			_delay_ms(80);
		}
	}*/

	for (uint8_t g=0;g<2;g++) {
		_setuptone();			// Set up the attiny to play tones
		for	(uint8_t n=0;n<CHIRPS;n++) {
			// new one thats faster
			for (uint8_t j = 4; j < 5; ++j) {
				//led_blink(LED_GREEN,i+1);
				for (int i = 0; i < 12; i += 4) {
					_tone(j, i);
					_delay_ms(40);
				}
				//_delay_ms(1000);
			}
			//_delay_ms(100);
		}
		stop();
		_delay_ms(200);
	}

	/*
	_tone(4, 7);	//note_C 
	_delay_ms(100);
	_tone(3, 6);	//
	_delay_ms(100);
	_tone(3, 3);	//
	_delay_ms(100);*/
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

//#define USE_RANDOM_SEED

#ifdef    USE_RANDOM_SEED
#define    RANDOM_SEED_ADDRESS    0x00
#endif    /* !USE_RANDOM_SEED */

static uint16_t random_number = 0;
static uint16_t lfsr16_next(uint16_t n) {
    return (n >> 0x01U) ^ (-(n & 0x01U) & 0xB400U);    
}

void random_init(uint16_t seed) {
#ifdef USE_RANDOM_SEED
    random_number = lfsr16_next(eeprom_read_word((uint16_t *)RANDOM_SEED_ADDRESS) ^ seed);
    eeprom_write_word((uint16_t *)0, random_number);
#else
    random_number = seed;
#endif    /* !USE_RANDOM_SEED */
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
	random_init(0xabcd); // initialize 16 bit seed

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