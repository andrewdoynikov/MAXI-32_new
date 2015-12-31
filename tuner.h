#ifndef TUNER_H
#define TUNER_H
//=============================================================================
#include "pins.h"
//=============================================================================
#define FM_FREQ_MIN					8700
#define FM_FREQ_MAX					10800
#define FM_FREQ_DEF					10000
#define FM_CHAN_MIN	  				0
#define FM_CHAN_MAX	  				210
#define FM_CHAN_DEF	  				0
//=============================================================================
uint16_t tuner_ChanToFreq(uint8_t chan);
uint8_t tuner_FreqToChan(uint16_t freq);
uint8_t tuner_GetChan(uint8_t channel);
void tuner_SetChannel(uint8_t channel, uint8_t chan);
void tuner_SetChan(uint8_t chan);
void tuner_Init_stations(void);
void tuner_Save_all_stations(void);
void tuner_Load_all_stations(void);
void tuner_Save_channel(uint8_t channel);
uint8_t tuner_Load_channel(void);
void tuner_Save_volume(uint8_t vol);
uint8_t tuner_Load_volume(void);
void tuner_Save_stations(uint8_t channel, uint8_t chan);
uint8_t tuner_Load_stations(uint8_t channel);
uint8_t get_fm_def_chan(void);
//=============================================================================
#if (TUNER == RDA5807M)
#include "rda5807.h"
#define tuner_Init(); 				rda5807Init(); tuner_Init_stations();
#define tuner_SetFreq(freq, mono); 	rda5807SetFreq(freq, mono);
#define tuner_ReadStatus();			rda5807ReadStatus();
#define tuner_SetMute(mute);		rda5807SetMute(mute);
#define tuner_SetVolume(value);		rda5807SetVolume(value);
#define tuner_PowerOn();			rda5807PowerOn();
#define tuner_PowerOff();			rda5807PowerOff();
#endif
//=============================================================================
#if (TUNER == TEA5767)
#include "tea5767.h"
#define tuner_Init(); 				tea5767Init(0x79); tuner_Init_stations();
#define tuner_SetFreq(freq, mono); 	tea5767SetFreq(freq, mono);
#define tuner_ReadStatus();			tea5767ReadStatus();
#define tuner_SetMute(mute);		tea5767SetMute(mute);
#define tuner_SetVolume(value);		asm("nop");
#define tuner_PowerOn();			tea5767PowerOn();
#define tuner_PowerOff();			tea5767PowerOff();
#endif
//=============================================================================
#if (TUNER == TUX032)
#endif
//=============================================================================
#endif
