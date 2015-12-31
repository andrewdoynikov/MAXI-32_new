#ifndef ENCODER_H
#define ENCODER_H
//=============================================================================
#include <avr/io.h>
#include "pins.h"
#include "events.h"
//=============================================================================
#define ENC_MASK       		3
#define ENC_RIGHT_MASK 		210
#define ENC_LEFT_MASK  		225
#define ENC_RIGHT 			EVENT_KEY_RIGHT 
#define ENC_LEFT  			EVENT_KEY_LEFT 
//=============================================================================
void ENC_init(void);
void ENC_scan(void);
//=============================================================================
#endif
