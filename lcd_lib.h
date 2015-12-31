#ifndef LCD_LIB_H
#define LCD_LIB_H	          
//=============================================================================
#define LCD_TIME  10              // wait for LCD
//=============================================================================
#define LCD_CLR                 0	// clear display
#define LCD_HOME                1	// return to home position
#define LCD_ENTRY_MODE          2	// set entry mode
#define LCD_ENTRY_INC           1	// increment
#define LCD_ENTRY_SHIFT         0	// shift
#define LCD_ON_CTRL             3	// turn lcd/cursor on
#define LCD_ON_DISPLAY          2	// turn display on
#define LCD_ON_CURSOR           1	// turn cursor on
#define LCD_ON_BLINK            0	// blinking cursor
#define LCD_MOVE                4	// move cursor/display
#define LCD_MOVE_DISP           3	// move display (0-> move cursor)
#define LCD_MOVE_RIGHT          2	// move right (0-> left)
#define LCD_FUNCTION            5	// function set
#define LCD_CGRAM               6	// set CG RAM address
#define LCD_DDRAM               7	// set DD RAM address
#define LCD_LINES			    2	// visible lines
#define LCD_LINE_LENGTH		    16	// line length (in characters)
#define PROGRESSPIXELS_PER_CHAR	6
//=============================================================================
// Functions prototype
//=============================================================================
void lcd(unsigned char p);
void LCD_init(void);
void LCD_com(unsigned char p);
void LCD_SetXY(unsigned char x, unsigned char y);
void LCD_dat(unsigned char p);
void LCD_puts(char *data);
void LCD_clear(void);
void LCD_space(unsigned char p);
void LCD_definechar(const unsigned char *pc, unsigned char char_code);
void LCD_progress_bar(unsigned char progress, unsigned char maxprogress, unsigned char length);
void LCD_load_defchar(void);
void LCD_load_bignum(void);
//=============================================================================
#endif
