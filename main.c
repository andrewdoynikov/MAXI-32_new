//************************************************************************************************
// Project MAXI32 TIMER SERVICE  
// Author: Andrew Doynikov
// 5.12.2015	
//************************************************************************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "pins.h"
#include "rtos.h"
#include "events.h"
#include "i2c.h"
#include "lcd_lib.h"
#include "ds1307.h"
#include "kbd.h"
#include "enc.h"
#include "rc5.h"
#include "ds18x20.h"
#include "tda7313.h"
#include "tuner.h"
#include "interface.h"
//=============================================================================
int main(void)
{
  LED_STB_OUT_init();
  if (load_stb_out() == 1) { LED_STB_OUT_1; } else { LED_STB_OUT_0; }
  LED_AC_WAIT_init();
  LED_VENTIL_init();
  LED_STANDBY_init();
  LED_STANDBY_on();
  LCD_init();
  LCD_load_defchar();
  LIGHT_init();
  LIGHT_on();
  I2C_init();
  RC5_init(RC5_ALL);
  RTC_init();
  KBD_init();
  BEEP_init();
  ENC_init();
  LED_RC5_CMD_init();
  tda7313SetMute(1);
  tuner_Init();
  tuner_PowerOn();
  RTOS_init();
  BEEP();
//-----------------------------------------------------------------------------
  ds18x20SearchDevices();
  ds18x20Process();
//-----------------------------------------------------------------------------
  sei(); 				        			 		// Разрешили прерывания
  RTOS_setTask(EVENT_START, 0, 0);
  RTOS_setTaskFunc(scan_fault, 0, TIME_FAULT);      // моргание
//-----------------------------------------------------------------------------
  while(1) {
    RTOS_dispatchTask();    			     // Вызываем диспетчер в цикле.    
  }
}
//=============================================================================
