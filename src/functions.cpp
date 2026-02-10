////// Includes //////////
#include "Arduino.h"

//#include <avr/io.h>
//#define __DELAY_BACKWARD_COMPATIBLE__
//#include <util/delay.h>

//#include <avr/interrupt.h>
//#include <avr/pgmspace.h>

//#include <avr/interrupt.h>

//#include <avr/power.h>
//#include <avr/sleep.h>


#include "main.h"
#include "functions.h"
#include "functions-led.h"
#include "functions-sleep.h"




/*******************************************************************************************************************************
 *  Delay Routine
 *******************************************************************************************************************************/

/**
 * @brief Provides a coarse-grained delay using repeated 40ms blocks.
 *
 * Creates a total delay of `_delay × 40ms` by calling `_delay_ms(40)` in a loop.
 * Useful for non-critical timing where precision < 40ms is acceptable.
 *
 * @param[in] _delay Number of 40ms blocks to execute (0-255)
 * @note Total delay range: 0ms to 10.2 seconds (255 × 40ms)
 * @note Uses blocking `_delay_ms()` - interrupts still execute during delay.
 *
 * @see _delay_ms()
 */
void _mydelay(uint8_t _delay) {		
	for (uint8_t i=0;i < _delay;i++ ){ 		/**< Repeat `_delay` times. */
		_delay_ms(40);						/**< 40ms blocking delay per iteration. */
	}

}


/*******************************************************************************************************************************
 *  Random Number Generator
 *******************************************************************************************************************************/

/***************************************************
 * Random Number Generator 
 ***************************************************
 * 
 */

/** 
 * @brief Global state variable for the LFSR random number generator.
 */
static uint16_t random_number = 0;

/**
 * @brief Linear Feedback Shift Register (LFSR) 16-bit pseudo-random generator.
 * 
 * Generates the next 16-bit pseudo-random value using a primitive polynomial.
 * Uses the characteristic polynomial `x^16 + x^14 + x^13 + x^11 + 1` (0xB400).
 *
 * @param[in] n Current 16-bit LFSR state
 * @return uint16_t Next pseudo-random 16-bit value
 *
 * @see _random()
 */
static uint16_t lfsr16_next(uint16_t n) {
    return (n >> 0x01U) ^ (-(n & 0x01U) & 0xB400U);    
}

/**
 * @brief Initializes the random number generator with entropy from analog pin.
 *
 * Reads analog noise from `PIN_RANDOM` pin twice to create a 16-bit seed value.
 * No traditional software seeding - relies on hardware noise for entropy.
 *
 * @note Called automatically by `_random()` - no manual initialization needed.
 * @note Comment "fuck the seed?" suggests deliberate avoidance of srand()/seed().
 *
 * @see _random()
 */
void random_init(void) {
	pinMode(PIN_RANDOM, INPUT);			/**< Configure random pin as input. */
	uint8_t Rand1 = analogRead((analog_pin_t) PIN_RANDOM);	 /**< First entropy sample. */
	uint8_t Rand2 = analogRead((analog_pin_t) PIN_RANDOM);	/**< Second entropy sample. */
	random_number = Rand1 + (Rand2<<8);						/**< Combine into 16-bit seed. */
}

/**
 * @brief Generates a pseudo-random number within specified bounds using LFSR.
 *
 * Provides uniform distribution within `[_min, _max]` range using LFSR primitive
 * polynomial and modulus operation. Re-seeds from analog noise on every call.
 * Uses `goto` retry logic to ensure result stays within bounds.
 *
 * @param[in] _min Minimum return value (inclusive)
 * @param[in] _max Maximum return value (inclusive) 
 * @return uint16_t Random number in range `[_min, _max]`
 *
 * @details Algorithm:
 *  1. Re-initialize seed from analog noise
 *  2. Generate LFSR value: `_return = lfsr16_next(random_number)`
 *  3. Scale to range: `_return = (_return % (_max - _min)) + _min`
 *  4. Retry (`goto guessagain`) if result out of bounds
 *
 * @note Uses `goto` for tight retry loop - uncommon but effective here.
 * @warning Re-seeding every call may reduce randomness quality for rapid calls.
 *
 * @see random_init(), lfsr16_next()
 */
uint16_t _random( uint16_t _min, uint16_t _max) {
	random_init();		///< Re-seed from analog noise every call. 
	uint16_t _return=0;
guessagain:;
	//remainder = dividend % divisor;
	uint16_t _modulus = (_max - _min);			/**< Range size for modulus. */
	_return =   lfsr16_next(random_number);		/**< Generate LFSR pseudo-random value. */
	_return =   _return % _modulus;				/**< Scale to [0, modulus). */
	_return += _min;							/**< Shift to [_min, _max]. */

	if (_return < _min) {						/**< Reject: below range. */
		//led_blink(LED_RED,4);
		goto guessagain;
	}
	if (_return > _max){						/**< Reject: above range. */
		//led_blink(LED_RED,8);
		goto guessagain;
	}

	//led_status(3,_return);			// debug for when it doesn't look random anymore

	return _return;						/**< Valid random value. */
}


/***************************************************
 * check_random 
 ***************************************************
 * Check that the random number is between our limits
 */
void check_random(uint16_t _return) {
	if (_return < RANDOM_SLEEP_MIN*8) {
		led_status(4,2);
	}
	if (_return > RANDOM_SLEEP_MAX*8 + 2){
		led_status(4,4);
	}
}






