



//#define IS_BUZZER			// Define if its a buzzer
//#define BEEP_EVERY_CYCLE

#define CHIRPS                      3       // Number of chirps

//#define FIXED_INTERVAL              2

// pins
#define	BUZZER_PIN	                PB1
#define LED_RED  	                PB4
#define LED_GREEN	                PB0 


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



uint16_t RANDOM_SLEEP_MIN = 1;			/// was 7
uint16_t RANDOM_SLEEP_MAX = 16;		// 2 is very regular, was 30



//#ifdef __AVR_ATtiny13__
  #define SLEEP_FOREVER  128
  #define SLEEP_016MS    ~(1<<WDP3)|~(1<<WDP2)|~(1<<WDP1)|~(1<<WDP0)
  #define SLEEP_125MS    (1<<WDP1) | (1<<WDP0)
  #define SLEEP_250MS    (1<<WDP2)
  #define SLEEP_500MS    (1<<WDP2) | (1<<WDP0)
  #define SLEEP_1SEC     (1<<WDP2) | (1<<WDP1)
  #define SLEEP_2SEC     (1<<WDP2) | (1<<WDP1) | (1<<WDP0)
  #define SLEEP_4SEC     (1<<WDP3)
  #define SLEEP_8SEC     (1<<WDP3) | (1<<WDP0)
//#endif /* ifdef __AVR_ATtiny13__ */


// I think those are wrong.. (_BV(CS01)|_BV(CS00))



#define	N_1	(_BV(CS00))                 // No Prescaling
#define	N_8	(_BV(CS01))
#define	N_64	(_BV(CS01)|_BV(CS00))
#define	N_256	(_BV(CS02))
#define	N_1024	(_BV(CS02)|_BV(CS00))   // clk/1024 from prescaler




