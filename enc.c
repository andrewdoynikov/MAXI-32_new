#include <avr/io.h>
#include "pins.h"
#include "enc.h"
#include "rtos.h"
#include "events.h"
//=============================================================================
void ENC_init(void)
{
  DDR(ENCODER) &= ~(1 << ENCODER_PIN_A);
  DDR(ENCODER) &= ~(1 << ENCODER_PIN_B);
  PORT(ENCODER) |= ENCODER_LINE_A;
  PORT(ENCODER) |= ENCODER_LINE_B;
}
//=============================================================================
void ENC_scan(void)
{
  static unsigned char stateEnc; 	//хранит последовательность состояний энкодера
  unsigned char tmp, key;  
  unsigned char currentState = 0;
  //проверяем состояние выводов микроконтроллера
  if ((PIN(ENCODER) & (ENCODER_LINE_A)) != 0) { currentState |= (1 << 0); }
  if ((PIN(ENCODER) & (ENCODER_LINE_B)) != 0) { currentState |= (1 << 1); }
  //если равно предыдущему, то выходим
  tmp = stateEnc;
  if (currentState == (tmp & ENC_MASK)) 
  {
    return;
  }
  //если не равно, то сдвигаем и сохраняем в озу
  tmp = (tmp << 2) | currentState;
  stateEnc = tmp;
  //сравниваем получившуюся последовательность
  if (tmp == ENC_LEFT_MASK) { key = ENC_LEFT; BEEP(); RTOS_setTask(EVENT_KEY_PRESS, 0, 0); RTOS_setTask(key, 0, 0); }
  if (tmp == ENC_RIGHT_MASK) { key = ENC_RIGHT;  BEEP(); RTOS_setTask(EVENT_KEY_PRESS, 0, 0); RTOS_setTask(key, 0, 0); }	 
}
//=============================================================================
