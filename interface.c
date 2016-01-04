#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//=============================================================================
#include "pins.h"
#include "rtos.h"
#include "events.h"
#include "lcd_lib.h"
#include "ds1307.h"
#include "interface.h"
#include "enc.h"
#include "rc5.h"
#include "kbd.h"
#include "tda7313.h"
#include "ds18x20.h"
#include "tuner.h"
//=============================================================================
#include "english.h"
//=============================================================================
extern void (*pState)(unsigned char event);
void (*old_pState)(unsigned char event);
#define SET_STATE(a) pState = a  // макрос для смены состояния
//=============================================================================
unsigned char blinks = 0;
unsigned char nokey = 0;
unsigned char mmenu = MM_VOLUME, omenu = OM_SET_TIME, rc5menu = KEY_STANDBY, nedit = 0;
unsigned char mute = 0;
unsigned char key_stb = 0, edit_rc5 = 0;
unsigned char time_alarm = 0, fault_on = 0;
unsigned char fm_mode = VOLUME_MODE;
unsigned char channel = 0, old_chan;
unsigned char vol_up, vol_down;
unsigned char hh, mm, ss, dd, ms, yy, a_hh, a_mm, a_ss, a_wday, a_onoff;
unsigned char max_temp, old_max_temp, as_wait, old_as_wait, stb_out;
unsigned char rc5_code[2][RC5_MENU_MAX + 1], rc5_cmd = 0xFF;
unsigned char main_mode = MODE_STANDBY;
unsigned char ds_cnt_show = 0;
//-----------------------------------------------------------------------------
char bignumchars1[] = {
  0x03,0x00,0x03, 0x00,0x03,0x20, 0x02,0x02,0x03, 0x00,0x02,0x03, 0x03,0x01,0x03,
  0x03,0x02,0x02, 0x03,0x02,0x02, 0x00,0x00,0x03, 0x03,0x02,0x03, 0x03,0x02,0x03
};
char bignumchars2[] = {
  0x03,0x01,0x03, 0x01,0x03,0x01, 0x03,0x01,0x01, 0x01,0x01,0x03, 0x20,0x20,0x03,
  0x01,0x01,0x03, 0x03,0x01,0x03, 0x20,0x20,0x03, 0x03,0x01,0x03, 0x01,0x01,0x03
};
//=============================================================================
void alarm_beep(void)
{
  BEEP_off()
  if (time_alarm > 0) {
    if (blinks == 1) { BEEP_on(); } else { BEEP_off(); }
	time_alarm--;
  }
}
//=============================================================================
void rc5_buffer_init(void)
{
  uint8_t i;
  for (i = 0; i < RC5_MENU_MAX + 1; i++) {
    rc5_code[RC5_CMD][i] = 0xFF;
    rc5_code[RC5_KEY][i] = 100 + i;
  }
}
//=============================================================================
uint8_t get_rc5_key(uint8_t code)
{
  uint8_t i, ret = 0xFF;
  for (i = 0; i < RC5_MENU_MAX + 1; i++) {
    if ( rc5_code[RC5_CMD][i] == code) {
	  ret = rc5_code[RC5_KEY][i];
	  break;
	}
  }
  return ret;
}
//=============================================================================
void save_rc5code(void)
{
  uint8_t i;
  for (i = 0; i < RC5_MENU_MAX + 1; i++) {
    save_rc5code_key(i, rc5_code[RC5_CMD][i]);
  }
}
//=============================================================================
void load_rc5code(void)
{
  uint8_t i;
  for (i = 0; i < RC5_MENU_MAX + 1; i++) {
    load_rc5code_key(i);
  }
}
//=============================================================================
void save_rc5code_key(uint8_t nkey, uint8_t key)
{
  eeprom_write_byte((uint8_t*)RC5_CODE_ADDR + nkey, key);
  rc5_code[RC5_CMD][nkey] = key;
}
//=============================================================================
uint8_t load_rc5code_key(uint8_t nkey)
{
  uint8_t t;
  t = eeprom_read_byte((uint8_t*)RC5_CODE_ADDR + nkey);
  if (t > EVENT_RC5_CODE_MAX) t = 0xFF;
  rc5_code[RC5_CMD][nkey] = t;
  return t;
}
//=============================================================================
void save_alarm_param(void)
{
#if (ALARM_DEBUG == 0) 
  eeprom_write_byte((uint8_t*)ALARM_ONOFF_ADDR, a_onoff);
  eeprom_write_byte((uint8_t*)ALARM_HOUR_ADDR, a_hh);
  eeprom_write_byte((uint8_t*)ALARM_MIN_ADDR, a_mm);
  eeprom_write_byte((uint8_t*)ALARM_SEC_ADDR, a_ss);
  eeprom_write_byte((uint8_t*)ALARM_WDAY_ADDR, (a_wday & 0x7F));
#endif
}
//=============================================================================
void load_alarm_param(void)
{
#if (ALARM_DEBUG == 0) 
  uint8_t t;
  t = eeprom_read_byte((uint8_t*)ALARM_ONOFF_ADDR);
  if (t > 1) a_onoff = 0; else a_onoff = t;
  t = eeprom_read_byte((uint8_t*)ALARM_HOUR_ADDR);
  if (t > 23) { a_hh = 0; a_onoff = 0; } else a_hh = t;
  t = eeprom_read_byte((uint8_t*)ALARM_MIN_ADDR);
  if (t > 59) { a_mm = 0;  a_onoff = 0; } else a_mm = t;
  t = eeprom_read_byte((uint8_t*)ALARM_SEC_ADDR);
  if (t > 59) { a_ss = 0;  a_onoff = 0; } else a_ss = t;
  t = eeprom_read_byte((uint8_t*)ALARM_WDAY_ADDR);
  a_wday = t & 0x7F;
#else
  RTC_set_time(13, 58, 57);
  a_onoff = 1; a_hh = 14; a_mm = 00; a_ss = 0; a_wday = 0x7F;
#endif
}
//=============================================================================
void save_as_wait(uint8_t temp)
{
  eeprom_write_byte((uint8_t*)AS_WAIT_ADDR, temp);
}
//=============================================================================
uint8_t load_as_wait(void)
{
  uint8_t t;
  t = eeprom_read_byte((uint8_t*)AS_WAIT_ADDR);
  if (t > MAX_AS_WAIT) t = DEF_AS_WAIT;
  as_wait = t;
  return t;
}
//=============================================================================
void save_max_temperature(uint8_t temp)
{
  eeprom_write_byte((uint8_t*)MAX_TEMPERATURE_ADDR, temp);
}
//=============================================================================
uint8_t load_max_temperature(void)
{
  uint8_t t;
  t = eeprom_read_byte((uint8_t*)MAX_TEMPERATURE_ADDR);
  if ((t > MAX_TEMP) || (t < MIN_TEMP)) t = DEF_TEMP;
  max_temp = t;
  return t;
}
//=============================================================================
void save_stb_out(uint8_t temp)
{
  eeprom_write_byte((uint8_t*)STANDBY_OUT_ADDR, temp);
}
//=============================================================================
uint8_t load_stb_out(void)
{
  uint8_t t;
  t = eeprom_read_byte((uint8_t*)STANDBY_OUT_ADDR);
  if (t > 1) t = STANDBY_OUT_DEF;
  stb_out = t;
  return t;
}
//=============================================================================
void stb_impuls(void)
{
  if (fault_on == 0) {
    cli();
    LED_STB_IMPULS_1;
	_delay_ms(1000);
	LED_STB_IMPULS_0;
	sei();
  }
}
//=============================================================================
void DS18x20_scan(void)
{
  static unsigned char ds_state = 0;
  switch(ds_state) {
    case 0:
	  ds18x20SearchDevices();
	  if (ds18x20GetDevCount() > 0) {
	    ds_state = 1;
        RTOS_setTaskFunc(DS18x20_scan, 0, 0);
	  } else {
	    LED_VENTIL_off();
        RTOS_setTaskFunc(DS18x20_scan, 3000, 0);
	  }
    break;
    case 1:
	  ds18x20Process();
      if (ds18x20GetDevCount() > 0) {
        ds_state = 2;
	  }
      RTOS_setTaskFunc(DS18x20_scan, 1000, 0);
    break;
    case 2:
	  if ((ds18x20GetTemp(0) / 10) >= max_temp) { 
	    LED_VENTIL_on();
      }
      if (ds18x20GetDevCount() > 1) {
        ds_state = 3;
      } else {
        ds_state = 6;
	  }
      RTOS_setTaskFunc(DS18x20_scan, 0, 0);
    break;
    case 3:
	  if ((ds18x20GetTemp(1) / 10) >= max_temp) { 
	    LED_VENTIL_on();
      }
      if (ds18x20GetDevCount() > 2) {
        ds_state = 4;
      } else {
        ds_state = 6;
	  }
      RTOS_setTaskFunc(DS18x20_scan, 0, 0);
    break;
    case 4:
	  if ((ds18x20GetTemp(2) / 10) >= max_temp) { 
	    LED_VENTIL_on();
      }
      if (ds18x20GetDevCount() > 3) {
        ds_state = 5;
      } else {
        ds_state = 6;
	  }
      RTOS_setTaskFunc(DS18x20_scan, 0, 0);
    break;
    case 5:
	  if ((ds18x20GetTemp(3) / 10) >= max_temp) { 
	    LED_VENTIL_on();
      }
      ds_state = 6;
      RTOS_setTaskFunc(DS18x20_scan, 0, 0);
    break;
    case 6:
      if (ds18x20GetDevCount() == 1) {
		if ( (ds18x20GetTemp(0) / 10) <= (max_temp - ONE_WIRE_GISTER) ) LED_VENTIL_off();
	  } else if (ds18x20GetDevCount() == 2) {
		if ( (ds18x20GetTemp(0) / 10) <= (max_temp - ONE_WIRE_GISTER) && (ds18x20GetTemp(1) / 10) <= (max_temp - ONE_WIRE_GISTER) ) LED_VENTIL_off();
	  } else if (ds18x20GetDevCount() == 3) {
		if ( (ds18x20GetTemp(0) / 10) <= (max_temp - ONE_WIRE_GISTER) && (ds18x20GetTemp(1) / 10) <= (max_temp - ONE_WIRE_GISTER) && (ds18x20GetTemp(2) / 10) <= (max_temp - ONE_WIRE_GISTER) ) LED_VENTIL_off();
      } else if (ds18x20GetDevCount() == 4) {
		if ( (ds18x20GetTemp(0) / 10) <= (max_temp - ONE_WIRE_GISTER) && (ds18x20GetTemp(1) / 10) <= (max_temp - ONE_WIRE_GISTER) && (ds18x20GetTemp(2) / 10) <= (max_temp - ONE_WIRE_GISTER)  && (ds18x20GetTemp(3) / 10) <= (max_temp - ONE_WIRE_GISTER) ) LED_VENTIL_off();
	  }
      ds_state = 0;
      RTOS_setTaskFunc(DS18x20_scan, ONE_WIRE_TIME, 0);
    break;
  }
}
//=============================================================================
void RC5_scan(void)
{
  static signed char old_flip = -1;
  unsigned char c;
  if (rc5.flip != -1) {
    if ((rc5.flip != old_flip) || (rc5.code == EVENT_RC5_LEFT) || (rc5.code == EVENT_RC5_RIGHT)) {
      LED_RC5_CMD_flip();
	  BEEP();
	  old_flip = rc5.flip;
      RTOS_setTask(EVENT_KEY_PRESS, 0, 0); 
	  if (edit_rc5 == 1) {
        RTOS_setTask(rc5.code, 0, 0); 
      } else {
	    c = get_rc5_key(rc5.code);
	    if ( c != 0xFF) RTOS_setTask(c, 0, 0); 
	  }
    }
	rc5.flip = -1;
  }
}
//=============================================================================
uint8_t check_fault(void)
{
  uint8_t r = 0;
  if (BitIsClear(PIN(LED_FAULT_IN), LED_FAULT_IN_PIN)) {
    _delay_ms(20);
    if (BitIsClear(PIN(LED_FAULT_IN), LED_FAULT_IN_PIN)) {
      fault_on = 1;
	  r = 1;
    }
  }
  return r;
}
//=============================================================================
void lcd_bigchar(unsigned char col, unsigned char x)
{
  unsigned char i;
  i = x * 3;
  LCD_SetXY(col, 0);
  LCD_dat(bignumchars1[i]); LCD_dat(bignumchars1[i + 1]); LCD_dat(bignumchars1[i + 2]); LCD_dat(' ');
  LCD_SetXY(col, 1);
  LCD_dat(bignumchars2[i]); LCD_dat(bignumchars2[i + 1]); LCD_dat(bignumchars2[i + 2]); LCD_dat(' ');
}
//=============================================================================
void show_bigtime(void)
{
  unsigned char h, m, s, w;
  RTC_get_time(&h, &m, &s);
  w = RTC_dayofweek();
  if (blinks)
  { 
    LCD_SetXY(10, 0); LCD_dat(0x04);
    LCD_SetXY(10, 1); LCD_dat(0x04);
  } else {
    LCD_SetXY(10, 0); LCD_dat(0x20);
    LCD_SetXY(10, 1); LCD_dat(0x20);
  }
  lcd_bigchar(2, h / 10); lcd_bigchar(6, h % 10); lcd_bigchar(11, m / 10); lcd_bigchar(15, m % 10);
  if (fault_on == 0) {
    if (!mute) {
      LCD_SetXY(5, 2); RTC_print_date();
      LCD_SetXY(5, 3); LCD_puts(den_dw_full[w]);
    } else {
      LCD_SetXY(5, 3); if (!blinks) LCD_puts(MSG_MUTE); else LCD_space(10);
      LCD_SetXY(5, 2); LCD_space(10);
    }
  } else {
    LCD_SetXY(0, 2); LCD_space(20);
    LCD_SetXY(0, 3); LCD_space(5);
	if (blinks) LCD_puts(MSG_FAULT); else LCD_space(9);
    LCD_space(6);
  }
}
//=============================================================================
void show_temperature(void)
{
  int16_t t = 0;
  if (ds_cnt_show <= TIME_SHOW_DS_1)
  {
// 1   ------------------------------------------------------------------------
  LCD_SetXY(0, 3);
  if (ds18x20GetDevCount() > 0) {
    t = ds18x20GetTemp(0) / 10;
#if (TEMP_BLINK_MAX == 1) 
    if (t >= max_temp)  {
	  if (blinks == 0) {
	    if (t < 0) LCD_dat('-'); else LCD_dat('+');
	    print_dec(t, 2,' '); LCD_dat('C');
	  } else {
	    LCD_space(4);
	  }
    } else {
	  if (t < 0) LCD_dat('-'); else LCD_dat('+');
	  print_dec(t, 2,' '); LCD_dat('C');
	}
    if (ds18x20GetDevCount() == 1) {
      LCD_SetXY(15, 3);
	  LCD_puts("     ");
	}
#else
   if (t < 0) LCD_dat('-'); else LCD_dat('+');
   print_dec(t, 2,' '); LCD_dat('C');
#endif
  } else {
    LCD_puts(MSG_NO_DS);
  }
// 2   ------------------------------------------------------------------------
  LCD_SetXY(16, 3);
  if (ds18x20GetDevCount() > 1) 
  {
    t = ds18x20GetTemp(1) / 10;
#if (TEMP_BLINK_MAX == 1) 
    if (t >= max_temp)  {
	  if (blinks == 0) {
	    if (t < 0) LCD_dat('-'); else LCD_dat('+');
	    print_dec(t, 2,' '); LCD_dat('C');
	  } else {
	    LCD_space(4);
	  }
    } else {
	  if (t < 0) LCD_dat('-'); else LCD_dat('+');
	  print_dec(t, 2,' '); LCD_dat('C');
	}
#else
   if (t < 0) LCD_dat('-'); else LCD_dat('+');
   print_dec(t, 2,' '); LCD_dat('C');
#endif
  } else {
    LCD_puts(MSG_NO_DS);
  }
  }

  if ((ds_cnt_show > TIME_SHOW_DS_1) && (ds18x20GetDevCount() > 2))
//  if (ds_cnt_show > TIME_SHOW_DS_1)
  {
// 3   ------------------------------------------------------------------------
  LCD_SetXY(0, 3);
  if (ds18x20GetDevCount() > 2)
  {
    t = ds18x20GetTemp(2) / 10;
#if (TEMP_BLINK_MAX == 1) 
    if (t >= max_temp)  {
	  if (blinks == 0) {
	    if (t < 0) LCD_dat('-'); else LCD_dat('+');
	    print_dec(t, 2,' '); LCD_dat('C');
	  } else {
	    LCD_space(4);
	  }
    } else {
	  if (t < 0) LCD_dat('-'); else LCD_dat('+');
	  print_dec(t, 2,' '); LCD_dat('C');
	}
    if (ds18x20GetDevCount() == 1) {
      LCD_SetXY(15, 3);
	  LCD_puts("     ");
	}
#else
   if (t < 0) LCD_dat('-'); else LCD_dat('+');
   print_dec(t, 2,' '); LCD_dat('C');
#endif
  } else {
    LCD_puts(MSG_NO_DS);
  }
// 4   ------------------------------------------------------------------------
  LCD_SetXY(16, 3);
  if (ds18x20GetDevCount() > 3)
  {
    t = ds18x20GetTemp(3) / 10;
#if (TEMP_BLINK_MAX == 1) 
    if (t >= max_temp)  {
	  if (blinks == 0) {
	    if (t < 0) LCD_dat('-'); else LCD_dat('+');
	    print_dec(t, 2,' '); LCD_dat('C');
	  } else {
	    LCD_space(4);
	  }
    } else {
	  if (t < 0) LCD_dat('-'); else LCD_dat('+');
	  print_dec(t, 2,' '); LCD_dat('C');
	}
#else
   if (t < 0) LCD_dat('-'); else LCD_dat('+');
   print_dec(t, 2,' '); LCD_dat('C');
#endif
  } else {
    LCD_puts(MSG_NO_DS);
  }
  } else {
    ds_cnt_show = 0;
  }
}
//=============================================================================
void show_freq(void)
{
  unsigned char i;
  unsigned char s[6];
  uint16_t freq = tuner_ChanToFreq(tuner_GetChan(channel));
  if (tuner_GetChan(channel) > FM_CHAN_MAX) {
    LCD_puts(MSG_ERROR_CHANNEL);
  } else {
    for (i = 0; i < 5; i++) {
      s[5 - i - 1] = '0' + (freq % 10);
      freq /= 10;
    }
    if (s[0] == '0') { s[0] = ' '; }
    for (i = 0; i < 5; i++) {
      if (i == 3) { LCD_dat('.'); }
      LCD_dat(s[i]);
    }
  }
}
//=============================================================================
void volume_step_up(void)
{
  unsigned char i;
  vol_up = tda7313Get_Param(TDA_VOLUME);
  tda7313Set_Param(TDA_VOLUME, 0);
  show_lcd_main();
  cli();
  for(i = 0; i < vol_up; i++)
  {
    tda7313Set_Param_up(TDA_VOLUME);
    LCD_SetXY(18, 2); print_dec(tda7313Get_Param(mmenu), 2, '0');
    LCD_SetXY(0, 3); LCD_progress_bar(tda7313Get_Param(mmenu), tda7313Get_Max_Param(mmenu), 20);
  }
  sei();
}
//=============================================================================
void volume_step_down(void)
{
  uint8_t vol_down, i;
  vol_down = tda7313Get_Param(TDA_VOLUME);
  show_lcd_main();
  cli();
  for(i = vol_down; i > 0; i--)
  {
    tda7313Set_Param_down(TDA_VOLUME);
    LCD_SetXY(18, 2); print_dec(tda7313Get_Param(mmenu), 2, '0');
    LCD_SetXY(0, 3); LCD_progress_bar(tda7313Get_Param(mmenu), tda7313Get_Max_Param(mmenu), 20);
  }
  tda7313Set_Param_no_send(TDA_VOLUME, vol_down);
  sei();
}
//=============================================================================
void scan_fault(void)
{
  if (fault_on == 0) {
    if (check_fault() == 1) {
	  fault_on = 1;
  	  BEEP(); 
	  if (main_mode != MODE_STANDBY) {
        RTOS_setTask(EVENT_KEY_STANDBY, 0, 0);
	  }
	}
  }
}
//=============================================================================
void set_blink(void)
{
  blinks = !blinks;
  if (ds_cnt_show < TIME_SHOW_DS) {
    ds_cnt_show++;
  } else {
    ds_cnt_show = 0;
  }
  if (nokey < NOKEY_TIME * 2) {
    nokey++;
  } else {
    ds_cnt_show = 0;
    RTOS_setTask(EVENT_NOKEY, 0, 0);
  }
  if ((check_alarm() == 1) && (main_mode != MODE_OPTIONS) && (fault_on == 0)) {
	BEEP(); 
    time_alarm = ALARM_BEEP;
    tda7313SetInput(FM_INPUT);
    tda7313_Save_Input(FM_INPUT);
	LIGHT_on();
	if (main_mode == MODE_STANDBY) {
      tda7313SetMute(1);
      RTOS_setTask(EVENT_KEY_STANDBY, 0, 0);
	} else if (main_mode == MODE_NOKEY) {
      RTOS_setTask(EVENT_KEY_PRESS, 0, 0);
	} else if (main_mode == MODE_MUTE) {
      RTOS_setTask(EVENT_KEY_MUTE, 0, 0);
	} else if (main_mode == MODE_MAIN) {
      show_lcd_main();
	}
 	return;
  }
  RTOS_setTask(EVENT_TIMER_SECOND, 0, 0);
}
//=============================================================================
void run_start(unsigned char event)
{
  switch(event) {
    case EVENT_LOAD_PARAM:
	  LCD_SetXY(0, 2); LCD_space(20);
	  LCD_SetXY(0, 3); LCD_space(20);
      LCD_SetXY(3, 3); LCD_puts(MSG_LOAD_PARAM);
      tuner_SetChan(tuner_GetChan(channel));
	  fault_on = 0; 
      rc5_buffer_init();
	  load_rc5code();
	  load_alarm_param();
      load_max_temperature();
      load_stb_out();
      load_as_wait();
	  tuner_Load_all_stations();
      channel = tuner_Load_channel();
      tda7313_Load_all_param();
      RTOS_setTask(EVENT_SET_STB_OUT_ON, 0, 0);
    break;
    case EVENT_SET_STB_OUT_ON:
      if (load_stb_out() == 1) { LED_STB_OUT_0; } else { LED_STB_OUT_1; }
      RTOS_setTask(EVENT_SET_AS_WAIT_ON, (as_wait * 1000), 0);
    break;
    case EVENT_SET_AS_WAIT_ON:
	  LED_AC_WAIT_on();
      RTOS_setTask(EVENT_SHOW_VOL_UP, 300, 0);
    break;
    case EVENT_SHOW_VOL_UP:
      LED_STANDBY_off();
	  LCD_clear();
      LCD_load_defchar();
	  mute = 0;
      tda7313SetMute(0);
      volume_step_up();
      main_mode = MODE_MAIN;
      show_lcd_main();
	  nokey = 0;
      SET_STATE(run_main);
    break;
  }
}
//=============================================================================
void run_stop(unsigned char event)
{
  switch(event) {
    case EVENT_SAVE_PARAM:
      main_mode = MODE_STANDBY;
	  LED_STANDBY_on();
      RTOS_setTask(EVENT_SHOW_VOL_DOWN, 0, 0);
    break;
    case EVENT_SHOW_VOL_DOWN:
	  if (mute == 0) {
	    volume_step_down();
      }
      RTOS_setTask(EVENT_SET_AS_WAIT_OFF, 100, 0);
    break;
    case EVENT_SET_AS_WAIT_OFF:
      tda7313SetMute(1);
	  LED_AC_WAIT_off();
      RTOS_setTask(EVENT_SET_STB_OUT_OFF, 400, 0);
    break;
    case EVENT_SET_STB_OUT_OFF:
      LED_STANDBY_on();
      if (load_stb_out() == 1) { LED_STB_OUT_1; } else { LED_STB_OUT_0; }
	  LCD_clear();
      LCD_load_bignum();
	  mmenu = MM_VOLUME;
      SET_STATE(run_standby);
      RTOS_setTask(EVENT_TIMER_SECOND, 0, 0);
    break;
  }
}
//=============================================================================
void run_standby(unsigned char event)
{
  switch(event) {
    case EVENT_START:
	  fault_on = 0; 
      LCD_SetXY(2, 0); LCD_puts(MSG_LOGO1);
      LCD_SetXY(4, 1); LCD_puts(MSG_LOGO2);
      tda7313SetMute(1);
      rc5_buffer_init();
	  load_rc5code();
	  load_alarm_param();
      load_max_temperature();
      load_stb_out();
      load_as_wait();
	  tuner_Load_all_stations();
      channel = tuner_Load_channel();
      tda7313_Load_all_param();
      RTOS_setTask(EVENT_SHOW_DS_CNT, 500, 0);
    break;
    case EVENT_SHOW_DS_CNT:
      LCD_SetXY(1, 3); LCD_puts(MSG_DS_FOUND); print_dec(ds18x20GetDevCount(), 2,'0');
      RTOS_setTask(EVENT_START_TASK, 500, 0);
    break;
    case EVENT_START_TASK:
      RTOS_setTaskFunc(set_blink, 0, BLINK_TIME);       // моргание
      RTOS_setTaskFunc(KBD_scan, 0, KBD_SCAN_TIME);		// запускаем опрос кнопок
      RTOS_setTaskFunc(ENC_scan, 0, ENC_SCAN_TIME);		// запускаем опрос енкодера
      RTOS_setTaskFunc(RC5_scan, 0, RC5_SCAN_TIME);		// запускаем опрос RC5
      RTOS_setTaskFunc(DS18x20_scan, 0, 0);      		// запускаем опрос датчиков температуры
	  LCD_clear();
      LCD_load_bignum();
    break;
    case EVENT_NOKEY:
	  LIGHT_off();
    break;
    case EVENT_KEY_PRESS:
	  nokey = 0;
	  LIGHT_on();
	  BEEP_off();
	  if (ds_cnt_show < TIME_SHOW_DS_1) ds_cnt_show = TIME_SHOW_DS_1; else ds_cnt_show = 0;
    break;
    case EVENT_TIMER_SECOND:
      show_bigtime();
      if (fault_on == 0) {
	    show_temperature();
	    alarm_beep();
	  }
    break;
    case EVENT_KEY_STANDBY:
      stb_impuls();
	  if (fault_on == 0) {
        SET_STATE(run_start);
        RTOS_setTask(EVENT_LOAD_PARAM, 0, 0);
	  } else {
	    RTOS_setTask(EVENT_START_IMPULS, 0, 0);
	  }
    break;
    case EVENT_START_IMPULS:
      LED_FAULT_IMPULS0_on();
      LED_FAULT_IMPULS1_on();
      RTOS_setTask(EVENT_STOP_IMPULS, 1000, 0);
    break;
    case EVENT_STOP_IMPULS:
      LED_FAULT_IMPULS0_off();
      LED_FAULT_IMPULS1_off();
      RTOS_setTask(EVENT_CHECK_FAULT, 500, 0);
    break;
    case EVENT_CHECK_FAULT:
      if (check_fault() == 0) {
        fault_on = 0;
        RTOS_setTask(EVENT_TIMER_SECOND, 0, 0);
      }
    break;
    case EVENT_KEY_MENU:
	  LCD_clear();
      LCD_load_defchar();
      tda7313SetMute(1);
	  omenu = OM_SET_TIME;
	  main_mode = MODE_OPTIONS;
      SET_STATE(run_option);
      show_lcd_option();
    break;
  }
}
//=============================================================================
void run_mute(unsigned char event)
{
  switch(event) {
    case EVENT_KEY_PRESS:
	  nokey = 0;
	  LIGHT_on();
	  BEEP_off();
      time_alarm = 0;
	  if (ds_cnt_show < TIME_SHOW_DS_1) ds_cnt_show = TIME_SHOW_DS_1; else ds_cnt_show = 0;
    break;
    case EVENT_NOKEY:
	  LIGHT_off();
    break;
    case EVENT_KEY_MUTE:
    case EVENT_KEY_MENU:
	  LIGHT_on();
	  nokey = 0;
	  LCD_clear();
      LCD_load_defchar();
      LED_MUTE_DUBLE_1;
	  mute = 0;
      tda7313SetMute(0);
      SET_STATE(run_main);
      show_lcd_main();
    break;
    case EVENT_TIMER_SECOND:
      show_bigtime();
	  show_temperature();
	  alarm_beep();
    break;
	default:
    break;
  }
}
//=============================================================================
void run_nokey(unsigned char event)
{
  switch(event) {
    case EVENT_NOKEY:
	  LIGHT_off();
    break;
    case EVENT_KEY_PRESS:
      time_alarm = 0;
	  nokey = 0;
      main_mode = MODE_MAIN;
	  LIGHT_on();
	  LCD_clear();
      LCD_load_defchar();
      SET_STATE(old_pState);
      show_lcd_main();
	  BEEP_off();
	  if (ds_cnt_show < TIME_SHOW_DS_1) ds_cnt_show = TIME_SHOW_DS_1; else ds_cnt_show = 0;
    break;
    case EVENT_KEY_STANDBY:
      stb_impuls();
	  LCD_clear();
      LCD_load_bignum();
      SET_STATE(run_stop);
      RTOS_setTask(EVENT_SAVE_PARAM, 0, 0);
    break;
    case EVENT_TIMER_SECOND:
      show_bigtime();
	  show_temperature();
	  alarm_beep();
    break;
	default:
    break;
  }
}
//=============================================================================
void run_main(unsigned char event)
{
  uint8_t in;
  if ((event >= EVENT_KEY_1) && (event <= EVENT_KEY_9)) {
    tda7313SetInput(FM_INPUT);
	tda7313Send_Param();
    tda7313SetMute(0);
    channel = event - 110;
    tuner_SetChan(tuner_GetChan(channel));
    show_lcd_main();
    RTOS_setTask(EVENT_SAVE_CHANNEL, TIME_SAVE_PARAM, 0);
  }
  switch(event) {
    case EVENT_KEY_PRESS:
	  nokey = 0;
	  LIGHT_on();
	  BEEP_off();
      time_alarm = 0;
    break;
    case EVENT_NOKEY:
      main_mode = MODE_NOKEY;
	  nokey = 0;
	  LCD_clear();
      LCD_load_bignum();
	  old_pState = run_main;
      SET_STATE(run_nokey);
      RTOS_setTask(EVENT_TIMER_SECOND, 0, 0);
    break;
    case EVENT_KEY_STANDBY:
      stb_impuls();
      SET_STATE(run_stop);
      RTOS_setTask(EVENT_SAVE_PARAM, 0, 0);
    break;
    case EVENT_TIMER_SECOND:
      show_lcd_main();
	  alarm_beep();
    break;
    case EVENT_KEY_MUTE:
    case EVENT_KEY_MENU:
      main_mode = MODE_MUTE;
      LED_MUTE_DUBLE_0;
	  mute = 1;
      tda7313SetMute(1);
	  LCD_clear();
      LCD_load_bignum();
      SET_STATE(run_mute);
      RTOS_setTask(EVENT_TIMER_SECOND, 0, 0);
    break;
    case EVENT_KEY_SELECT:
	  in = tda7313GetInput();
	  if ( in < INPUT_MAX) tda7313SetInput(in + 1); else tda7313SetInput(0);
      RTOS_setTask(EVENT_SAVE_INPUT, TIME_SAVE_PARAM, 0);
	  mmenu = MM_VOLUME;
      show_lcd_main();
    break;
    case EVENT_KEY_UP:
      if (fm_mode == VOLUME_MODE) {
	    if (mmenu > 0) mmenu--; else mmenu = MMENU_MAX;
        RTOS_setTask(EVENT_RETURN_VOLUME, TIME_RETURN_VOLUME, 0);
	  } else {
	    if (channel > 0) channel--; else channel = FM_STATIONS - 1;
	    tuner_SetChan(tuner_GetChan(channel));
        RTOS_setTask(EVENT_RETURN_MODE, TIME_RETURN_VOLUME, 0);
        RTOS_setTask(EVENT_SAVE_CHANNEL, TIME_SAVE_PARAM, 0);
	  }
      show_lcd_main();
    break;
    case EVENT_KEY_DOWN:
      if (fm_mode == VOLUME_MODE) {
	    if (mmenu < MMENU_MAX) mmenu++; else mmenu = 0;
        RTOS_setTask(EVENT_RETURN_VOLUME, TIME_RETURN_VOLUME, 0);
	  } else {
	    if (channel < FM_STATIONS - 1) channel++; else channel = 0;
	    tuner_SetChan(tuner_GetChan(channel));
        RTOS_setTask(EVENT_RETURN_MODE, TIME_RETURN_VOLUME, 0);
        RTOS_setTask(EVENT_SAVE_CHANNEL, TIME_SAVE_PARAM, 0);
	  }
      show_lcd_main();
    break;
    case EVENT_RETURN_VOLUME:
	  mmenu = MM_VOLUME;
      show_lcd_main();
    break;
    case EVENT_KEY_LEFT:
	  tda7313Set_Param_down(mmenu);
      RTOS_setTask(EVENT_SAVE_PARAM, TIME_SAVE_PARAM, 0);
      show_lcd_main();
    break;
    case EVENT_KEY_RIGHT:
      tda7313Set_Param_up(mmenu);
      RTOS_setTask(EVENT_SAVE_PARAM, TIME_SAVE_PARAM, 0);
      show_lcd_main();
    break;
    case EVENT_KEY_LOUDNESS:
	  if (tda7313GetLoudness() == 0) tda7313SetLoudness(1); else tda7313SetLoudness(0);
      RTOS_setTask(EVENT_SAVE_LOUDNESS, TIME_SAVE_PARAM, 0);
      show_lcd_main();
    break;
    case EVENT_KEY_SET:
	  if (tda7313GetInput() == IN_3) fm_mode = !fm_mode; 
      show_lcd_main();
      RTOS_setTask(EVENT_RETURN_MODE, TIME_RETURN_VOLUME, 0);
    break;
    case EVENT_SAVE_PARAM:
      tda7313_Save_param(tda7313GetInput(), mmenu);
    break;
    case EVENT_SAVE_CHANNEL:
      tuner_Save_channel(channel);
    break;
    case EVENT_SAVE_LOUDNESS:
      tda7313_Save_Loudness(tda7313GetLoudness());	  
    break;
    case EVENT_SAVE_INPUT:
      tda7313_Save_Input(tda7313GetInput());	  
    break;
    case EVENT_RETURN_MODE:
	  fm_mode = VOLUME_MODE;
      show_lcd_main();
    break;

  }
}
//=============================================================================
void show_lcd_main(void)
{
  LCD_SetXY(0, 0); LCD_puts(input[tda7313GetInput()]);
  LCD_SetXY(17, 0);
  if (a_onoff == 1) LCD_dat(ALARM_CHAR); else LCD_dat(' ');
  LCD_SetXY(19, 0);
  if (tda7313GetLoudness()) LCD_dat(LOUDNESS_CHAR); else LCD_dat(' ');
  LCD_SetXY(6, 0); RTC_print_time();
  LCD_SetXY(0, 1);
  if (tda7313GetInput() == FM_INPUT) {
    if (fm_mode == VOLUME_MODE) {
	  LCD_puts(MSG_CHANNEL);
    } else {
	  if (blinks) LCD_puts(MSG_CHANNEL); else LCD_puts("   ");
	}
	print_dec(channel + 1, 2, '0'); LCD_dat(' '); show_freq(); LCD_puts(MSG_MHZ);
  } else {
    LCD_space(19);
  }
  LCD_SetXY(0, 2); LCD_puts(mainmenu[mmenu]);
  LCD_SetXY(18, 2); print_dec(tda7313Get_Param(mmenu), 2, '0');
  LCD_SetXY(0, 3); LCD_progress_bar(tda7313Get_Param(mmenu), tda7313Get_Max_Param(mmenu), 20);
}
//=============================================================================
void run_option(unsigned char event)
{
  switch(event) {
    case EVENT_NOKEY:
      main_mode = MODE_NOKEY;
	  nokey = 0;
      RTOS_setTask(EVENT_KEY_MENU, 0, 0);
    break;
    case EVENT_KEY_PRESS:
	  nokey = 0;
    break;
    case EVENT_KEY_STANDBY:
      stb_impuls();
      RTOS_setTask(EVENT_KEY_MENU, 0, 0);
    break;
    case EVENT_TIMER_SECOND:
      show_lcd_option();
    break;
    case EVENT_KEY_UP:
	  if (omenu > 0) omenu--; else omenu = OPTION_MAX;
      show_lcd_option();
    break;
    case EVENT_KEY_DOWN:
	  if (omenu < OPTION_MAX) omenu++; else omenu = 0;
      show_lcd_option();
    break;
    case EVENT_KEY_SET:
      nedit = 0;
      switch(omenu) {
        case OM_SET_TIME:
          RTC_get_time(&hh, &mm, &ss);
          SET_STATE(run_edit_time);
          show_lcd_edit_time();
        break;
        case OM_SET_DATE:
          RTC_get_date(&dd, &ms, &yy);
          SET_STATE(run_edit_date);
          show_lcd_edit_date();
        break;
        case OM_SET_ALARM:
          KBD_set_type(0);
          load_alarm_param();
          SET_STATE(run_edit_alarm);
          show_lcd_edit_alarm();
        break;
        case OM_SET_STANBY_OUT:
		  KBD_set_type(0);
          SET_STATE(run_edit_stb_out);
          show_lcd_edit_stb_out();
        break;
        case OM_SET_TEMP_MAX:
		  old_max_temp = max_temp;
          SET_STATE(run_edit_temp_max);
          show_lcd_edit_temp_max();
        break;
        case OM_SET_AS_TIME_ON:
		  KBD_set_type(0);
		  old_as_wait = as_wait;
          SET_STATE(run_edit_as_wait);
          show_lcd_edit_as_wait();
        break;
        case OM_SET_FM_STATIONS:
	      LCD_clear();
          channel = 0;
		  key_stb = 0;
          tda7313SetMute(1);
		  tda7313SetInput(FM_INPUT);
	      tda7313Set_Param(MM_VOLUME, 0);
          SET_STATE(run_edit_fm_stations);
          RTOS_setTask(EVENT_SET_STB_OUT_ON, 0, 0);
		  show_lcd_edit_fm_stations();
        break;
        case OM_SET_RC5_CODE:
	      LCD_clear();
		  rc5menu = KEY_STANDBY;
		  edit_rc5 = 1;
          SET_STATE(run_edit_rc5_code);
		  show_lcd_edit_rc5_code();
        break;
      }
    break;
    case EVENT_SET_STB_OUT_ON:
      if (load_stb_out() == 1) { LED_STB_OUT_0; } else { LED_STB_OUT_1; }
      RTOS_setTask(EVENT_SET_AS_WAIT_ON, (as_wait * 200), 0);
    break;
    case EVENT_SET_AS_WAIT_ON:
	  LED_AC_WAIT_on();
    break;
    case EVENT_KEY_MENU:
      main_mode = MODE_STANDBY;
	  omenu = 0;
	  LCD_clear();
      LCD_load_bignum();
      SET_STATE(run_standby);
    break;
  }
}
//=============================================================================
void show_lcd_option(void)
{
  uint8_t i;
  LCD_SetXY(0, 0); LCD_puts(MSG_MENU_OPTIONS);
  LCD_SetXY(0, 2); LCD_puts(optionmenu[omenu]);
  switch(omenu) {
    case OM_SET_TIME:
	  LCD_SetXY(10, 2); RTC_print_time(); LCD_space(3);
    break;
    case OM_SET_DATE:
	  LCD_SetXY(10, 2); RTC_print_date(); LCD_dat(' ');
    break;
    case OM_SET_ALARM:
	  LCD_SetXY(17, 1); LCD_puts(alarm_onoff[a_onoff]);
	  LCD_SetXY(12, 2);
      print_dec(a_hh,2,'0'); LCD_dat(':'); print_dec(a_mm,2,'0'); LCD_dat(':'); print_dec(0,2,'0'); LCD_dat(' ');
	  LCD_SetXY(0, 3);
      for(i = 0; i < 7; i++) {
	    if (BitIsSet(a_wday, i)) {
		  LCD_puts(den_dw[i]); 
	    } else {
		  LCD_puts("  ");
		}
		if (i < 6) LCD_dat(':');
	  }
    break;
    case OM_SET_STANBY_OUT:
	  LCD_SetXY(16, 2); print_dec(stb_out, 1, ' '); LCD_space(4);
    break;
    case OM_SET_TEMP_MAX:
	  LCD_SetXY(14, 2); print_dec(max_temp, 2, ' '); LCD_dat('C'); LCD_space(3);
    break;
    case OM_SET_AS_TIME_ON:
	  LCD_SetXY(15, 2); print_dec(as_wait, 2, ' '); LCD_puts(MSG_SEC); 
    break;
    case OM_SET_FM_STATIONS:
	  LCD_space(5);
    break;
    case OM_SET_RC5_CODE:
	  LCD_space(7);
    break;
  }
  if (omenu != OM_SET_ALARM) { 
    LCD_SetXY(0, 3); LCD_space(20);
    LCD_SetXY(16, 1); LCD_space(4);
  }
}
//=============================================================================
void run_edit_time(unsigned char event)
{
  switch(event) {
    case EVENT_NOKEY:
	  nokey = 0;
      RTOS_setTask(EVENT_KEY_MENU, 0, 0);
    break;
    case EVENT_KEY_PRESS:
	  nokey = 0;
    break;
    case EVENT_KEY_STANDBY:
      stb_impuls();
	  LCD_clear();
      LCD_load_bignum();
      SET_STATE(run_standby);
      RTOS_setTask(EVENT_TIMER_SECOND, 0, 0);
    break;
    case EVENT_TIMER_SECOND:
      show_lcd_edit_time();
    break;
    case EVENT_KEY_LEFT:
	  blinks = 1;
      show_lcd_edit_time();
	  if (nedit == 0) { if (hh > 0) hh--;  else hh = 23; } 
	  if (nedit == 1) { if (mm > 0) mm--; else mm = 59; }
	  RTOS_deleteTask(EVENT_TIMER_SECOND);
      show_lcd_edit_time();
    break;
    case EVENT_KEY_RIGHT:
	  blinks = 1;
      show_lcd_edit_time();
	  if (nedit == 0) { if (hh < 23) hh++;  else hh = 0; } 
	  if (nedit == 1) { if (mm < 59) mm++; else mm = 0; }
	  RTOS_deleteTask(EVENT_TIMER_SECOND);
      show_lcd_edit_time();
    break;
    case EVENT_KEY_SET:
	  if (nedit < 1) {
	    nedit++;
	  } else {
	    nedit = 0;
        RTC_set_time(hh, mm, 0);
        RTOS_setTask(EVENT_KEY_MENU, 0, 0);
	  }
      show_lcd_edit_time();
    break;
    case EVENT_KEY_MENU:
	  edit_rc5 = 1;
      SET_STATE(run_option);
      show_lcd_option();
    break;
  }
}
//=============================================================================
void show_lcd_edit_time(void)
{
  LCD_SetXY(10, 2);
  if (nedit == 0)
  {
	if (blinks == 1) print_dec(hh, 2, '0'); else { LCD_dat(' '); LCD_dat(' '); }
    LCD_dat(':'); print_dec(mm, 2, '0'); LCD_dat(':'); print_dec(0, 2, '0');
  } else if (nedit == 1) {
    print_dec(hh, 2, '0'); LCD_dat(':');
	if (blinks == 1) print_dec(mm, 2, '0'); else { LCD_dat(' '); LCD_dat(' '); }
    LCD_dat(':'); print_dec(0, 2, '0');
  }
}
//=============================================================================
//=============================================================================
void run_edit_date(unsigned char event)
{
  switch(event) {
    case EVENT_NOKEY:
	  nokey = 0;
      RTOS_setTask(EVENT_KEY_MENU, 0, 0);
    break;
    case EVENT_KEY_PRESS:
	  nokey = 0;
    break;
    case EVENT_KEY_STANDBY:
      stb_impuls();
	  LCD_clear();
      LCD_load_bignum();
      SET_STATE(run_standby);
    break;
    case EVENT_TIMER_SECOND:
      show_lcd_edit_date();
    break;
    case EVENT_KEY_LEFT:
	  blinks = 1;
      show_lcd_edit_date();
	  if (nedit == 0) { if (dd > 0) dd--;  else dd = 31; } 
	  if (nedit == 1) { if (ms > 0) ms--; else ms = 12; }
	  if (nedit == 2) { if (yy > 0) yy--; }
	  RTOS_deleteTask(EVENT_TIMER_SECOND);
      show_lcd_edit_date();
    break;
    case EVENT_KEY_RIGHT:
	  blinks = 1;
      show_lcd_edit_date();
	  if (nedit == 0) { if (dd < 31) dd++;  else dd = 1; } 
	  if (nedit == 1) { if (ms < 12) ms++; else ms = 1; }
	  if (nedit == 2) { if (yy < TIME_YEAR_MAX) yy++; }
	  RTOS_deleteTask(EVENT_TIMER_SECOND);
      show_lcd_edit_date();
    break;
    case EVENT_KEY_SET:
	  if (nedit < 2) {
	    nedit++;
	  } else {
	    nedit = 0;
        RTC_set_date(dd, ms, yy);
        RTOS_setTask(EVENT_KEY_MENU, 0, 0);
	  }
      show_lcd_edit_date();
    break;
    case EVENT_KEY_MENU:
      SET_STATE(run_option);
      show_lcd_option();
    break;
  }
}
//=============================================================================
void show_lcd_edit_date(void)
{
  LCD_SetXY(10, 2);
  if (nedit == 0)
  {
	if (blinks == 1) print_dec(dd, 2, '0'); else { LCD_dat(' '); LCD_dat(' '); }
    LCD_dat('/'); print_dec(ms, 2, '0'); LCD_dat('/'); print_dec(2000 + yy, 4, '0');
  } else if (nedit == 1) {
    print_dec(dd, 2, '0'); LCD_dat('/');
	if (blinks == 1) print_dec(ms, 2, '0'); else { LCD_dat(' '); LCD_dat(' '); }
    LCD_dat('/'); print_dec(2000 + yy, 4, '0');
  } else if (nedit == 2) {
    print_dec(dd, 2, '0'); LCD_dat('/'); print_dec(ms, 2, '0'); LCD_dat('/');
	if (blinks == 1) print_dec(2000 + yy, 4, '0'); else { LCD_space(4); }
  }
}
//=============================================================================
//=============================================================================
void run_edit_alarm(unsigned char event)
{
  switch(event) {
    case EVENT_NOKEY:
	  nokey = 0;
      RTOS_setTask(EVENT_KEY_MENU, 0, 0);
    break;
    case EVENT_KEY_PRESS:
	  nokey = 0;
    break;
    case EVENT_KEY_STANDBY:
      stb_impuls();
	  LCD_clear();
      LCD_load_bignum();
      SET_STATE(run_standby);
    break;
    case EVENT_TIMER_SECOND:
      show_lcd_edit_alarm();
    break;
    case EVENT_KEY_LEFT:
	  blinks = 1;
      show_lcd_edit_alarm();
      switch(nedit) {
        case ALARM_ONOFF:
		  a_onoff = !a_onoff;
        break;
        case ALARM_HOUR:
		  if (a_hh > 0) a_hh--; else a_hh = 23;
        break;
        case ALARM_MIN:
		  if (a_mm > 0) a_mm--; else a_mm = 59;
        break;
        case ALARM_MO:
        case ALARM_TU:
        case ALARM_WE:
        case ALARM_TH:
        case ALARM_FR:
        case ALARM_SA:
        case ALARM_SU:
		  if (BitIsSet(a_wday, nedit - ALARM_MO)) ClearBit(a_wday, nedit - ALARM_MO); else SetBit(a_wday, nedit - ALARM_MO);
        break;
      }
	  RTOS_deleteTask(EVENT_TIMER_SECOND);
      show_lcd_edit_alarm();
    break;
    case EVENT_KEY_RIGHT:
	  blinks = 1;
      show_lcd_edit_alarm();
      switch(nedit) {
        case ALARM_ONOFF:
		  a_onoff = !a_onoff;
        break;
        case ALARM_HOUR:
		  if (a_hh < 23) a_hh++; else a_hh = 0;
        break;
        case ALARM_MIN:
		  if (a_mm < 59) a_mm++; else a_mm = 0;
        break;
        case ALARM_MO:
        case ALARM_TU:
        case ALARM_WE:
        case ALARM_TH:
        case ALARM_FR:
        case ALARM_SA:
        case ALARM_SU:
		  if (BitIsSet(a_wday, nedit - ALARM_MO)) ClearBit(a_wday, nedit - ALARM_MO); else SetBit(a_wday, nedit - ALARM_MO);
        break;
      }
	  RTOS_deleteTask(EVENT_TIMER_SECOND);
      show_lcd_edit_alarm();
    break;
    case EVENT_KEY_SET:
	  if (nedit < ALARM_SU) {
	    if (a_onoff == 1) {
	      nedit++;
        } else {
          save_alarm_param();
          RTOS_setTask(EVENT_KEY_MENU, 0, 0);
		}
	    blinks = 1;
        show_lcd_edit_alarm();
	    RTOS_deleteTask(EVENT_TIMER_SECOND);
	  } else {
        save_alarm_param();
        RTOS_setTask(EVENT_KEY_MENU, 0, 0);
	  }
	  if (nedit >= ALARM_MO) KBD_set_type(0); else KBD_set_type(1);
      show_lcd_edit_alarm();
    break;
    case EVENT_KEY_MENU:
      show_lcd_edit_alarm();
      nokey = 0;
	  KBD_set_type(1);
      SET_STATE(run_option);
      show_lcd_option();
    break;
  }
}
//=============================================================================
void show_lcd_edit_alarm(void)
{
  uint8_t i;
  switch(nedit) {
    case ALARM_ONOFF:
      LCD_SetXY(17, 1); 
	  if (blinks) LCD_puts(alarm_onoff[a_onoff]); else LCD_space(3);
	  LCD_SetXY(12, 2);
      print_dec(a_hh,2,'0'); LCD_dat(':'); print_dec(a_mm,2,'0'); LCD_dat(':'); print_dec(0,2,'0'); LCD_dat(' ');
    break;
    case ALARM_HOUR:
	  LCD_SetXY(12, 2);
      if (blinks) print_dec(a_hh,2,'0'); else { LCD_dat(' '); LCD_dat(' '); }
	  LCD_dat(':'); print_dec(a_mm,2,'0'); LCD_dat(':'); print_dec(0,2,'0'); LCD_dat(' ');
    break;
    case ALARM_MIN:
	  LCD_SetXY(12, 2);
      print_dec(a_hh,2,'0'); LCD_dat(':');
	  if (blinks)  print_dec(a_mm,2,'0'); else { LCD_dat(' '); LCD_dat(' '); }
	  LCD_dat(':'); print_dec(0,2,'0'); LCD_dat(' ');
    break;
    case ALARM_MO:
    case ALARM_TU:
    case ALARM_WE:
    case ALARM_TH:
    case ALARM_FR:
    case ALARM_SA:
    case ALARM_SU:
      LCD_SetXY(17, 1); 
	  LCD_puts(alarm_onoff[a_onoff]);
	  LCD_SetXY(12, 2);
      print_dec(a_hh,2,'0'); LCD_dat(':'); print_dec(a_mm,2,'0'); LCD_dat(':'); print_dec(0,2,'0'); LCD_dat(' ');
	  LCD_SetXY(0, 3);
      for(i = 0; i < 7; i++) {
	    if (BitIsSet(a_wday, i)) {
		  if ((blinks == 1) || (nedit - ALARM_MO != i)) LCD_puts(den_dw[i]); else { LCD_dat(' '); LCD_dat(' '); }
	    } else {
		  if ((blinks == 1) && ((nedit - ALARM_MO) == i)) { LCD_puts(MSG_NO_WDAY); } else { LCD_dat(' '); LCD_dat(' '); }
		}
		if (i < 6) LCD_dat(':');
	  }
    break;
  }
}
//=============================================================================
//=============================================================================
void run_edit_as_wait(unsigned char event)
{
  switch(event) {
    case EVENT_NOKEY:
	  nokey = 0;
      RTOS_setTask(EVENT_KEY_MENU, 0, 0);
    break;
    case EVENT_KEY_PRESS:
	  nokey = 0;
    break;
    case EVENT_KEY_STANDBY:
      stb_impuls();
	  LCD_clear();
      LCD_load_bignum();
      SET_STATE(run_standby);
    break;
    case EVENT_TIMER_SECOND:
      show_lcd_edit_as_wait();
    break;
    case EVENT_KEY_LEFT:
	  blinks = 1;
      show_lcd_edit_as_wait();
	  if (old_as_wait > MIN_AS_WAIT) old_as_wait--;
	  RTOS_deleteTask(EVENT_TIMER_SECOND);
      show_lcd_edit_as_wait();
    break;
    case EVENT_KEY_RIGHT:
	  blinks = 1;
      show_lcd_edit_as_wait();
	  if (old_as_wait < MAX_AS_WAIT) old_as_wait++;
	  RTOS_deleteTask(EVENT_TIMER_SECOND);
      show_lcd_edit_as_wait();
    break;
    case EVENT_KEY_SET:
      nedit = 0;
	  as_wait = old_as_wait;
	  save_as_wait(as_wait);
      show_lcd_edit_as_wait();
      RTOS_setTask(EVENT_KEY_MENU, 0, 0);
    break;
    case EVENT_KEY_MENU:
	  KBD_set_type(1);
      SET_STATE(run_option);
      show_lcd_option();
    break;
  }
}
//=============================================================================
void show_lcd_edit_as_wait(void)
{
  LCD_SetXY(15, 2); 
  if (blinks) print_dec(old_as_wait, 2, ' '); else { LCD_dat(' '); LCD_dat(' '); }
  LCD_puts(MSG_SEC); 
}
//=============================================================================
//=============================================================================
void run_edit_temp_max(unsigned char event)
{
  switch(event) {
    case EVENT_NOKEY:
	  nokey = 0;
      RTOS_setTask(EVENT_KEY_MENU, 0, 0);
    break;
    case EVENT_KEY_PRESS:
	  nokey = 0;
    break;
    case EVENT_KEY_STANDBY:
      stb_impuls();
	  LCD_clear();
      LCD_load_bignum();
      SET_STATE(run_standby);
    break;
    case EVENT_TIMER_SECOND:
      show_lcd_edit_temp_max();
    break;
    case EVENT_KEY_LEFT:
	  blinks = 1;
      show_lcd_edit_temp_max();
	  if (old_max_temp > MIN_TEMP) old_max_temp--;
	  RTOS_deleteTask(EVENT_TIMER_SECOND);
      show_lcd_edit_temp_max();
    break;
    case EVENT_KEY_RIGHT:
	  blinks = 1;
      show_lcd_edit_temp_max();
	  if (old_max_temp < MAX_TEMP) old_max_temp++;
	  RTOS_deleteTask(EVENT_TIMER_SECOND);
      show_lcd_edit_temp_max();
    break;
    case EVENT_KEY_SET:
      nedit = 0;
	  max_temp = old_max_temp;
      save_max_temperature(max_temp);
      show_lcd_edit_temp_max();
      RTOS_setTask(EVENT_KEY_MENU, 0, 0);
    break;
    case EVENT_KEY_MENU:
	  KBD_set_type(1);
      SET_STATE(run_option);
      show_lcd_option();
    break;
  }
}
//=============================================================================
void show_lcd_edit_temp_max(void)
{
  LCD_SetXY(14, 2); 
  if (blinks) print_dec(old_max_temp, 2, ' '); else { LCD_dat(' '); LCD_dat(' '); }
  LCD_dat('C'); LCD_space(3);
}
//=============================================================================
//=============================================================================
void run_edit_stb_out(unsigned char event)
{
  switch(event) {
    case EVENT_NOKEY:
	  nokey = 0;
      RTOS_setTask(EVENT_KEY_MENU, 0, 0);
    break;
    case EVENT_KEY_PRESS:
	  nokey = 0;
    break;
    case EVENT_KEY_STANDBY:
      stb_impuls();
	  LCD_clear();
      LCD_load_bignum();
      if (load_stb_out() == 1) { LED_STB_OUT_1; } else { LED_STB_OUT_0; }
      SET_STATE(run_standby);
    break;
    case EVENT_TIMER_SECOND:
      show_lcd_edit_stb_out();
    break;
    case EVENT_KEY_LEFT:
	  blinks = 1;
      show_lcd_edit_stb_out();
	  stb_out = !stb_out;
	  RTOS_deleteTask(EVENT_TIMER_SECOND);
      show_lcd_edit_stb_out();
    break;
    case EVENT_KEY_RIGHT:
	  blinks = 1;
      show_lcd_edit_stb_out();
	  stb_out = !stb_out;
	  RTOS_deleteTask(EVENT_TIMER_SECOND);
      show_lcd_edit_stb_out();
    break;
    case EVENT_KEY_SET:
	  save_stb_out(stb_out);
      nedit = 0;
      RTOS_setTask(EVENT_KEY_MENU, 0, 0);
      show_lcd_edit_stb_out();
      if (load_stb_out() == 1) { LED_STB_OUT_1; } else { LED_STB_OUT_0; }
    break;
    case EVENT_KEY_MENU:
	  KBD_set_type(1);
      SET_STATE(run_option);
      show_lcd_option();
      if (load_stb_out() == 1) { LED_STB_OUT_1; } else { LED_STB_OUT_0; }
    break;
  }
}
//=============================================================================
void show_lcd_edit_stb_out(void)
{
  LCD_SetXY(16, 2); 
  if (blinks == 1) print_dec(stb_out, 1, ' '); else LCD_dat(' ');
}
//=============================================================================
//=============================================================================
void run_edit_fm_stations(unsigned char event)
{
  switch(event) {
    case EVENT_SET_STB_OUT_ON:
      if (load_stb_out() == 1) { LED_STB_OUT_0; } else { LED_STB_OUT_1; }
      RTOS_setTask(EVENT_SET_AS_WAIT_ON, 1000, 0);
    break;
    case EVENT_SET_AS_WAIT_ON:
      LED_AC_WAIT_on();
      tuner_SetChan(tuner_GetChan(channel));
	  tda7313Set_Param(MM_VOLUME, tda7313_Load_vol_fm_edit());
      tda7313SetMute(0);
      show_lcd_edit_fm_stations();
    break;
    case EVENT_SET_AS_WAIT_OFF:
	  tuner_SetMute(1);
      LED_AC_WAIT_off();
      RTOS_setTask(EVENT_SET_STB_OUT_OFF, 100, 0);
    break;
    case EVENT_SET_STB_OUT_OFF:
      if (load_stb_out() == 1) { LED_STB_OUT_1; } else { LED_STB_OUT_0; }
      RTOS_setTask(EVENT_STOP_FM_EDIT, 0, 0);
    break;
    case EVENT_NOKEY:
	  if (nedit == FM_NORMAL_MODE) {
        RTOS_setTask(EVENT_SET_AS_WAIT_OFF, 0, 0);
      } else {
	    nedit = FM_NORMAL_MODE;
	    nokey = 0;
	  }
      show_lcd_edit_fm_stations();
    break;
    case EVENT_RETURN_MODE:
	  if (nedit == FM_EDIT_MODE) {
	    tuner_SetChannel(channel, old_chan);
	    tuner_SetChan(old_chan);
	    nedit = FM_NORMAL_MODE;
	  }
      show_lcd_edit_fm_stations();
    break;
    case EVENT_KEY_PRESS:
	  nokey = 0;
    break;
    case EVENT_KEY_STANDBY:
      stb_impuls();
	  key_stb = 1;
      RTOS_setTask(EVENT_SET_AS_WAIT_OFF, 0, 0);
    break;
    case EVENT_TIMER_SECOND:
      show_lcd_edit_fm_stations();
    break;
    case EVENT_KEY_UP:
	  if (nedit == FM_NORMAL_MODE) {
	    if (channel > 0) channel--; else channel = FM_STATIONS - 1;
	    tuner_SetChan(tuner_GetChan(channel));
	  }
      show_lcd_edit_fm_stations();
    break;
    case EVENT_KEY_DOWN:
	  if (nedit == FM_NORMAL_MODE) {
	    if (channel < FM_STATIONS - 1) channel++; else channel = 0;
	    tuner_SetChan(tuner_GetChan(channel));
	  }
      show_lcd_edit_fm_stations();
    break;
    case EVENT_KEY_LEFT:
	  blinks = 1;
      show_lcd_edit_fm_stations();
	  if (nedit == FM_NORMAL_MODE) {
	    tda7313Set_Param_down(mmenu);
        RTOS_setTask(EVENT_SAVE_PARAM, TIME_SAVE_PARAM, 0);
	  } else {
        if ( tuner_GetChan(channel) > FM_CHAN_MIN) tuner_SetChan(tuner_GetChan(channel) - 1);
		tuner_SetChannel(channel, tuner_GetChan(channel) - 1);
        RTOS_setTask(EVENT_RETURN_MODE, TIME_RETURN_VOLUME, 0);
	  }
	  RTOS_deleteTask(EVENT_TIMER_SECOND);
      show_lcd_edit_fm_stations();
    break;
    case EVENT_KEY_RIGHT:
	  blinks = 1;
      show_lcd_edit_fm_stations();
	  if (nedit == FM_NORMAL_MODE) {
	    tda7313Set_Param_up(mmenu);
        RTOS_setTask(EVENT_SAVE_PARAM, TIME_SAVE_PARAM, 0);
	  } else {
        if ( tuner_GetChan(channel) < FM_CHAN_MAX) tuner_SetChan(tuner_GetChan(channel) + 1);
		tuner_SetChannel(channel, tuner_GetChan(channel) + 1);
        RTOS_setTask(EVENT_RETURN_MODE, TIME_RETURN_VOLUME, 0);
	  }
	  RTOS_deleteTask(EVENT_TIMER_SECOND);
      show_lcd_edit_fm_stations();
    break;
    case EVENT_SAVE_PARAM:
      tda7313_Save_vol_fm_edit(tda7313Get_Param(0));
    break;
    case EVENT_KEY_SELECT:
	  tuner_SetChannel(channel, 0xFF);
      tuner_SetChan(0xFF);
	  nedit = FM_NORMAL_MODE;
      tuner_Save_stations(channel, 0xFF);
      show_lcd_edit_fm_stations();
    break;
    case EVENT_KEY_SET:
	  if (nedit == FM_NORMAL_MODE) {
	    old_chan = tuner_GetChan(channel);
	    if (old_chan > FM_CHAN_MAX) {
		  tuner_SetChannel(channel, get_fm_def_chan());
	      tuner_SetChan(get_fm_def_chan());
		}
        RTOS_setTask(EVENT_RETURN_MODE, TIME_RETURN_VOLUME, 0);
        show_lcd_edit_fm_stations();
	  }
	  if (nedit == FM_EDIT_MODE) {
        tuner_Save_stations(channel, tuner_GetChan(channel));
	  }
      nedit = !nedit;
    break;
    case EVENT_KEY_MENU:
      RTOS_setTask(EVENT_SET_AS_WAIT_OFF, 0, 0);
    break;
    case EVENT_STOP_FM_EDIT:
      LCD_clear();
      channel = tuner_Load_channel();
	  tda7313SetInput(tda7313_Load_Input());
      tda7313Set_Param_no_send(TDA_VOLUME, tda7313_Load_param(tda7313GetInput(), TDA_VOLUME)), 
      tda7313SetMute(1);
      tuner_Save_all_stations();
	  if (key_stb == 1) {
        key_stb = 0;
        LCD_load_bignum();
        SET_STATE(run_standby);
	  } else {
        SET_STATE(run_option);
        show_lcd_option();
	  }
    break;
  }
}
//=============================================================================
void show_lcd_edit_fm_stations(void)
{
  LCD_SetXY(0, 0); LCD_puts(optionmenu[OM_SET_FM_STATIONS]);
  LCD_SetXY(0, 1); LCD_puts(MSG_CHANNEL); print_dec(channel + 1, 2, '0'); LCD_dat(' ');
  show_freq(); if ((nedit == FM_NORMAL_MODE) || (blinks == 1)) LCD_puts(MSG_MHZ); else LCD_space(3);
  LCD_SetXY(0, 2); LCD_puts(mainmenu[MM_VOLUME]);
  LCD_SetXY(18, 2); print_dec(tda7313Get_Param(mmenu), 2, '0');
  LCD_SetXY(0, 3); LCD_progress_bar(tda7313Get_Param(mmenu), tda7313Get_Max_Param(mmenu), 20);
}
//=============================================================================
//=============================================================================
void run_alarm(unsigned char event)
{
  switch(event) {
    case EVENT_KEY_PRESS:
	  nokey = 0;
	  LCD_clear();
      LCD_load_defchar();
	  time_alarm = 0;
      SET_STATE(run_main);
    break;
    case EVENT_KEY_STANDBY:
      stb_impuls();
	  LCD_clear();
      LCD_load_bignum();
      tda7313SetMute(1);
      LED_STANDBY_on();
      SET_STATE(run_standby);
    break;
    case EVENT_TIMER_SECOND:
	  nokey = 0;
      if (time_alarm > 0) {
		time_alarm--;
      } else {
	    LCD_clear();
        LCD_load_defchar();
        SET_STATE(run_main);
	  }
    break;
	default:
    break;
  }
}
//=============================================================================
uint8_t check_alarm(void)
{
  unsigned char h,m,s;
  if (a_onoff == 1) {
    if (BitIsSet(a_wday, RTC_dayofweek())) {
      RTC_get_time(&h,&m,&s);
	  if ((a_hh == h) && (a_mm == m) && (s == 0)) {
        return 1;
	  }
	} 
  }
  return 0;
}
//=============================================================================
//=============================================================================
void run_edit_rc5_code(unsigned char event)
{
  if (event < EVENT_RC5_CODE_MAX) {
    blinks = 1;
    rc5_cmd = event;
    show_lcd_edit_rc5_code();
  }
  switch(event) {
    case EVENT_NOKEY:
	  nokey = 0;
      RTOS_setTask(EVENT_KEY_STANDBY, 0, 0);
    break;
    case EVENT_KEY_PRESS:
	  nokey = 0;
    break;
    case EVENT_KEY_STANDBY:
      stb_impuls();
      edit_rc5 = 0;
	  LCD_clear();
      LCD_load_bignum();
      SET_STATE(run_standby);
    break;
    case EVENT_TIMER_SECOND:
      show_lcd_edit_rc5_code();
    break;
    case EVENT_KEY_UP:
	  if (rc5menu > 0) rc5menu--; else rc5menu = RC5_MENU_MAX;
      show_lcd_edit_rc5_code();
    break;
    case EVENT_KEY_DOWN:
	  if (rc5menu < RC5_MENU_MAX) rc5menu++; else rc5menu = 0;
      show_lcd_edit_rc5_code();
    break;
    case EVENT_KEY_SET:
	  save_rc5code_key(rc5menu, rc5_cmd);
      LCD_SetXY(4, 3); LCD_puts(MSG_SAVE_RC5);
      RTOS_setTask(EVENT_CLEAR, 500, 0);
      rc5_cmd = 0xFF;
    break;
    case EVENT_KEY_MENU:
	  save_rc5code();
      edit_rc5 = 0;
      SET_STATE(run_option);
      show_lcd_option();
    break;
    case EVENT_CLEAR:
      LCD_SetXY(0, 3); LCD_space(20);
    break;
  }
}
//=============================================================================
void show_lcd_edit_rc5_code(void)
{
  LCD_SetXY(0, 0); LCD_puts(optionmenu[OM_SET_RC5_CODE]);
  LCD_SetXY(0, 2); LCD_puts(rc5_menu[rc5menu]);
  LCD_SetXY(14, 2); 
  if (rc5_code[RC5_CMD][rc5menu] == 0xFF) {
    LCD_puts(MSG_NO_RC5);
  } else {
    print_dec(rc5_code[RC5_CMD][rc5menu], 2, '0');
  }
  LCD_dat(':');
  if (blinks == 1) {
    if (rc5_cmd == 0xFF) {
      LCD_puts(MSG_NO_RC5);
    } else {
      print_dec(rc5_cmd, 2, '0');
    }
  } else {
    LCD_space(2);
  } 
}
//=============================================================================

