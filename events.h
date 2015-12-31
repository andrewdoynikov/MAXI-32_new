#ifndef EVENT_LIST_H
#define EVENT_LIST_H
//=============================================================================
// ќпределение кодов событий
#define EVENT_NULL         		254
// сообщение - —екундный таймер
#define EVENT_TIMER_SECOND		253
// сообщени€ от пульта RC5
#define EVENT_RC5_STANDBY      	13
#define EVENT_RC5_MUTE       	14
#define EVENT_RC5_KEY0       	1
#define EVENT_RC5_KEY1      	2
#define EVENT_RC5_KEY2       	3
#define EVENT_RC5_KEY3       	4
#define EVENT_RC5_KEY4       	5
#define EVENT_RC5_KEY5       	6
#define EVENT_RC5_KEY6       	7
#define EVENT_RC5_KEY7       	8
#define EVENT_RC5_KEY8       	9
#define EVENT_RC5_KEY9       	10
#define EVENT_RC5_BANK       	11
#define EVENT_RC5_PP     	  	15
#define EVENT_RC5_MENU       	49
#define EVENT_RC5_PPP       	35
#define EVENT_RC5_UP       	    33
#define EVENT_RC5_DOWN       	34
#define EVENT_RC5_LEFT       	18
#define EVENT_RC5_RIGHT      	17
#define EVENT_RC5_SET       	60
#define EVENT_RC5_LEFT1      	22
#define EVENT_RC5_RIGHT1     	23
#define EVENT_RC5_TV  	     	32
#define EVENT_RC5_AV  	     	57
#define EVENT_RC5_SLEEP	     	39
#define EVENT_RC5_TIME 	     	43
#define EVENT_RC5_STEREO     	37
#define EVENT_RC5_I_II 	     	36
#define EVENT_RC5_FORVARD     	31
#define EVENT_RC5_LIGHT	     	47
#define EVENT_RC5_S  	     	56
#define EVENT_RC5_P  	     	55
#define EVENT_RC5_F  	     	51
#define EVENT_RC5_I  	     	53
#define EVENT_RC5_FP  	     	45
#define EVENT_RC5_FM  	     	44
#define EVENT_RC5_M  	     	42
#define EVENT_RC5_LINE 	     	61
#define EVENT_RC5_CODE_MAX     	99
//=============================================================================
// сообщени€ от кнопок
#define EVENT_KEY_STANDBY  		100
#define EVENT_KEY_MUTE     	    101
#define EVENT_KEY_SELECT       	102
#define EVENT_KEY_SET       	103
#define EVENT_KEY_UP 	      	104
#define EVENT_KEY_DOWN       	105
#define EVENT_KEY_LEFT      	106
#define EVENT_KEY_RIGHT     	107
#define EVENT_KEY_MENU       	108
#define EVENT_KEY_LOUDNESS     	109
#define EVENT_KEY_1     	    110
#define EVENT_KEY_2     		111
#define EVENT_KEY_3     		112
#define EVENT_KEY_4     		113
#define EVENT_KEY_5     		114
#define EVENT_KEY_6     		115
#define EVENT_KEY_7     		116
#define EVENT_KEY_8     		117
#define EVENT_KEY_9     		118
//=============================================================================
// сообщени€ дл€ вывода на дисплей
#define EVENT_SHOW_LOGO			150
#define EVENT_SHOW_DS_CNT		151
#define EVENT_SHOW_AS_WAIT		152
#define EVENT_SHOW_VOL_DOWN		153
#define EVENT_SHOW_VOL_UP		154
#define EVENT_SHOW_STANDBY		155
#define EVENT_NOKEY				156
// системные сообщени€
#define EVENT_KEY_PRESS     	160
#define EVENT_START				161
#define EVENT_STOP				162
#define EVENT_RETURN_VOLUME		163
#define EVENT_RETURN_MODE		164
#define EVENT_SAVE_PARAM		165
#define EVENT_SAVE_INPUT		166
#define EVENT_SAVE_LOUDNESS		167
#define EVENT_SAVE_CHANNEL		168
#define EVENT_SET_STB_OUT_ON	169
#define EVENT_SET_STB_OUT_OFF	170
#define EVENT_SET_AS_WAIT_ON	171
#define EVENT_SET_AS_WAIT_OFF	172
#define EVENT_LOAD_PARAM		173
#define EVENT_START_TASK		174
#define EVENT_STOP_FM_EDIT		175
#define EVENT_CLEAR				176
#define EVENT_START_IMPULS		177
#define EVENT_STOP_IMPULS		178
#define EVENT_CHECK_FAULT		179
//=============================================================================
#endif // EVENT_LIST_H
