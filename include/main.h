#ifndef MAIN_H
#define MAIN_H


// includes
#include <Arduino.h>
#include <avr/io.h>

//#define __DELAY_BACKWARD_COMPATIBLE__
//#include <util/delay.h>

//#include <avr/interrupt.h>
#include <avr/pgmspace.h>

//#include <avr/interrupt.h>

//#include <avr/power.h>
//#include <avr/sleep.h>



//#define IS_BUZZER			// Define if its a buzzer //////////////////////////////////////////////////////////////////////////////
//#define IS_SMOKE_ALARM			// Define if it is suppose to act like a smoke alarm
//#define BEEP_EVERY_CYCLE				////////////////////////////////////////////////////////////////////////////////////////////


#define CHIRPS                      3       // Number of chirps

#define DUTY_CYCLE                  50

//#define FIXED_INTERVAL              2

// pins
#define	BUZZER_PIN	                PB1
#define LED_RED  	                PB4
#define LED_GREEN	                PB0 
#define PIN_RANDOM					PB5

#define BLINK_INTERVAL_ON 			100		//ms
#define BLINK_INTERVAL_OFF 			100		//ms
#define BLINK_INTERVAL_END 			200		//ms
#define BLINK_INTERVAL_STATUS_END	200		//ms


/*
#define BODS 7                   //BOD Sleep bit in MCUCR
#define BODSE 2                  //BOD Sleep enable bit in MCUCR
*/


// RANDOM_SLEEP_MIN = minimum number of 8-sec WDT periods to sleep for
// RANDOM_SLEEP_MAX = maximum number of 8-sec WDT periods to sleep for
//int RANDOM_SLEEP_MIN = 7;     // 1 min (1 * 60 / 8)
//int RANDOM_SLEEP_MAX = 30;    // 4 mins
//int RANDOM_SLEEP_MIN = 225; // 30 mins
//int RANDOM_SLEEP_MAX = 525; // 70 mins
/// multiples of 8 seconds

#define SLEEP_FACTOR 7								// what we count intervals in, should be 8







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

//const uint8_t divisors[];

#define NOTE_0C		(uint8_t) 0
#define NOTE_0CS	(uint8_t) 1
#define NOTE_0D		(uint8_t) 2
#define NOTE_0DS	(uint8_t) 3
#define NOTE_0E		(uint8_t) 4
#define NOTE_0F		(uint8_t) 5
#define NOTE_0FS	(uint8_t) 6
#define NOTE_0G		(uint8_t) 7
#define NOTE_0GS	(uint8_t) 8
#define NOTE_0A		(uint8_t) 9
#define NOTE_0AS	(uint8_t) 10
#define NOTE_0B		(uint8_t) 11

#define NOTE_1C		(uint8_t) 12		
#define NOTE_1CS	(uint8_t) 13
#define NOTE_1D		(uint8_t) 14
#define NOTE_1DS	(uint8_t) 15
#define NOTE_1E		(uint8_t) 16
#define NOTE_1F		(uint8_t) 17
#define NOTE_1FS	(uint8_t) 18
#define NOTE_1G		(uint8_t) 19
#define NOTE_1GS	(uint8_t) 20
#define NOTE_1A		(uint8_t) 21
#define NOTE_1AS	(uint8_t) 22
#define NOTE_1B		(uint8_t) 23

#define NOTE_2C		(uint8_t) 24	
#define NOTE_2CS	(uint8_t) 25
#define NOTE_2D		(uint8_t) 26
#define NOTE_2DS	(uint8_t) 27
#define NOTE_2E		(uint8_t) 28
#define NOTE_2F		(uint8_t) 29
#define NOTE_2FS	(uint8_t) 30
#define NOTE_2G		(uint8_t) 31
#define NOTE_2GS	(uint8_t) 32
#define NOTE_2A		(uint8_t) 33
#define NOTE_2AS	(uint8_t) 34
#define NOTE_2B		(uint8_t) 35

#define NOTE_3C		(uint8_t) 36
#define NOTE_3CS	(uint8_t) 37
#define NOTE_3D		(uint8_t) 38
#define NOTE_3DS	(uint8_t) 39
#define NOTE_3E		(uint8_t) 40
#define NOTE_3F		(uint8_t) 41
#define NOTE_3FS	(uint8_t) 42
#define NOTE_3G		(uint8_t) 43
#define NOTE_3GS	(uint8_t) 44
#define NOTE_3A		(uint8_t) 45
#define NOTE_3AS	(uint8_t) 46
#define NOTE_3B		(uint8_t) 47

#define NOTE_4C		(uint8_t) 48
#define NOTE_4CS	(uint8_t) 49
#define NOTE_4D		(uint8_t) 50
#define NOTE_4DS	(uint8_t) 51
#define NOTE_4E		(uint8_t) 52
#define NOTE_4F		(uint8_t) 53
#define NOTE_4FS	(uint8_t) 54
#define NOTE_4G		(uint8_t) 55
#define NOTE_4GS	(uint8_t) 56
#define NOTE_4A		(uint8_t) 57
#define NOTE_4AS	(uint8_t) 58
#define NOTE_4B		(uint8_t) 59

#define NOTE_5C		(uint8_t) 60
#define NOTE_5CS	(uint8_t) 61
#define NOTE_5D		(uint8_t) 62
#define NOTE_5DS	(uint8_t) 63
#define NOTE_5E		(uint8_t) 64
#define NOTE_5F		(uint8_t) 65
#define NOTE_5FS	(uint8_t) 66
#define NOTE_5G		(uint8_t) 67
#define NOTE_5GS	(uint8_t) 68
#define NOTE_5A		(uint8_t) 69
#define NOTE_5AS	(uint8_t) 70
#define NOTE_5B		(uint8_t) 71


#define NOTE_6C		(uint8_t) 72
#define NOTE_6CS	(uint8_t) 73
#define NOTE_6D		(uint8_t) 74
#define NOTE_6DS	(uint8_t) 75
#define NOTE_6E		(uint8_t) 76
#define NOTE_6F		(uint8_t) 77
#define NOTE_6FS	(uint8_t) 78
#define NOTE_6G		(uint8_t) 79
#define NOTE_6GS	(uint8_t) 80
#define NOTE_6A		(uint8_t) 81
#define NOTE_6AS	(uint8_t) 82
#define NOTE_6B		(uint8_t) 83



#endif

