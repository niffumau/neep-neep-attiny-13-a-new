/**
 * @file main.cpp
 * @brief Main application for ATtiny13A watchdog‑based low‑power notifier.
 *
 * This file implements:
 * - A low‑power ATtiny13A program that spends most of its time sleeping
 *   and wakes periodically via the watchdog timer.
 * - Notification behaviour using either:
 *   - A physical buzzer or smoke‑alarm‑style output, or
 *   - PWM‑driven tones (via Timer0) using predefined melodies.
 * - LED status feedback during startup and melody selection.
 *
 * The device alternates between short bursts of activity (tones / LED blinks)
 * and long sleep intervals (configurable via #define or randomization). Power
 * is kept very low during sleep; measured draw is approximately:
 * - 27.8 mA maximum when the buzzer is active and sounding,
 * - around 10 mA during wake‑up bursts,
 * - less than 10 mA when actually asleep.
 *
 * @note
 *   - Uses watchdog‑interrupt‑based `system_sleep(SLEEP_8SEC)` to enter low‑power
 *     mode and wake the device periodically.
 *   - The interval between tone notifications is controlled by either:
 *     - `FIXED_INTERVAL` (if defined), or
 *     - a randomized range scaled by `RANDOM_SLEEP_MIN`, `RANDOM_SLEEP_MAX`
 *       and `SLEEP_FACTOR`.
 *   - Melody selection is handled in `_playtones()` using `tunes.h`.
 *
 * @see main.h, functions‑sleep.h, functions‑led.h, functions.h, tunes.h
 */


#include <Arduino.h>
#include <avr/io.h>
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <avr/interrupt.h>

//#include <avr/power.h>
//#include <avr/sleep.h>

//#include <math.h>		// fuck me this can use up a lot of flash
//#include <avr/wdt.h>

#ifndef ARDUINO
#include <stdint.h>
#include "millionUtil.h"         //not needed if compiled with Arduino & Arduino-Tiny
#endif

#include "main.h"
#include "functions-sleep.h"
#include "functions-led.h"
#include "functions.h"
#include "tunes.h"


long countSleep=0;
long countSleepLimit=0;


/*******************************************************************************************************************************
 *  Tone Functions
 *******************************************************************************************************************************/
/***************************************************
 *  
 ***************************************************
 * 
 */
#if !defined(IS_BUZZER) && !defined(IS_SMOKE_ALARM)
/**
 * @brief Initializes the tone generation system by configuring the buzzer output
 *        pin and setting up Timer0 for Fast PWM mode.
 *
 * This function prepares the microcontroller for tone (sound) generation
 * by configuring the relevant timer registers and output pin. It:
 * - Sets the buzzer pin as an output.
 * - Configures Timer0 for Fast PWM mode, where the TOP value is defined by OCR0A.
 * - Sets the compare output mode to control whether the PWM output is inverted
 *   or non-inverted.
 *
 * The function relies on `TCCR0A`, `TCCR0B`, and related control bits to
 * define Fast PWM behavior. Specifically:
 * - `WGM00`, `WGM01`, and `WGM02` collectively select Fast PWM mode using OCR0A as TOP.
 * - `COM0B1` and `COM0B0` control the output compare mode of the OC0B pin.
 *
 * @details
 * This implementation references insights from community discussions on
 * configuring Fast PWM mode on ATtiny13A:
 * https://forum.arduino.cc/t/attiny13a-fast-pwm/855861
 *
 * @note When `WGM02`, `WGM01`, and `WGM00` are set, Timer0 counts from 0 to OCR0A
 *       (defining PWM frequency), and the duty cycle is determined by OCR0B.
 *
 * @warning Improper configuration of `COM0B1`/`COM0B0` or `WGM` bits may lead to
 *          incorrect PWM output or unintended timer behavior.
 *
 * @see stop(), digitalWrite(), pinMode(), TCCR0A, TCCR0B
 */
static void _setuptone(void){

	pinMode(BUZZER_PIN, OUTPUT); /**< Configure the buzzer output pin. */


	// Set Timer Mode to Fast PWM
	// Below, it looks like they are just setting WGM01 to 1,, the manual seems to say that WGM01 and WGM00 should be 1 ?



	// fucking really doesn't make sense

	// notes https://forum.arduino.cc/t/attiny13a-fast-pwm/855861
	/* When WGM02, WGM01, WGM00 is set, the TOP value of the timer is controlled by OCRA.
	That is, the timer count increments from 0 to the value of OCR2A and returns to 0.
	This allows you to control the frequency, but OCRB must be used for waveform output.
	The OCRB value must be between 0 and OCRA.
	Because it is the PWM duty. */

/*
	// Following that
	TCCR0A |= ((1<<WGM02)|(1<<WGM01)|(1<<WGM00));		// this should really be mode 7
	TCCR0A |= ((1<<WGM01)|(1<<WGM00)); // set fast PWM mode 7 - page 79 - uses OCR0A for TOP, PWM signal comes out on OCR0B
	TCCR0A |= (1<<COM0B1)|(1<<COM0B0); // define inverted
*/
	//	TCCR0A |= _BV(COM0B0); // connect PWM pin to Channel A of Timer0... PB1 ?

	/**
     * Configure Timer0 for Fast PWM mode using OCR0A as TOP and
     * enable output compare settings for the buzzer pin.
     */
	TCCR0A |= ((1<<WGM02)|(1<<WGM01)|(1<<WGM00)|(1<<COM0B1)|(1<<COM0B0)|_BV(COM0B0));	
	TCCR0B |= (1<<WGM02); // Use OCR0A as the TOP value instead of 0xFF

}
#endif

/***************************************************
 *  Stop tones
 ***************************************************/

/**
 * @brief Stops tone generation and halts the timer responsible for PWM output.
 *
 * This function disables the timer registers controlling tone generation
 * on Timer0. It clears the prescaler bits in TCCR0B to stop the timer entirely
 * and resets TCCR0A to zero to ensure the counter halts. Finally, it drives
 * the buzzer output pin low to guarantee that no signal remains active.
 *
 * The function behaves identically whether compiled for a buzzer or smoke alarm
 * configuration, unless specific preprocessor flags (`IS_BUZZER` or `IS_SMOKE_ALARM`)
 * alter its compilation.
 *
 * @note Disabling TCCR0A is sometimes necessary if clearing only TCCR0B does not
 *       fully stop the counter, depending on the microcontroller or previous timer setup.
 *
 * @warning If further hardware modules share Timer0, this call will also affect them.
 *
 * @see digitalWrite, TCCR0B, TCCR0A
 */
#if !defined(IS_BUZZER) && !defined(IS_SMOKE_ALARM)
static void stop(void)
{
	TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00)); // stop the timer.... This should absoloutly stop the timer
  	TCCR0A = 0; // stop the counter    // fuck knows why... fuck knos why the other one dind't work				////////////////disable this if it stops working

	digitalWrite(BUZZER_PIN, LOW); // set the output to low
}
#else	
static void stop(void)
{
	TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00)); // stop the timer.... This should absoloutly stop the timer
  	TCCR0A = 0; // stop the counter    // fuck knows why... fuck knos why the other one dind't work				////////////////disable this if it stops working
	digitalWrite(BUZZER_PIN, LOW); // set the output to low	
}
#endif






//EMPTY_INTERRUPT(ADC_vect)		//Discard adc interrupt	// i'm not sure what this is for?
ISR(WDT_vect) {}				// without this it reboots





/***************************************************
 *  Play Tones
 ***************************************************
 * 
 */

/**
 * @brief Plays notification tones based on device configuration and random selection.
 *
 * This function handles audio feedback for different hardware configurations:
 * - **Buzzer/Smoke Alarm**: Simple LED blink patterns for audible feedback
 * - **PWM Tone Capable**: Plays one of several predefined melodies randomly selected
 *
 * The function first checks preprocessor defines to determine the hardware type,
 * then executes the appropriate tone/feedback sequence followed by a cleanup call
 * to `stop()`.
 *
 * @details
 * **Buzzer Mode (`IS_BUZZER` or `IS_SMOKE_ALARM`)**:
 * - Configures buzzer pin as output
 * - Smoke alarm: Single 50ms pulse
 * - Buzzer: Double blink pattern (2ms × 2, 40ms pause) repeated twice
 *
 * **PWM Mode** (default):
 * - Calls `_setuptone()` to configure Timer0 for PWM
 * - Selects random melody (0-6): Nokia (0-1), iPhone (2-4), SMS (5-6)
 * - Plays melody using `playtune_melody_new()` with predefined tune data
 * - Calls `stop()` to silence and reset timer
 *
 * @note Random selection uses `_random(0, 7)` for 7 possible outcomes.
 * @note LED status indicators show melody selection for debugging.
 *
 * @see _setuptone(), stop(), playtune_melody_new(), led_on(), led_off()
 */
void _playtones(void){
//	led_status(4,1);

#if defined(IS_BUZZER) || defined(IS_SMOKE_ALARM)
	pinMode(BUZZER_PIN, OUTPUT);	/**< Configure buzzer pin for output. */

	#if defined(IS_SMOKE_ALARM)
		led_on(BUZZER_PIN);			 /**< Smoke alarm: Single pulse pattern. */
		_delay_ms(50);
		led_off(BUZZER_PIN);
	#else
		/* Buzzer: Double-blink pattern repeated twice */
		for	(uint8_t n=0;n<2;n++) {
			for (int i = 0; i < 2; i++) {
				led_on(BUZZER_PIN);		/**< Blink pattern: 30ms ON, 10ms OFF. */
				_delay_ms(30);
				led_off(BUZZER_PIN);
				_delay_ms(10);
			}
			_delay_ms(40);				/**< Pause between pattern repeats. */
		}
	#endif

#else						/**< PWM tone generation mode */
	_setuptone();			/**< Initialize Timer0 for PWM tone output. */


	uint16_t decision = _random( 0, 7);		/**< Select random melody (0-6). */
	led_status(3,decision);					/**< Debug: Show selected melody via LED. */

/*	if (decision < 2 ) playtune_melody(tune_nokia,sizeof(tune_nokia)/2);
	//else if (decision < 5) playtune_melody(tune_happybirthday,sizeof(tune_happybirthday)/2);
	else if (decision < 5) playtune_melody(tune_iphone,sizeof(tune_iphone)/2);
	else playtune_melody(tune_sms,sizeof(tune_sms)/2);*/

	/* Melody selection logic: Nokia(0-1), iPhone(2-4), SMS(5-6) */
	if (decision < 2 ) 
		playtune_melody_new(tune_nokia_new);		///< Nokia tune. 
	else if (decision < 5) 
		playtune_melody_new(tune_iphone_new);		///< iPhone tune. 
	else 
		playtune_melody_new(tune_sms_new);			///< SMS tune. 

	stop();											///< Stop the tones, ie Silence and reset timer

#endif

}



/*******************************************************************************************************************************
 *  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP  SETUP
 *******************************************************************************************************************************/

const uint8_t _checkvariable PROGMEM = 5;


void setup() {
	random_init(); 					///< initialize 16 bit seed
	pinMode(BUZZER_PIN, OUTPUT);	///< Set BUZZER_PIN mode to OUTPUT
	led_setup();					///< Setup the LED
	led_status(1,1);				///< blink the Red Led once then the green LED Once
	_playtones();					///< Play some tones at the end of the setup() function
	stop();							///< Stop all tones
}


/*******************************************************************************************************************************
 *  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP  LOOP
 *******************************************************************************************************************************/

 /**
 * @brief Main loop implementing randomized sleep/beep cycles for low-power operation.
 *
 * Executes continuously after `setup()`. Each iteration:
 * 1. Blinks green status LED
 * 2. Increments sleep counter (`countSleep`) unless `BEEP_EVERY_CYCLE` forces immediate beeps
 * 3. When `countSleep >= countSleepLimit`:
 *    - Plays notification tones (except first run)
 *    - Resets counter and picks new random interval
 * 4. Sleeps for exactly 8 seconds via `system_sleep(SLEEP_8SEC)`
 *
 * **Sleep timing calculation:**
 * ```
 * Total sleep = random(RANDOM_SLEEP_MIN×SLEEP_FACTOR, RANDOM_SLEEP_MAX×SLEEP_FACTOR+1) × 8 seconds
 * Example: SLEEP_FACTOR=10 → 80s to 720s (1.3-12 minutes) intervals
 * ```
 *
 * @details
 * The convoluted `if (!(countSleep < countSleepLimit))` is equivalent to `if (countSleep >= countSleepLimit)`.
 * 
 * **First run behavior:** `countSleepLimit=0` initially, so no tones play until after first sleep cycle.
 *
 * **Configuration modes:**
 * | Mode | Behavior |
 * |------|----------|
 * | `BEEP_EVERY_CYCLE` | Beeps every wake-up (no counting) |
 * | `FIXED_INTERVAL` | Fixed sleep cycles between beeps |
 * | Default | Random intervals via `SLEEP_FACTOR` scaling |
 *
 * @warning Each `system_sleep(SLEEP_8SEC)` blocks for **exactly 8 seconds** regardless of active time.
 * Total cycle time = (sleep cycles × 8s) + (~2s melody time).
 *
 * @see system_sleep(), _playtones(), _random(), led_blink()
 */
void loop() {

	led_blink(LED_GREEN,1);				///< Every time around the loop, blink the green LED once.

#ifdef BEEP_EVERY_CYCLE
	countSleep = countSleepLimit;		///< If BEEP_EVERY_CYCLE defined, beep every cycle...
#else
	countSleep++;
#endif

	if ( !(countSleep < countSleepLimit) )	{
		if (countSleepLimit != 0) {		///< if its not our first run
			_playtones();				///< Play tones
		} else {
			//led_status(1,2);			
		}

		countSleep = 0;					///< reset countsleep
#ifdef FIXED_INTERVAL
		countSleepLimit = FIXED_INTERVAL
#else
		countSleepLimit = _random( RANDOM_SLEEP_MIN*SLEEP_FACTOR, 
									RANDOM_SLEEP_MAX*SLEEP_FACTOR + 1) ;
									///< New Random Interval: [min×factor, max×factor+1] cycles
#endif
		//check_random(countSleepLimit); ///< Debug: This is just a debug to check the random count
	}

	system_sleep(SLEEP_8SEC);			///< Sleep 8s, wake via WDT interrupt
	
}