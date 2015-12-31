#ifndef PINS_H
#define PINS_H
//=============================================================================
#include <avr/io.h>
#include <util/delay.h>
//=============================================================================
#define CONCAT(x,y)			x ## y

#define DDR(x)				CONCAT(DDR,x)
#define PORT(x)				CONCAT(PORT,x)
#define PIN(x)				CONCAT(PIN,x)
//=============================================================================
#define RDA5807M			0
#define TEA5767				1

#define TUNER				1

#define FM_STATIONS			30
#define FM_INPUT			2
//=============================================================================
// eeprom addreses
//=============================================================================
#define ALARM_DEBUG      			0
#define ALARM_ONOFF_ADDR      		0x12
#define ALARM_HOUR_ADDR      		0x13
#define ALARM_MIN_ADDR      		0x14
#define ALARM_SEC_ADDR      		0x15
#define ALARM_WDAY_ADDR      		0x16

#define FM_STATION_ADDR  			0x10
#define FM_VOLUME_ADDR      		0x11
#define FM_CHANEL_ALL_ADDR  		0x20
#define FM_VOLUME_EDIT_ADDR 		0x17

#define TDA_PARAM_ADDR_IN1 			0x50
#define TDA_PARAM_ADDR_IN2 			0x60
#define TDA_PARAM_ADDR_IN3 			0x70
#define TDA_INPUT_ADDR 				0x80
#define TDA_LOUDNESS_ADDR 			0x81

#define MAX_TEMPERATURE_ADDR		0x82
#define STANDBY_OUT_ADDR			0x83
#define AS_WAIT_ADDR				0x84
#define STANDBY_OUT_DEF				1

#define RC5_CODE_ADDR 				0x90
//=============================================================================
#define ONE_WIRE			B
#define ONE_WIRE_LINE		(1 << 4)
#define MAX_TEMP			85
#define MIN_TEMP			30
#define DEF_TEMP			45
#define ONE_WIRE_TIME		1300
#define ONE_WIRE_GISTER		2
#define TEMP_BLINK_MAX		1

#define MAX_AS_WAIT			10
#define MIN_AS_WAIT			0
#define DEF_AS_WAIT			3

#define BEEPER				D
#define BEEPER_LINE			(1 << 7)
#define BEEP_init()			DDR(BEEPER) |= BEEPER_LINE; PORT(BEEPER) |= BEEPER_LINE;
#define BEEP_on()			PORT(BEEPER) &= ~BEEPER_LINE;
#define BEEP_off()			PORT(BEEPER) |= BEEPER_LINE;
#define BEEP()				BEEP_on(); _delay_ms(10); BEEP_off();

#define BUT_STANDBY			B
#define BUT_STANDBY_PIN		5
#define BUT_STANDBY_LINE	(1 << BUT_STANDBY_PIN)
#define BUT_SET				B
#define BUT_SET_PIN			6
#define BUT_SET_LINE		(1 << BUT_SET_PIN)
#define BUT_MENU			B
#define BUT_MENU_PIN		7
#define BUT_MENU_LINE		(1 << BUT_MENU_PIN)
#define BUT_SELECT			C
#define BUT_SELECT_PIN		3
#define BUT_SELECT_LINE		(1 << BUT_SELECT_PIN)
#define BUT_UP				D
#define BUT_UP_PIN			4
#define BUT_UP_LINE			(1 << BUT_UP_PIN)
#define BUT_DOWN			D
#define BUT_DOWN_PIN		5
#define BUT_DOWN_LINE		(1 << BUT_DOWN_PIN)
#define BUT_LEFT			C
#define BUT_LEFT_PIN		2
#define BUT_LEFT_LINE		(1 << BUT_LEFT_PIN)
#define BUT_RIGHT			D
#define BUT_RIGHT_PIN		6
#define BUT_RIGHT_LINE		(1 << BUT_RIGHT_PIN)
#define KBD_NOKEY			0xFF

#define I2C_SCL				C
#define I2C_SCL_PIN			0
#define I2C_SCL_LINE		(1 << I2C_SCL_PIN)
#define I2C_SDA				C
#define I2C_SDA_PIN			1
#define I2C_SDA_LINE		(1 << I2C_SDA_PIN)

#define LIGHT				C
#define LIGHT_PIN			4
#define LIGHT_LINE			(1 << LIGHT_PIN)
#define LIGHT_init()		DDR(LIGHT) |= (1 << LIGHT_PIN);
#define LIGHT_on()			PORT(LIGHT) |= (1 << LIGHT_PIN);
#define LIGHT_off()			PORT(LIGHT) &= (~(1 << LIGHT_PIN));

#define EXT_KBD_CHANNEL		7

#define LCD_PORT_DATA       A
#define LCD_PORT_CTRL       A
#define LCD_RS_PIN 			5
#define LCD_EN_PIN		    4
#define LCD_RW_PIN		    7
#define LCD_D4_PIN		    0               
#define LCD_D5_PIN    		1               
#define LCD_D6_PIN    		2               
#define LCD_D7_PIN    		3               

#define LED_FAULT_IN			C
#define LED_FAULT_IN_PIN		7
#define LED_FAULT_IN_LINE		(1 << 7)
#define LED_FAULT_IMPULS0		C
#define LED_FAULT_IMPULS0_LINE	(1 << 6)
#define LED_FAULT_IMPULS0_on()	PORT(LED_FAULT_IMPULS0) |= LED_FAULT_IMPULS0_LINE;
#define LED_FAULT_IMPULS0_off()	PORT(LED_FAULT_IMPULS0) &= ~LED_FAULT_IMPULS0_LINE;
#define LED_FAULT_IMPULS1		C
#define LED_FAULT_IMPULS1_LINE	(1 << 5)
#define LED_FAULT_IMPULS1_on()	PORT(LED_FAULT_IMPULS1) &= ~LED_FAULT_IMPULS1_LINE;
#define LED_FAULT_IMPULS1_off()	PORT(LED_FAULT_IMPULS1) |= LED_FAULT_IMPULS1_LINE;

#define LED_MUTE_DUBLE			A
#define LED_MUTE_DUBLE_LINE		(1 << 7)
#define LED_MUTE_DUBLE_1		PORT(LED_MUTE_DUBLE) |= LED_MUTE_DUBLE_LINE;
#define LED_MUTE_DUBLE_0		PORT(LED_MUTE_DUBLE) &= ~LED_MUTE_DUBLE_LINE;

#define LED_STB_IMPULS			A
#define LED_STB_IMPULS_LINE		(1 << 6)
#define LED_STB_IMPULS_1		PORT(LED_STB_IMPULS) |= LED_STB_IMPULS_LINE;
#define LED_STB_IMPULS_0		PORT(LED_STB_IMPULS) &= ~LED_STB_IMPULS_LINE;

#define LED_STANDBY			B
#define LED_STANDBY_LINE	(1 << 0)
#define LED_STANDBY_on()	PORT(LED_STANDBY) &= ~LED_STANDBY_LINE;
#define LED_STANDBY_off()	PORT(LED_STANDBY) |= LED_STANDBY_LINE;
#define LED_STANDBY_init()	DDR(LED_STANDBY) |= LED_STANDBY_LINE; PORT(LED_STANDBY) &= ~LED_STANDBY_LINE;
#define LED_STANDBY_flip()	LED_STANDBY_on(); _delay_ms(20); LED_STANDBY_off();

#define LED_AC_WAIT			B
#define LED_AC_WAIT_LINE	(1 << 3)
#define LED_AC_WAIT_on()	PORT(LED_AC_WAIT) |= LED_AC_WAIT_LINE;
#define LED_AC_WAIT_off()	PORT(LED_AC_WAIT) &= ~LED_AC_WAIT_LINE;
#define LED_AC_WAIT_init()	DDR(LED_AC_WAIT) |= LED_AC_WAIT_LINE; PORT(LED_AC_WAIT) &= ~LED_AC_WAIT_LINE;

#define LED_RC5_CMD			B
#define LED_RC5_CMD_LINE	(1 << 1)
#define LED_RC5_CMD_on()	PORT(LED_RC5_CMD) &= ~LED_RC5_CMD_LINE;
#define LED_RC5_CMD_off()	PORT(LED_RC5_CMD) |= LED_RC5_CMD_LINE;
#define LED_RC5_CMD_init()	DDR(LED_RC5_CMD) |= LED_RC5_CMD_LINE; PORT(LED_RC5_CMD) |= LED_RC5_CMD_LINE;
#define LED_RC5_CMD_flip()	LED_RC5_CMD_on(); _delay_ms(30); LED_RC5_CMD_off();

#define LED_STB_OUT			B
#define LED_STB_OUT_LINE	(1 << 2)
#define LED_STB_OUT_1		PORT(LED_STB_OUT) |= LED_STB_OUT_LINE;
#define LED_STB_OUT_0		PORT(LED_STB_OUT) &= ~LED_STB_OUT_LINE;
#define LED_STB_OUT_init()	DDR(LED_STB_OUT) |= LED_STB_OUT_LINE;

#define LED_VENTIL			D
#define LED_VENTIL_LINE		(1 << 1)
#define LED_VENTIL_off()	PORT(LED_VENTIL) &= ~LED_VENTIL_LINE;
#define LED_VENTIL_on()		PORT(LED_VENTIL) |= LED_VENTIL_LINE;
#define LED_VENTIL_init()	DDR(LED_VENTIL) |= LED_VENTIL_LINE; PORT(LED_VENTIL) &= ~LED_VENTIL_LINE;

#define ENCODER				D
#define ENCODER_PIN_A		0
#define ENCODER_PIN_B		3
#define ENCODER_LINE_A		(1 << ENCODER_PIN_A)
#define ENCODER_LINE_B		(1 << ENCODER_PIN_B)

#define ALARM_BEEP	        60 * 2
#define ALARM_CHAR	        '*'
#define LOUDNESS_CHAR	    'L'
#define BLINK_TIME		    600  // ms
#define DS18x20_SCAN_TIME   1000 // ms
#define KBD_SCAN_TIME       5    // ms
#define ENC_SCAN_TIME       1    // ms
#define RC5_SCAN_TIME       5    // ms
#define FM_VOLUME_EDIT_DEF  24
#define NOKEY_TIME			30   // 30
#define TIME_YEAR_MAX		99 
#define TIME_RETURN_VOLUME	10000
#define TIME_SAVE_PARAM	    2000
#define TIME_FAULT	    	500


#endif /* PINS_H */
