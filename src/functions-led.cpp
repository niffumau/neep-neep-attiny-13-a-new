
#include <Arduino.h>
#include <avr/io.h>
#define __DELAY_BACKWARD_COMPATIBLE__


#include "main.h"
#include "functions-led.h"


/*******************************************************************************************************************************
 *  LED Functions
 *******************************************************************************************************************************/

/***************************************************
 *  led_setup
 ***************************************************
 * 
 */

/**
 * @brief Initializes LED pins and sets Port B to low state.
 *
 * Configures RED and GREEN LED pins as outputs and clears all Port B pins to LOW.
 * Ensures clean LED state on startup.
 *
 * @note I'm not sure if this PORTB = 0b00000... is a good idea, shoudln't i write to only both led pins?
 * @see PORTB, pinMode()
 */
void led_setup(void){
  pinMode(LED_RED, OUTPUT);			///< Configure red LED as output. 
  pinMode(LED_GREEN, OUTPUT);		///< Configure green LED as output. 
  //DDRB |= (1 << LED_PIN);	// set as output
  //DDRB |= (1 << LED_PIN2);	// set as output
  //PORTB = 0b00000000;				///< Clear all Port B pins to LOW. 
  //// I did have these two orignally	
  //digitalWrite(LED_PIN, LOW); 
  //digitalWrite(LED_PIN2, LOW); 
  // I commented out the PORTB line in favor of this:
  PORTB &= ~(_BV(LED_RED) | _BV(LED_GREEN)); ///< Clear ONLY LED pins. 

}
/***************************************************
 *  led_toggle
 ***************************************************
 * 
 */

/**
 * @brief Toggles the state of specified LED pin using direct Port B manipulation.
 *
 * Uses bitwise XOR on PORTB with bit position defined by `_PIN`.
 * Faster than `digitalWrite()` for direct port access.
 *
 * @param[in] _PIN Bit position corresponding to LED pin (LED_RED, LED_GREEN)
 *
 * @see _BV(), PORTB
 */
void led_toggle(uint8_t _PIN){
  //PORTB ^= _BV(LED_PIN2);  // toggle LED pin		//_BV returns the byte value of some shit
  PORTB ^= _BV(_PIN);  ///< Toggle specified bit on Port B, ie toggle LED pin. _BV returns the byte value of some shit 
}


/***************************************************
 *  led_on
 ***************************************************
 * 
 */

/**
 * @brief Turns specified LED ON using Arduino digitalWrite.
 *
 * @param[in] _PIN LED pin number (LED_RED, LED_GREEN, BUZZER_PIN, etc.)
 *
 * @see digitalWrite()
 */
void led_on(int _PIN){
	digitalWrite(_PIN, HIGH); 		///< Set LED pin HIGH. 
}

/**
 * @brief Turns specified LED OFF using Arduino digitalWrite.
 *
 * @param[in] _PIN LED pin number (LED_RED, LED_GREEN, BUZZER_PIN, etc.)
 *
 * @see digitalWrite()
 */
void led_off(int _PIN){
  	digitalWrite(_PIN, LOW); 		///< Set LED pin LOW. 
}

/**
 * @brief Blinks specified LED a given number of times.
 *
 * Executes blink pattern: ON(`BLINK_INTERVAL_ON`) → OFF(`BLINK_INTERVAL_OFF`)
 * repeated `_number` times, followed by final pause.
 *
 * @param[in] _PIN LED pin number
 * @param[in] _number Number of blink cycles (0-255)
 *
 * @see BLINK_INTERVAL_ON, BLINK_INTERVAL_OFF, BLINK_INTERVAL_END
 */
void led_blink(int _PIN,int _number) {
	for (uint8_t i=0;i < _number;i++ ){ 
		digitalWrite(_PIN, HIGH);			///< LED ON 
		_delay_ms(BLINK_INTERVAL_ON);		///< ON duration
		digitalWrite(_PIN, LOW);			///< LED Off
		_delay_ms(BLINK_INTERVAL_OFF);		///< Off Duratin
	}
	_delay_ms(BLINK_INTERVAL_END);			///< Final pause after sequence
}

/**
 * @brief Displays status using red/green LED blink pattern.
 *
 * Encodes two values as blink counts: Red=`_first`, Green=`_second`.
 * Used for debugging and status indication.
 *
 * @param[in] _first Number of red LED blinks
 * @param[in] _second Number of green LED blinks
 *
 * @see led_blink(), BLINK_INTERVAL_STATUS_END
 */
void led_status(int _first,int _second) {
	led_blink(LED_RED,_first);					///< Red LED: `_first` blinks.
	led_blink(LED_GREEN,_second);				///< Green LED: `_second` blinks.
	_delay_ms(BLINK_INTERVAL_STATUS_END);		///< Pause between status sequences.
}

