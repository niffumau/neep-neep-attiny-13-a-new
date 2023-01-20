
#ifndef MAIN_H
#define MAIN_H



//#define IS_BUZZER			// Define if its a buzzer
//#define BEEP_EVERY_CYCLE

#define CHIRPS                      3       // Number of chirps

#define DUTY_CYCLE                  50

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
uint16_t RANDOM_SLEEP_MAX = 8;		// 2 is very regular, was 30.. 4 seemed .. ok but regular, 16 seems maybe too long?



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





#define NOTE_0C   0
#define NOTE_0CS	1
#define NOTE_0D		2
#define NOTE_0DS	3
#define NOTE_0E		4
#define NOTE_0F		5
#define NOTE_0FS	6
#define NOTE_0G		7
#define NOTE_0GS	8
#define NOTE_0A		9
#define NOTE_0AS	10
#define NOTE_0B		11

#define NOTE_1C   12		
#define NOTE_1CS	
#define NOTE_1D		
#define NOTE_1DS	
#define NOTE_1E		
#define NOTE_1F		
#define NOTE_1FS	
#define NOTE_1G		
#define NOTE_1GS	
#define NOTE_1A		
#define NOTE_1AS	
#define NOTE_1B		

#define NOTE_2C		24	
#define NOTE_2CS	
#define NOTE_2D		
#define NOTE_2DS	
#define NOTE_2E		
#define NOTE_2F		
#define NOTE_2FS	
#define NOTE_2G		
#define NOTE_2GS	
#define NOTE_2A		
#define NOTE_2AS	
#define NOTE_2B		

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

const uint8_t divisors[] = {142,134,127,120,113,106,100,95,89,84,79,75,71,67,63,59,56,53,50,47,44,42,39,37,35,33,31,29,27,26,24,23,22,20,19,18};

typedef struct s_notes {
	uint8_t	note; 
	uint8_t duration;
} notes_t;


//{ // octave 0
	//.note_C = {142, N_256}, // 16.35 Hz
	//.note_CS = {134, N_256}, // 17.32 Hz
/*
notes_t tune_happybirthday[25] = {
	{.note = NOTE_5G, .duration = 1},
	{.note = NOTE_5G, .duration = 1},
	{.note = NOTE_5A, .duration = 1},
	{.note = NOTE_5G, .duration = 1},
	{.note = NOTE_6C, .duration = 1},
	{.note = NOTE_5B, .duration = 1}
};*/


uint8_t tune_scale_notes[7] = {NOTE_4C,NOTE_4D,NOTE_4E,NOTE_4F,NOTE_4G,NOTE_4A,NOTE_4B};

uint8_t tune_scale_notes_duration[14] = {NOTE_4C,1,NOTE_4D,1,NOTE_4E,1,NOTE_4F,1,NOTE_4G,1,NOTE_4A,NOTE_4B};

const notes_t tune_scale_0[] PROGMEM = {
  {NOTE_0C,1},
  {NOTE_0CS,1},
  {NOTE_0D,1},
  {NOTE_0DS,1},
  {NOTE_0E,1},
  {NOTE_0F,1},
  {NOTE_0FS,1},
  {NOTE_0G,1},
  {NOTE_0GS,1},
  {NOTE_0A,1},
  {NOTE_0AS,1},
  {NOTE_0B,1}
};

const notes_t tune_scale_3[12] PROGMEM = {
  {NOTE_3C,2},
  {NOTE_3CS,2},
  {NOTE_3D,2},
  {NOTE_3DS,2},
  {NOTE_3E,2},
  {NOTE_3F,2},
  {NOTE_3FS,2},
  {NOTE_3G,2},
  {NOTE_3GS,2},
  {NOTE_3A,2},
  {NOTE_3AS,2},
  {NOTE_3B,2}
};

const notes_t tune_scale_4[12] PROGMEM = {
  {NOTE_4C,2},
  {NOTE_4CS,2},
  {NOTE_4D,2},
  {NOTE_4DS,2},
  {NOTE_4E,2},
  {NOTE_4F,2},
  {NOTE_4FS,2},
  {NOTE_4G,2},
  {NOTE_4GS,2},
  {NOTE_4A,2},
  {NOTE_4AS,2},
  {NOTE_4B,2}
};
/*
notes_t tune_test[25]= {
  {1,1},
  {2,1},
  {3,1},
  {4,1},
  {5,1},
  {6,1},
  {7,1},
  {8,1},
  {9,1},
  {10,1},
  {11,1},
  {12,1},
  {13,1},
  {14,1},
  {15,1},
  {16,1},
  {17,1},
  {18,1},
  {19,1},
  {20,1},
  {21,1},
  {22,1},
  {23,1},
  {24,1},
  {25,1}
};*/



const notes_t tune_nokia[13] PROGMEM = {
  {NOTE_6E,1},
  {NOTE_6D,1},
  {NOTE_5FS,2},
  {NOTE_5GS,1},

  {NOTE_6CS,1},
  {NOTE_5B,1},
  {NOTE_5D,2},
  {NOTE_5E,1},

  {NOTE_5B,1},
  {NOTE_5A,1},
  {NOTE_5CS,2},
  {NOTE_5E,2},
  {NOTE_5A,2}

};

const notes_t tune_test[] PROGMEM = {
  {1,1},
  {2,1},
  {3,2},
  {4,2},{5,2},{6,2},{7,2},{8,2},{9,2}
};

const notes_t tune_happybirthday[] PROGMEM = {
	{NOTE_5G,1},
	{NOTE_5G,1},
	{NOTE_5A,1},
	{NOTE_5G,1},
	{NOTE_6C,1},
	{NOTE_5B,4},

	{NOTE_5G,1},
	{NOTE_5G,1},
	{NOTE_5A,1},
	{NOTE_5G,1},
	{NOTE_6D,1},
	{NOTE_6C,4},

	{NOTE_5G,1},
	{NOTE_5G,1},
	{NOTE_6G,1},
	{NOTE_6E,1},
	{NOTE_6C,1},
	{NOTE_5B,1},
	{NOTE_5A,1},

	{NOTE_6F,1},
	{NOTE_6F,1},
	{NOTE_6E,1},
	{NOTE_6C,1},
	{NOTE_6D,1},
	{NOTE_6C,1}
	
};


///// rubbish here



/*

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
} octave_t;*/

/*
// these all need to be multiplied by 8 because the clock is 8x faster
//24 bytes per octave
//PROGMEM const octave_t octaves[4] = {
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
	},
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
	}
};*/



#endif

