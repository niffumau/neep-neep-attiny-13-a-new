#ifndef TUNES_H
#define TUNES_H

#include "main.h"

/*
typedef struct s_notes {
	uint8_t	note; 
	uint8_t duration;
} notes_t;
*/


typedef struct s_tune {
    uint8_t size;
    notes_t notes[];
} tune_t;

//extern const notes_t tune_new[];

#endif