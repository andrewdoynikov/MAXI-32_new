#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <inttypes.h>
#include "pins.h"
#include "lcd_lib.h"
//=============================================================================
const unsigned char TransTable[] =
{ 'A',0xA0,'B',0xA1,0xE0,'E',0xA3,0xA4,0xA5,0xA6,'K',0xA7,'M','H','O',0xA8,'P','C','T',0xA9,0xAA,'X',0xE1,0xAB,
0xAC,0xD9,0xDA,0xAE,'b',0xAF,0xB0,0xB1,'a',0xB2,0xB3,0xB4,0xE3,'e',0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,'o',0xBE,
'p','c',0xBF,'y',0xE4,'x',0xE5,0xC0,0xC1,0xE6,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7 };
//=============================================================================
const uint8_t LcdCustomChar[] PROGMEM = //define 8 custom LCD chars
{
  0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, // 0. 0/5 full progress block
  0x10, 0x10, 0x10, 0x15, 0x10, 0x10, 0x10, 0x00, // 1. 1/5 full progress block
  0x18, 0x18, 0x18, 0x1D, 0x18, 0x18, 0x18, 0x00, // 2. 2/5 full progress block
  0x1C, 0x1C, 0x1C, 0x1D, 0x1C, 0x1C, 0x1C, 0x00, // 3. 3/5 full progress block
  0x1E, 0x1E, 0x1E, 0x1F, 0x1E, 0x1E, 0x1E, 0x00, // 4. 4/5 full progress block
  0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, // 5. 5/5 full progress block
  0x0E, 0x15, 0x15, 0x17, 0x11, 0x11, 0x0E, 0x00, // 6. clock
  0x03, 0x05, 0x19, 0x11, 0x19, 0x05, 0x03, 0x00  // 5. dinamik fill
};
//=============================================================================
const uint8_t LcdBigNum[] PROGMEM = //define 8 custom LCD chars
{
  0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F,
  0x1F, 0x1F, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F,
  0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
  0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x00,
  0x1F, 0x1B, 0x13, 0x1B, 0x1B, 0x1B, 0x1F, 0x00,
  0x1F, 0x11, 0x1D, 0x11, 0x17, 0x11, 0x1F, 0x00,
  0x1F, 0x11, 0x1D, 0x11, 0x1D, 0x11, 0x1F, 0x00
};
//=============================================================================
void lcd(unsigned char p)
{ 
  PORT(LCD_PORT_CTRL) |= (1 << LCD_EN_PIN);
  if (p & 0x80) { PORT(LCD_PORT_DATA) |= (1 << LCD_D7_PIN); } else { PORT(LCD_PORT_DATA) &= (~(1 << LCD_D7_PIN)); }
  if (p & 0x40) { PORT(LCD_PORT_DATA) |= (1 << LCD_D6_PIN); } else { PORT(LCD_PORT_DATA) &= (~(1 << LCD_D6_PIN)); }
  if (p & 0x20) { PORT(LCD_PORT_DATA) |= (1 << LCD_D5_PIN); } else { PORT(LCD_PORT_DATA) &= (~(1 << LCD_D5_PIN)); }
  if (p & 0x10) { PORT(LCD_PORT_DATA) |= (1 << LCD_D4_PIN); } else { PORT(LCD_PORT_DATA) &= (~(1 << LCD_D4_PIN)); }
  PORT(LCD_PORT_CTRL) &= ~_BV(LCD_EN_PIN);
  _delay_ms(1);
  PORT(LCD_PORT_CTRL) |= (1 << LCD_EN_PIN);
  if (p & 0x08) { PORT(LCD_PORT_DATA) |= (1 << LCD_D7_PIN); } else { PORT(LCD_PORT_DATA) &= (~(1 << LCD_D7_PIN)); }
  if (p & 0x04) { PORT(LCD_PORT_DATA) |= (1 << LCD_D6_PIN); } else { PORT(LCD_PORT_DATA) &= (~(1 << LCD_D6_PIN)); }
  if (p & 0x02) { PORT(LCD_PORT_DATA) |= (1 << LCD_D5_PIN); } else { PORT(LCD_PORT_DATA) &= (~(1 << LCD_D5_PIN)); }
  if (p & 0x01) { PORT(LCD_PORT_DATA) |= (1 << LCD_D4_PIN); } else { PORT(LCD_PORT_DATA) &= (~(1 << LCD_D4_PIN)); }
  PORT(LCD_PORT_CTRL) &= (~(1 << LCD_EN_PIN));
  _delay_ms(1);
  PORT(LCD_PORT_CTRL) |= (1 << LCD_EN_PIN);
}                
//=============================================================================
void LCD_com(unsigned char p)
{
  PORT(LCD_PORT_CTRL) &= ~_BV(LCD_RS_PIN);
  PORT(LCD_PORT_CTRL) |= _BV(LCD_EN_PIN);
  lcd(p);
}
//=============================================================================
void LCD_dat(unsigned char p)
{
  PORT(LCD_PORT_CTRL) |= (1 << LCD_RS_PIN) | (1 << LCD_EN_PIN);
  if ((p > 0x80) && (p != 0xED)) {
    p = TransTable[(p & 0x7F) - 0x40];
  }
  lcd(p);
}
//=============================================================================
void LCD_init(void)
{
  // настройка портов
  LIGHT_init();
  LIGHT_on(); 
  DDR(LCD_PORT_DATA) |= ((1 << LCD_D4_PIN) | (1 << LCD_D5_PIN) | (1 << LCD_D6_PIN) | (1 << LCD_D7_PIN));
  PORT(LCD_PORT_DATA) &= (~(1 << LCD_D4_PIN) | (1 << LCD_D5_PIN) | (1 << LCD_D6_PIN) | (1 << LCD_D7_PIN));
  DDR(LCD_PORT_CTRL) |= ((1 << LCD_RS_PIN) | (1 << LCD_EN_PIN) | (1 << LCD_RW_PIN)); 
  PORT(LCD_PORT_CTRL) &= (~(1 << LCD_RS_PIN) | (1 << LCD_EN_PIN) | (1 << LCD_RW_PIN));
  _delay_ms(200);
  LCD_com(0x33);
  _delay_ms(20);
  LCD_com(0x32);
  _delay_ms(1);
  LCD_com(0x28);
  _delay_ms(1);
  LCD_com(0x08);
  _delay_ms(1);
  LCD_com(0x01);
  _delay_ms(50);
  LCD_com(0x06);
  LCD_com(0x0C);
  LCD_load_bignum();
}
//=============================================================================
void LCD_puts(char *data)
{
  while(*data) LCD_dat(*(data++));
}
//=============================================================================
void LCD_SetXY(unsigned char x, unsigned char y)
{
  uint8_t i = 0x80;
  switch (y)
  {
    case 0: i = 0x80; break;
    case 1: i = 0xC0; break;
    case 2: i = 0x94; break;
    case 3: i = 0xD4; break;
  }
  LCD_com(i + x);
}
//=============================================================================
void LCD_clear(void)
{
  LCD_com(1 << LCD_CLR);
  _delay_ms(200);
}
//=============================================================================
void LCD_definechar(const unsigned char *pc,uint8_t char_code)
{
  uint8_t a, pcc;
  uint16_t i;
  a = (char_code << 3) | 0x40;
  for (i = 0; i < 8; i++) {
    pcc = pgm_read_byte(&pc[i]);
	LCD_com(a++); LCD_dat(pcc);
  }
}
//=============================================================================
void LCD_progress_bar(unsigned char progress, unsigned char maxprogress, unsigned char length)
{
uint8_t i;
uint16_t pixelprogress;
uint8_t c;
pixelprogress = ((progress * (length * PROGRESSPIXELS_PER_CHAR)) / maxprogress);
for(i=0; i<length; i++)
{
  if( ((i * (uint16_t)PROGRESSPIXELS_PER_CHAR) + 5) > pixelprogress )
  {
    if( ((i*(uint16_t)PROGRESSPIXELS_PER_CHAR)) > pixelprogress )
    {
      c = 0;
    } else {
      c = pixelprogress % PROGRESSPIXELS_PER_CHAR;
    }
  } else {
    c = 5;
  }
  LCD_dat(c);
}

}
//=============================================================================
void LCD_space(unsigned char n_sp)
{
unsigned char i;
for (i = 1; i <= n_sp; i++) {
  LCD_dat(' ');
}
}
//=============================================================================
void LCD_load_defchar(void)
{
unsigned char ch = 0, chn = 0;
while(ch < 64)
{
  LCD_definechar((LcdCustomChar + ch),chn++);
  _delay_ms(1);
  ch = ch + 8;
}
}
//=============================================================================
void LCD_load_bignum(void)
{
unsigned char ch = 0, chn = 0;
while(ch < 64)
{
  LCD_definechar((LcdBigNum + ch),chn++);
  _delay_ms(1);
  ch = ch + 8;
}
}
//=============================================================================
