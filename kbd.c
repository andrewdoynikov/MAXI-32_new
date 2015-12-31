#include <avr/io.h>
#include "pins.h"
#include "kbd.h"
#include "rtos.h"
#include "events.h"
#include "pins.h"
//=============================================================================
unsigned char kbd_type = 1;
//=============================================================================
void KBD_set_type(uint8_t val)
{
  kbd_type = val;
}
//=============================================================================
void KBD_scan(void)
{
  unsigned char key = KBD_NOKEY;
  if (debounce_sw_standby() == 1) { key = EVENT_KEY_STANDBY; }
  if (debounce_sw_menu() == 1) { key = EVENT_KEY_MENU; }
  if (debounce_sw_set() == 1) { key = EVENT_KEY_SET; }
  if (debounce_sw_select() == 1) { key = EVENT_KEY_SELECT; }
  if (debounce_sw_up() == 1) { key = EVENT_KEY_UP; }
  if (debounce_sw_down() == 1) { key = EVENT_KEY_DOWN; }
  if (kbd_type == 1) {
    if (debounce_sw_left() >= 1) { key = EVENT_KEY_LEFT; }
    if (debounce_sw_right() >= 1) { key = EVENT_KEY_RIGHT; }
  } else {
    if (debounce_sw_left() == 1) { key = EVENT_KEY_LEFT; }
    if (debounce_sw_right() == 1) { key = EVENT_KEY_RIGHT; }
  }
  if (key != KBD_NOKEY) {
    BEEP();
    RTOS_setTask(EVENT_KEY_PRESS, 0, 0); 
    RTOS_setTask(key, 0, 0); 
  }
}
//=============================================================================
void KBD_init(void)
{
  DDR(BUT_STANDBY) &= ~BUT_STANDBY_LINE;
  PORT(BUT_STANDBY) |= BUT_STANDBY_LINE;
  DDR(BUT_SET) &= ~BUT_SET_LINE;
  PORT(BUT_SET) |= BUT_SET_LINE;
  DDR(BUT_MENU) &= ~BUT_MENU_LINE;
  PORT(BUT_MENU) |= BUT_MENU_LINE;
  DDR(BUT_SELECT) &= ~BUT_SELECT_LINE;
  PORT(BUT_SELECT) |= BUT_SELECT_LINE;
  DDR(BUT_UP) &= ~BUT_UP_LINE;
  PORT(BUT_UP) |= BUT_UP_LINE;
  DDR(BUT_DOWN) &= ~BUT_DOWN_LINE;
  PORT(BUT_DOWN) |= BUT_DOWN_LINE;
  DDR(BUT_LEFT) &= ~BUT_LEFT_LINE;
  PORT(BUT_LEFT) |= BUT_LEFT_LINE;
  DDR(BUT_RIGHT) &= ~BUT_RIGHT_LINE;
  PORT(BUT_RIGHT) |= BUT_RIGHT_LINE;
  DDR(BEEPER) |= BEEPER_LINE;
  PORT(BEEPER) |= BEEPER_LINE;
//-----------------------------------------------------------------------------
  DDR(LED_FAULT_IN) &= ~LED_FAULT_IN_LINE;
  PORT(LED_FAULT_IN) |= LED_FAULT_IN_LINE;
  DDR(LED_FAULT_IMPULS0) |= LED_FAULT_IMPULS0_LINE;
  LED_FAULT_IMPULS0_off();
  DDR(LED_FAULT_IMPULS1) |= LED_FAULT_IMPULS1_LINE;
  LED_FAULT_IMPULS1_off();
  DDR(LED_MUTE_DUBLE) |= LED_MUTE_DUBLE_LINE;
  LED_MUTE_DUBLE_1;
  DDR(LED_STB_IMPULS) |= LED_STB_IMPULS_LINE;
//-----------------------------------------------------------------------------
}
//=============================================================================
unsigned char debounce_sw_standby(void)
{
static uint16_t state1 = 0; //holds present state
state1 = (state1 << 1) | (! bit_is_clear(PIN(BUT_STANDBY), BUT_STANDBY_PIN)) | 0xE000;
if (state1 == 0xF000) return 1;
return 0;
}
//=============================================================================
unsigned char debounce_sw_left(void)
{
static uint16_t state2 = 0; //holds present state
state2 = (state2 << 1) | (! bit_is_clear(PIN(BUT_LEFT), BUT_LEFT_PIN)) | 0xE000;
if (state2 == 0xF000) return 1;
if (state2 == 0xE000) return 2;
return 0;
}
//=============================================================================
unsigned char debounce_sw_right(void)
{
static uint16_t state3 = 0; //holds present state
state3 = (state3 << 1) | (! bit_is_clear(PIN(BUT_RIGHT), BUT_RIGHT_PIN)) | 0xE000;
if (state3 == 0xF000) return 1;
if (state3 == 0xE000) return 2;
return 0;
}
//=============================================================================
unsigned char debounce_sw_set(void)
{
static uint16_t state4 = 0; //holds present state
state4 = (state4 << 1) | (! bit_is_clear(PIN(BUT_SET), BUT_SET_PIN)) | 0xE000;
if (state4 == 0xF000) return 1;
return 0;
}
//=============================================================================
unsigned char debounce_sw_menu(void)
{
static uint16_t state5 = 0; //holds present state
state5 = (state5 << 1) | (! bit_is_clear(PIN(BUT_MENU), BUT_MENU_PIN)) | 0xE000;
if (state5 == 0xF000) return 1;
return 0;
}
//=============================================================================
unsigned char debounce_sw_select(void)
{
static uint16_t state6 = 0; //holds present state
state6 = (state6 << 1) | (! bit_is_clear(PIN(BUT_SELECT), BUT_SELECT_PIN)) | 0xE000;
if (state6 == 0xF000) return 1;
return 0;
}
//=============================================================================
unsigned char debounce_sw_up(void)
{
static uint16_t state7 = 0; //holds present state
state7 = (state7 << 1) | (! bit_is_clear(PIN(BUT_UP), BUT_UP_PIN)) | 0xE000;
if (state7 == 0xF000) return 1;
if (state7 == 0xE000) return 2;
return 0;
}
//=============================================================================
unsigned char debounce_sw_down(void)
{
static uint16_t state8 = 0; //holds present state
state8 = (state8 << 1) | (! bit_is_clear(PIN(BUT_DOWN), BUT_DOWN_PIN)) | 0xE000;
if (state8 == 0xF000) return 1;
if (state8 == 0xE000) return 2;
return 0;
}
//=============================================================================

