#include <avr/eeprom.h>
#include "i2c.h"
#include "tda7313.h"
//=============================================================================
static uint8_t _input = 2, _loudness = 1;
unsigned char tda7313_param[TDA7313_IN_CNT][TDA_MAX_PARAM] = { {26,3,8,8,31,31,31,31}, {18,3,4,10,31,31,31,31}, {18,3,4,12,31,31,31,31} };
unsigned char tda7313_attens[] = { 0x18,0x10,0x08,0x00 };
unsigned char tda7313_max_param[] = { 31, 3, 15, 15, 31, 31, 31, 31};
unsigned char tda7313_bass_treb[] = { 0,1,2,3,4,5,6,7,15,14,13,12,11,10,9,8 };
unsigned char tda7313_def_param[] = { TDA_VOLUME_DEF, TDA_ATTENS_ALL_DEF, TDA_BASS_DEF, TDA_TREBLE, TDA_ATTENS_FL_DEF, TDA_ATTENS_FR_DEF, TDA_ATTENS_RL_DEF, TDA_ATTENS_RR_DEF};
//=============================================================================
uint8_t tda7313GetInput(void)
{
  return _input;
}
//=============================================================================
uint8_t tda7313GetLoudness(void)
{
  return _loudness;
}
//=============================================================================
void tda7313SetLoudness(uint8_t loudness)
{
  _loudness = loudness;
  tda7313Send_Param();
}
//=============================================================================
void tda7313SetInput(uint8_t in)
{
  _input = in;
  tda7313Send_Param();
}
//=============================================================================
uint8_t tda7313Get_Param(uint8_t param)
{
  return tda7313_param[_input][param];
}
//=============================================================================
void tda7313Set_Param_no_send(uint8_t param, uint8_t val)
{
  tda7313_param[_input][param] = val;
}
//=============================================================================
void tda7313Set_Param(uint8_t param, uint8_t val)
{
  tda7313_param[_input][param] = val;
  tda7313Send_Param();
}
//=============================================================================
void tda7313Set_Param_up(uint8_t param)
{
  if (tda7313_param[_input][param] < tda7313_max_param[param]) tda7313_param[_input][param] = tda7313_param[_input][param] + 1;
  tda7313Send_Param();
}
//=============================================================================
void tda7313Set_Param_down(uint8_t param)
{
  if (tda7313_param[_input][param] > 0) tda7313_param[_input][param] = tda7313_param[_input][param] - 1;
  tda7313Send_Param();
}
//=============================================================================
uint8_t tda7313Get_Max_Param(uint8_t param)
{
  return tda7313_max_param[param];
}
//=============================================================================
void tda7313Send_Param(void)
{
  unsigned char sw_reg = TDA7313_SW, val;
  // Volume
  I2C_start();
  I2C_write(TDA7313_I2C_ADDR);
  val = tda7313_param[_input][TDA_VOLUME];
  I2C_write(TDA7313_VOLUME + 0x3F - (val + val));
  I2C_stop();
  // Attens
  val = tda7313_param[_input][TDA_ATTENS_ALL];
  sw_reg += tda7313_attens[val] + _input;
  if (_loudness) sw_reg &= ~(1 << TDA7313_LONDNESS_BIT); else sw_reg |= (1 << TDA7313_LONDNESS_BIT);
  I2C_start();
  I2C_write(TDA7313_I2C_ADDR);
  I2C_write(sw_reg);
  I2C_stop();
  // Attens Front Left
  val = tda7313_param[_input][TDA_ATTENS_FL];
  I2C_start();
  I2C_write(TDA7313_I2C_ADDR);
  I2C_write(TDA7313_FRONT_LEFT + (TDA7313_MAX_ATTENS - val));
  I2C_stop();
  // Attens Front Right
  val = tda7313_param[_input][TDA_ATTENS_FR];
  I2C_start();
  I2C_write(TDA7313_I2C_ADDR);
  I2C_write(TDA7313_FRONT_RIGHT + (TDA7313_MAX_ATTENS - val));
  I2C_stop();
  // Attens Rear Left
  val = tda7313_param[_input][TDA_ATTENS_RL];
  I2C_start();
  I2C_write(TDA7313_I2C_ADDR);
  I2C_write(TDA7313_REAR_LEFT + (TDA7313_MAX_ATTENS - val));
  I2C_stop();
  // Attens Rear Right
  val = tda7313_param[_input][TDA_ATTENS_RR];
  I2C_start();
  I2C_write(TDA7313_I2C_ADDR);
  I2C_write(TDA7313_REAR_RIGHT + (TDA7313_MAX_ATTENS - val));
  I2C_stop();
  // Bass
  val = tda7313_param[_input][TDA_BASS];
  I2C_start();
  I2C_write(TDA7313_I2C_ADDR);
  I2C_write(TDA7313_BASS + tda7313_bass_treb[val]);
  I2C_stop();
  // Treble
  val = tda7313_param[_input][TDA_TREBLE];
  I2C_start();
  I2C_write(TDA7313_I2C_ADDR);
  I2C_write(TDA7313_TREBLE + tda7313_bass_treb[val]);
  I2C_stop();
}
//=============================================================================
void tda7313SetMute(uint8_t val)
{
  if (val) {
    I2C_start();
    I2C_write(TDA7313_I2C_ADDR);
    I2C_write(TDA7313_FRONT_LEFT | TDA7313_MUTE);
    I2C_write(TDA7313_FRONT_RIGHT | TDA7313_MUTE);
    I2C_write(TDA7313_REAR_LEFT | TDA7313_MUTE);
    I2C_write(TDA7313_REAR_RIGHT | TDA7313_MUTE);
    I2C_stop();
  } else {
    tda7313Send_Param();
  }
}
//=============================================================================
void tda7313_Save_all_param(void)
{
  unsigned char i;
  for(i = 0; i < TDA_MAX_PARAM; i++)
  {
    tda7313_Save_param(0, i);
    tda7313_Save_param(1, i);
    tda7313_Save_param(2, i);
  }
  tda7313_Save_Input(_input);
  tda7313_Save_Loudness(_loudness);
}
//=============================================================================
void tda7313_Load_all_param(void)
{
  unsigned char i, param;
  for(i = 0; i < TDA_MAX_PARAM; i++)
  {
    param = tda7313_Load_param(0, i);
	if (param > tda7313_max_param[i]) tda7313_param[0][i] = tda7313_def_param[i]; else tda7313_param[0][i] = param;
    param = tda7313_Load_param(1, i);
	if (param > tda7313_max_param[i]) tda7313_param[1][i] = tda7313_def_param[i]; else tda7313_param[1][i] = param;
    param = tda7313_Load_param(2, i);
	if (param > tda7313_max_param[i]) tda7313_param[2][i] = tda7313_def_param[i]; else tda7313_param[2][i] = param;
  }
  tda7313_Load_Input();
  tda7313_Load_Loudness();
}
//=============================================================================
void tda7313_Save_Input(uint8_t in)
{
  eeprom_write_byte((uint8_t*)TDA_INPUT_ADDR, in);
}
//=============================================================================
uint8_t tda7313_Load_Input(void)
{
  _input = eeprom_read_byte((uint8_t*)TDA_INPUT_ADDR);
  if (_input > TDA7313_IN_CNT - 1) _input = 0;
  return _input;
}
//=============================================================================
void tda7313_Save_Loudness(uint8_t loud)
{
  eeprom_write_byte((uint8_t*)TDA_LOUDNESS_ADDR, loud);
}
//=============================================================================
uint8_t tda7313_Load_Loudness(void)
{
  _loudness = eeprom_read_byte((uint8_t*)TDA_LOUDNESS_ADDR);
  if (_loudness > 2) _loudness = 1;
  return _loudness;
}
//=============================================================================
void tda7313_Save_param(uint8_t in, uint8_t param)
{
  switch (in)
  {
    case 0:
      eeprom_write_byte((uint8_t*)(TDA_PARAM_ADDR_IN1 + param), tda7313_param[0][param]);
    break;
    case 1:
      eeprom_write_byte((uint8_t*)(TDA_PARAM_ADDR_IN2 + param), tda7313_param[1][param]);
    break;
    case 2:
      eeprom_write_byte((uint8_t*)(TDA_PARAM_ADDR_IN3 + param), tda7313_param[2][param]);
    break;
  }
}
//=============================================================================
uint8_t tda7313_Load_param(uint8_t in, uint8_t param)
{
  uint8_t ret = 0;
  switch (in)
  {
    case 0:
      ret = eeprom_read_byte((uint8_t*)(TDA_PARAM_ADDR_IN1 + param));
    break;
    case 1:
      ret = eeprom_read_byte((uint8_t*)(TDA_PARAM_ADDR_IN2 + param));
    break;
    case 2:
      ret = eeprom_read_byte((uint8_t*)(TDA_PARAM_ADDR_IN3 + param));
    break;
  }
  return ret;
}
//=============================================================================
void tda7313_Save_vol_fm_edit(uint8_t vol)
{
  eeprom_write_byte((uint8_t*)FM_VOLUME_EDIT_ADDR, vol);
}
//=============================================================================
uint8_t tda7313_Load_vol_fm_edit(void)
{
  uint8_t v;
  v = eeprom_read_byte((uint8_t*)FM_VOLUME_EDIT_ADDR);
  if (v > tda7313_max_param[TDA_VOLUME]) {
    v = TDA_VOLUME_DEF;
    tda7313_Save_vol_fm_edit(v);
  }
  return v;
}
//=============================================================================

