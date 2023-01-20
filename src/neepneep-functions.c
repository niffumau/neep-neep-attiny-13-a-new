

////// Includes //////////
#include "Arduino.h"

#include <neeppeep-functions.h>>
//#include <attiny13a-sleep.h>
//#include <sleepfunction.h>

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
/*
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
*/
/*
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
}*/


/*
 * I think that in future, the following should be reduced into maybe an array of notes and duration.. so
 * NOTE_C5,1,NOTE_G3,2
 * 
*/

/*
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
*/
/*
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

}
*/

