#ifndef KBD_H
#define KBD_H	          
//=============================================================================
#include <avr/io.h>
//===============================================================================
#include "pins.h"
//=============================================================================
void KBD_init(void);
void KBD_scan(void);
void KBD_set_type(uint8_t val);
unsigned char debounce_sw_standby(void);
unsigned char debounce_sw_menu(void);
unsigned char debounce_sw_set(void);
unsigned char debounce_sw_select(void);
unsigned char debounce_sw_up(void);
unsigned char debounce_sw_down(void);
unsigned char debounce_sw_left(void);
unsigned char debounce_sw_right(void);
//=============================================================================
#endif // KBD_H
