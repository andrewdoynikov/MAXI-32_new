#ifndef TDA7313_H
#define TDA7313_H
//=============================================================================
#include <avr/io.h>
//=============================================================================
#define TDA7313_I2C_ADDR			0x88

#define TDA7313_VOLUME				0x00
#define TDA7313_FRONT_LEFT			0x80
#define TDA7313_FRONT_RIGHT			0xA0
#define TDA7313_REAR_LEFT			0xC0
#define TDA7313_REAR_RIGHT			0xE0
#define TDA7313_MUTE				0x1F
#define TDA7313_SW					0x40
#define TDA7313_BASS				0x60
#define TDA7313_TREBLE				0x70
#define TDA7313_LONDNESS_BIT	   	2
#define TDA7313_MAX_ATTENS		   	31

#define TDA7313_IN_CNT				3

#define TDA_VOLUME					0
#define TDA_ATTENS_ALL				1
#define TDA_BASS					2
#define TDA_TREBLE					3
#define TDA_ATTENS_FL				4
#define TDA_ATTENS_FR				5
#define TDA_ATTENS_RL				6
#define TDA_ATTENS_RR				7

#define TDA_VOLUME_DEF				24
#define TDA_ATTENS_ALL_DEF			3
#define TDA_BASS_DEF				8
#define TDA_TREBLE_DEF				8
#define TDA_ATTENS_FL_DEF			31
#define TDA_ATTENS_FR_DEF			31
#define TDA_ATTENS_RL_DEF			31
#define TDA_ATTENS_RR_DEF			31

#define TDA_MAX_PARAM				8

uint8_t tda7313GetLoudness(void);
void tda7313SetLoudness(uint8_t loudness);
uint8_t tda7313GetInput(void);
void tda7313SetInput(uint8_t in);
void tda7313Send_Param(void);
void tda7313Set_Param(uint8_t param, uint8_t val);
void tda7313Set_Param_no_send(uint8_t param, uint8_t val);
void tda7313Set_Param_up(uint8_t param);
void tda7313Set_Param_down(uint8_t param);
uint8_t tda7313Get_Param(uint8_t param);
uint8_t tda7313Get_Max_Param(uint8_t param);
void tda7313SetMute(uint8_t val);
//=============================================================================
void tda7313_Save_all_param(void);
void tda7313_Load_all_param(void);
void tda7313_Save_Input(uint8_t in);
uint8_t tda7313_Load_Input(void);
void tda7313_Save_Loudness(uint8_t loud);
uint8_t tda7313_Load_Loudness(void);
void tda7313_Save_param(uint8_t in, uint8_t param);
uint8_t tda7313_Load_param(uint8_t in, uint8_t param);
void tda7313_Save_vol_fm_edit(uint8_t vol);
uint8_t tda7313_Load_vol_fm_edit(void);
//=============================================================================
#endif /* TDA7313_H */
