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





//uint8_t mcucr1, mcucr2;

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




#ifndef IS_BUZZER		
/***************************************************
 *  play_note
 ***************************************************
 *   I am not really sure how this is going to pan out
 *   I'll start by trying to work out what the frequency of the note is
 *   https://pages.mtu.edu/~suits/NoteFreqCalcs.html
 */
void play_note(uint8_t _note, uint8_t _duration) {
	uint8_t _prescaler;
	uint8_t divisor;
	
	/*
	if (_note < 0)  {
		led_status(4,2);
	}
	if (_note > 83)  {
		led_status(4,3);
	}*/

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
	_mydelay(_duration);

}



/***************************************************
 *  playtune_melody
 ***************************************************
 *  This should play a tune 
 *   
 */

void playtune_melody(notes_t *melody,uint8_t _size) {

	for	(int i=0; i <_size; i++) {
		play_note(pgm_read_byte(&melody[i].note),pgm_read_byte(&melody[i].duration));
		
	}

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
	uint8_t Rand1 = analogRead((analog_pin_t) PB3);
	uint8_t Rand2 = analogRead((analog_pin_t) PB3);
	//random_number = Rand1 + (Rand1<<8);
	random_number = Rand1 + (Rand2<<8);
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

	
	// pick one tune

	if (_random( 0, 1) == 0 ) playtune_melody(tune_nokia,sizeof(tune_nokia)/2);

	else playtune_melody(tune_happybirthday,sizeof(tune_happybirthday)/2);

	//playtune_melody(tune_nokia,sizeof(tune_nokia)/2);
	//playtune_melody(tune_happybirthday,sizeof(tune_happybirthday)/2);


	
	//stop();
#endif
	//led_off(LED_RED);
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