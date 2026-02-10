#ifndef MAIN_H
#define MAIN_H


// includes
#include <Arduino.h>
#include <avr/io.h>
#include <avr/pgmspace.h>



//#define IS_BUZZER					//< Define if its a buzzer 
//#define IS_SMOKE_ALARM			//< Define if it is suppose to act like a smoke alarm
//#define BEEP_EVERY_CYCLE			//< Define if you want it to beep every cycle



#define DUTY_CYCLE                  50

/**
 * @def FIXED_INTERVAL
 * @brief Sets a constant number of watchdog sleep cycles between tone notifications.
 *
 * When this macro is defined, the main loop uses a fixed sleep interval
 * instead of a random delay range, by assigning `countSleepLimit` to this
 * constant value after each notification cycle. [web:14]
 *
 * @note Each increment of `countSleep` corresponds to one watchdog sleep
 *       period (configured with `system_sleep(SLEEP_8SEC)`), so the actual
 *       elapsed time is `FIXED_INTERVAL` multiplied by the watchdog interval. 
 */
//#define FIXED_INTERVAL              2


/**
 * @defgroup pin_macros Pin Configuration Macros
 * @brief ATtiny GPIO pin assignments used in the project.
 * @{
 */
#define	BUZZER_PIN	                PB1		//< I/O pin connected to the buzzer / alarm output (PB1).
#define LED_RED  	                PB4		//< I/O pin driving the red LED indicator (PB4)
#define LED_GREEN	                PB0 	//< I/O pin driving the green LED indicator (PB0)
#define PIN_RANDOM					PB5		//< ADC / random‑seed input pin (PB5).
/** @} */

/**
 * @defgroup Blink Interval Configuration Macros
 * @brief ATtiny The blink intervals in ms
 * @{
 */
#define BLINK_INTERVAL_ON 			100		///< ms
#define BLINK_INTERVAL_OFF 			100		///< ms
#define BLINK_INTERVAL_END 			200		///< ms
#define BLINK_INTERVAL_STATUS_END	200		///< ms

/// The interval that we count in.
/// @note I had it written here that this should be 8, no idea why?
#define SLEEP_FACTOR 7						





// Check that if FIXED_INTERVAL is actually set, it is set to a value 1 or greater.
#ifdef FIXED_INTERVAL
  #if (FIXED_INTERVAL) < 1
    #error "FIXED_INTERVAL must be at least 1 watchdog sleep cycles"
  #endif
#endif




// I think those are wrong.. (_BV(CS01)|_BV(CS00))



#define	N_1	(_BV(CS00))                 // No Prescaling
#define	N_8	(_BV(CS01))
#define	N_64	(_BV(CS01)|_BV(CS00))
#define	N_256	(_BV(CS02))
#define	N_1024	(_BV(CS02)|_BV(CS00))   // clk/1024 from prescaler


typedef struct s_notes {
	uint8_t	note; 
	uint8_t duration;
} notes_t;




#endif

