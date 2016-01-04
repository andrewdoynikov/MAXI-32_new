#ifndef INTERFACE_H
#define INTERFACE_H
//=============================================================================
#define VOLUME_MODE			0
#define CHANNEL_MODE		1
#define FM_NORMAL_MODE		0
#define FM_EDIT_MODE		1
//=============================================================================
#define Bit(bit)  (1<<(bit))
#define ClearBit(reg, bit) 	     	reg &= (~(1<<(bit)))
#define SetBit(reg, bit)        	reg |= (1<<(bit))	
#define SetBitVal(reg, bit, val)	do { if ((val & 1) == 0) reg &= (~(1<<(bit))); else reg |= (1<<(bit));} while(0)
#define BitIsClear(reg, bit)    	((reg & (1<<(bit))) == 0)
#define BitIsSet(reg, bit)      	((reg & (1<<(bit))) != 0)
#define InvBit(reg, bit)	  		reg ^= (1<<(bit))
//=============================================================================
void run_start(unsigned char event);
void run_stop(unsigned char event);
void run_standby(unsigned char event);
void run_nokey(unsigned char event);
void run_main(unsigned char event);
void run_mute(unsigned char event);
void run_option(unsigned char event);
void run_main(unsigned char event);
void run_edit_time(unsigned char event);
void run_edit_date(unsigned char event);
void run_edit_alarm(unsigned char event);
void run_edit_as_wait(unsigned char event);
void run_edit_temp_max(unsigned char event);
void run_edit_stb_out(unsigned char event);
void run_edit_fm_stations(unsigned char event);
void run_edit_rc5_code(unsigned char event);
void run_alarm(unsigned char event);
//-----------------------------------------------------------------------------
void show_bigtime(void);
void show_temperature(void);
void show_freq(void);
//-----------------------------------------------------------------------------
void show_lcd_main(void);
void show_lcd_option(void);
void show_lcd_edit_time(void);
void show_lcd_edit_date(void);
void show_lcd_edit_alarm(void);
void show_lcd_edit_as_wait(void);
void show_lcd_edit_temp_max(void);
void show_lcd_edit_stb_out(void);
void show_lcd_edit_fm_stations(void);
void show_lcd_edit_rc5_code(void);
//-----------------------------------------------------------------------------
void RC5_scan(void);
void rc5_buffer_init(void);
uint8_t get_rc5_key(uint8_t code);
void DS18x20_scan(void);
void set_blink(void);
void volume_step_down(void);
void volume_step_up(void);
uint8_t check_alarm(void);
void alarm_beep(void);
uint8_t check_fault(void);
void scan_fault(void);
void stb_impuls(void);
//=============================================================================
void save_max_temperature(uint8_t temp);
uint8_t load_max_temperature(void);
void save_stb_out(uint8_t temp);
uint8_t load_stb_out(void);
void save_as_wait(uint8_t temp);
uint8_t load_as_wait(void);
void save_alarm_param(void);
void load_alarm_param(void);
void save_rc5code(void);
void load_rc5code(void);
void save_rc5code_key(uint8_t nkey, uint8_t key);
uint8_t load_rc5code_key(uint8_t nkey);
//=============================================================================
#define ALARM_ONOFF			0
#define ALARM_HOUR			1
#define ALARM_MIN			2
#define ALARM_MO			3
#define ALARM_TU			4
#define ALARM_WE			5
#define ALARM_TH			6
#define ALARM_FR			7
#define ALARM_SA			8
#define ALARM_SU			9
//=============================================================================
#define RC5_CMD			    0
#define RC5_KEY			    1
//=============================================================================
#define MODE_STANDBY		0
#define MODE_MUTE			1
#define MODE_NOKEY			2
#define MODE_MAIN			3
#define MODE_OPTIONS		4
//=============================================================================

#endif
