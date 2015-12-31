#ifndef ENGLISH_H
#define ENGLISH_H	          
//=============================================================================
#define MSG_LOAD_PARAM		"LOAD PARAMETERS"
#define MSG_DS_FOUND		"DS18x20 found: "
#define MSG_LOGO1			"Project MAXI-32"
#define MSG_LOGO2			"Version 3.0"
#define MSG_MENU_OPTIONS	"SET OPTIONS MENU"
#define MSG_CHANNEL			"CH:"
#define MSG_MHZ				"MHz"
#define MSG_SEC				"sec"
#define MSG_ERROR_CHANNEL	"---.--"
#define MSG_MUTE			"== MUTE =="
#define MSG_NO_WDAY			"__"
#define MSG_NO_RC5			"??"
#define MSG_SAVE_RC5		"SET RC5 KEY"
#define MSG_FAULT			"* FAULT *"
//=============================================================================
char *den_dw[] = {"MO","TU","WE","TH","FR","SA","SU"};
char *den_dw_full[] = {"  Monday  ","  Tuesday ","Wednesday "," Thursday ","  Friday  ","  Saturday ", "  Sunday  "};
//=============================================================================
//012345678901234567890
#define MMENU_MAX 7
char *mainmenu[] = {
 "SET VOLUME   ", // 0
 "SET GAIN     ", // 1
 "SET BASS     ", // 2
 "SET TREBLE   ", // 3
 "SET FRONT (L)", // 4
 "SET FRONT (R)", // 5
 "SET REAR (L) ", // 6
 "SET REAR (R) "  // 7
};
#define MM_VOLUME      	0
#define MM_ATTENS      	1
#define MM_BASS        	2
#define MM_TREBLE      	3
#define MM_ATTENS_FL   	4
#define MM_ATTENS_FR   	5
#define MM_ATTENS_RL   	6
#define MM_ATTENS_RR   	7
//=============================================================================
#define INPUT_MAX 		2
char *input[] = {
 "PC ", // 0
 "DVD", // 1
 "FM " // 2
};
#define IN_1     		0
#define IN_2      		1
#define IN_3      		2
//=============================================================================
#define OPTION_MAX 7
//012345678901234567890
char *optionmenu[] = {
 "SET TIME: ", 			// 0
 "SET DATE: ", 			// 1
 "SET ALARM : ", 		// 2
 "SET STANBY OUT: ", 	// 3
 "SET TEMP MAX: ", 		// 4
 "SET AS TIME ON: ", 	// 5
 "SET FM STATIONS >", 	// 6
 "SET RC5 CODE >"  		// 7
};
#define OM_SET_TIME			0
#define OM_SET_DATE			1
#define OM_SET_ALARM		2
#define OM_SET_STANBY_OUT	3
#define OM_SET_TEMP_MAX		4
#define OM_SET_AS_TIME_ON	5
#define OM_SET_FM_STATIONS	6
#define OM_SET_RC5_CODE		7
//=============================================================================
char *alarm_onoff[] = {"OFF", " ON"};
//=============================================================================
#define RC5_MENU_MAX 18
char *rc5_menu[] = {
//0123456789ABCDEF0123
//**************00:??*
 "KEY STANDBY :",  	  	
 "KEY MUTE    :",  	  	
 "KEY SELECT  :",  	  	
 "KEY SET     :",    
 "KEY UP      :",    
 "KEY DOWN    :",		
 "KEY LEFT    :", 
 "KEY RIGHT   :", 
 "KEY MENU    :", 
 "KEY LOUDNESS:", 
 "KEY 1       :", 
 "KEY 2       :", 
 "KEY 3       :", 
 "KEY 4       :", 
 "KEY 5       :", 
 "KEY 6       :", 
 "KEY 7       :", 
 "KEY 8       :", 
 "KEY 9       :" 
};
//=============================================================================
#define KEY_STANDBY 	0
#define KEY_MUTE		1
#define KEY_SELECT		2
#define KEY_SET			3
#define KEY_UP			4
#define KEY_DOWN		5
#define KEY_LEFT		6
#define KEY_RIGHT		7
#define KEY_MENU		8
#define KEY_LOUDNESS	9
#define KEY_1			10
#define KEY_2			11
#define KEY_3			12
#define KEY_4			13
#define KEY_5			14
#define KEY_6			15
#define KEY_7			16
#define KEY_8			17
#define KEY_9			18
//=============================================================================
#endif // ENGLISH_H
