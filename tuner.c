#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "pins.h"
#include "i2c.h"
#include "lcd_lib.h"
#include "tuner.h"
//=============================================================================
uint8_t fm_stations[FM_STATIONS + 1];
//=============================================================================
#if (TUNER == RDA5807M)
#include "rda5807.c"
#endif
//=============================================================================
#if (TUNER == TEA5767)
#include "tea5767.c"
#endif
//=============================================================================
#if (TUNER == TUX032)
#endif
//=============================================================================
void tuner_Init_stations(void)
{
  uint8_t i;
  for (i = 0; i < FM_STATIONS; i++) fm_stations[i] = 0xFF;
}
//=============================================================================
uint16_t tuner_ChanToFreq(uint8_t chan)
{
  return (FM_FREQ_MIN + (chan * 10));
}
//=============================================================================
uint8_t tuner_FreqToChan(uint16_t freq)
{
  return ((freq - FM_FREQ_MIN) / 10);
}
//=============================================================================
uint8_t tuner_GetChan(uint8_t channel)
{
  if (channel < FM_STATIONS) {
    return fm_stations[channel];
  } else return 0;
}
//=============================================================================
void tuner_SetChan(uint8_t chan)
{
  if (chan <= FM_CHAN_MAX) {
    tuner_SetMute(0);
    tuner_SetFreq(tuner_ChanToFreq(chan), 0);
  } else {
    tuner_SetMute(1);
  }
}
//=============================================================================
void tuner_SetChannel(uint8_t channel, uint8_t chan)
{
  if ((channel < FM_STATIONS) && (chan <= FM_CHAN_MAX)) {
    fm_stations[channel] = chan;
  } else {
    fm_stations[channel] = 0xFF;
  }
}
//=============================================================================
void tuner_Save_stations(uint8_t channel, uint8_t chan)
{
  if ((channel < FM_STATIONS) && (chan <= FM_CHAN_MAX)) {
    fm_stations[channel] = chan;
    eeprom_write_byte((uint8_t*)FM_CHANEL_ALL_ADDR + channel, fm_stations[channel]);
  }
}
//=============================================================================
uint8_t tuner_Load_stations(uint8_t channel)
{
  return eeprom_read_byte((uint8_t*)FM_CHANEL_ALL_ADDR + channel);
}
//=============================================================================
void tuner_Save_all_stations(void)
{
  unsigned char i;
  for(i = 0; i < FM_STATIONS; i++)
  {
    eeprom_write_byte((uint8_t*)FM_CHANEL_ALL_ADDR + i, fm_stations[i]);
  }
}
//=============================================================================
void tuner_Load_all_stations(void)
{
  unsigned char i;
  for(i = 0; i < FM_STATIONS; i++)
  {
    fm_stations[i] = eeprom_read_byte((uint8_t*)FM_CHANEL_ALL_ADDR + i);
  }
}
//=============================================================================
void tuner_Save_channel(uint8_t channel)
{
  eeprom_write_byte((uint8_t*)FM_STATION_ADDR, channel);
}
//=============================================================================
uint8_t tuner_Load_channel(void)
{
  unsigned char i;
  i = eeprom_read_byte((uint8_t*)FM_STATION_ADDR);
  if (i > FM_STATIONS) i = 0;
  return i;
}
//=============================================================================
void tuner_Save_volume(uint8_t vol)
{
  eeprom_write_byte((uint8_t*)FM_VOLUME_ADDR, vol);
}
//=============================================================================
uint8_t tuner_Load_volume(void)
{
  return eeprom_read_byte((uint8_t*)FM_VOLUME_ADDR);
}
//=============================================================================
uint8_t get_fm_def_chan(void)
{
  uint8_t ret = FM_CHAN_DEF, chan_max = 0, i;
  for(i = 0; i < FM_STATIONS; i++)
  {
    if (fm_stations[i] <= FM_CHAN_MAX) {
	  if (fm_stations[i] >= chan_max) {
	    chan_max = fm_stations[i];
	    ret = fm_stations[i];
	  }
	}
  }
  return ret;
}
//=============================================================================
