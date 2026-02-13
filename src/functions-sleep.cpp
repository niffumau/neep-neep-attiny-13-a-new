/**
 * @file functions-sleep.cpp
 * @brief Implements sleep and watchdog timer control for AVR-based Arduino systems.
 *
 * This module provides functionality to enter power-down sleep modes and
 * configure the watchdog timer for timed wakeups.
 */

////// Includes //////////
#include "Arduino.h"
#include <avr/power.h>
#include <avr/sleep.h>
#include "functions-sleep.h"

/// @brief  Minimum random sleep duration in minutes
/// It is in minutes because it is timesed by SLEEP_FACTOR, which is 7 and turns out to around 7x8 seconds
/// @note this was set to 7 seconds now 
uint16_t RANDOM_SLEEP_MIN = 1;

/// @brief  Maximum random sleep duration in minutes (scaled by SLEEP_FACTOR).
/// Because SLEEP_FACTOR is 7, this means its essentually in minutes.
/// 2 is very regular
/// was 30... 4 seemed .. ok but regular, 16 seems maybe too long?
uint16_t RANDOM_SLEEP_MAX = 8;



/*******************************************************************************************************************************
 *  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP  NEW SLEEP
 *******************************************************************************************************************************/

#define CUNT (1<<WDP3) | (1<<WDP0)			///< Not sure why but this apparently made me angry?

/**
 * @brief Updates the watchdog timer configuration.
 *
 * Sets the watchdog prescaler bits and enables the interrupt used for timed wakeups.
 * 
 * @param c Watchdog prescaler configuration bits. Example values include:
 * - `(0<<WDP3)|(0<<WDP2)|(0<<WDP1)|(0<<WDP0)` for 16 ms
 * - `(1<<WDP3)|(0<<WDP2)|(0<<WDP1)|(1<<WDP0)` for 8 s
 *
 * @note The global interrupt flag is cleared during configuration and restored afterward.
 *
 * Example:
 * @code
 * updateWatchDog(SLEEP_8SEC);
 * @endcode
 */
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


/**
 * @brief Puts the system into power-down sleep mode until the watchdog interrupt fires.
 *
 * Disables analog subsystems and peripheral clocks to minimize consumption, sets
 * the power-down sleep mode, and re-enables everything on wake.
 *
 * @param b (Unused) Reserved for future expansion.
 *
 * Example:
 * @code
 * system_sleep(0);
 * @endcode
 *
 * @note The watchdog interrupt must be configured before calling this.
 */
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