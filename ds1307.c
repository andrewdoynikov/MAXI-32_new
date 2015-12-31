#include <avr/io.h>
#include <util/delay.h>
#include "pins.h"
#include "i2c.h"
#include "lcd_lib.h"
#include "ds1307.h"
//=============================================================================
void RTC_write_reg(unsigned char addr,unsigned char data)
{
  I2C_start();
  I2C_write(DS1307_ADDR_WRITE);
  I2C_write(addr);
  I2C_write(data);
  I2C_stop();
}
//=============================================================================
unsigned char RTC_read_reg(unsigned char addr)
{
  unsigned char temp = 0; 
  I2C_start();
  I2C_write(DS1307_ADDR_WRITE);
  I2C_write(addr);
  I2C_stop();
  I2C_start();
  I2C_write(DS1307_ADDR_READ);
  temp = I2C_read(I2C_NOACK);
  I2C_stop();
  return temp;
}
//=============================================================================
void RTC_init(void)
{
  unsigned temp = 0; 
  temp = RTC_read_reg(0x00);
  if ((temp & 0x80) != 0) {
    RTC_write_reg(0x00, temp & ~0x80);
  }
  RTC_write_reg(DS1307_CONTROL, 0x90);
}
//=============================================================================
void RTC_set_time(unsigned char hour, unsigned char min, unsigned char sec)
{
  I2C_start();
  I2C_write(DS1307_ADDR_WRITE);
  I2C_write(0);
  I2C_write(bin2bcd(sec));
  I2C_write(bin2bcd(min));
  I2C_write(bin2bcd(hour));
  I2C_stop();
}
//=============================================================================
void RTC_get_time(unsigned char *hour,unsigned char *min,unsigned char *sec)
{
  I2C_start();
  I2C_write(DS1307_ADDR_WRITE);
  I2C_write(0);
  I2C_stop();
  I2C_start();
  I2C_write(DS1307_ADDR_READ);
  *sec = bcd2bin(I2C_read(I2C_ACK));
  *min = bcd2bin(I2C_read(I2C_ACK));
  *hour = bcd2bin(I2C_read(I2C_NOACK));
  I2C_stop();
}
//=============================================================================
void RTC_set_date(unsigned char date,unsigned char month,unsigned char year)
{
  I2C_start();
  I2C_write(DS1307_ADDR_WRITE);
  I2C_write(DS1307_DATE);
  I2C_write(bin2bcd(date));
  I2C_write(bin2bcd(month));
  I2C_write(bin2bcd(year));
  I2C_stop();
}
//=============================================================================
void RTC_get_date(unsigned char *date,unsigned char *month, unsigned char *year)
{
  I2C_start();
  I2C_write(DS1307_ADDR_WRITE);
  I2C_write(DS1307_DATE);
  I2C_stop();
  I2C_start();
  I2C_write(DS1307_ADDR_READ);
  *date = bcd2bin(I2C_read(I2C_ACK));
  *month = bcd2bin(I2C_read(I2C_ACK));
  *year = bcd2bin(I2C_read(I2C_NOACK));
  I2C_stop();
}
//=============================================================================
unsigned char bcd2bin(unsigned char bcd)
{
  unsigned char temp = bcd & 0x0F; 
  while (bcd >= 0x10) 
  { 
    temp += 10; 
    bcd -= 0x10; 
  } 
  return temp; 
}
//=============================================================================
unsigned char bin2bcd(unsigned char bin)
{
  unsigned temp = 0; 
  while(bin > 9) 
  { 
    temp += 0x10; 
    bin -= 10; 
  } 
  return temp + bin; 
}
//=============================================================================
void RTC_print_time(void)
{
  unsigned char h,m,s;
  RTC_get_time(&h,&m,&s);
  print_dec(h,2,'0'); LCD_dat(':'); print_dec(m,2,'0'); LCD_dat(':'); print_dec(s,2,'0');
}
//=============================================================================
void RTC_print_date(void)
{
  unsigned char d,m,y;
  RTC_get_date(&d,&m,&y);
  print_dec(d,2,'0'); LCD_dat('/'); print_dec(m,2,'0'); LCD_dat('/'); print_dec(20,2,'0'); print_dec(y,2,'0');
}
//=============================================================================
uint8_t RTC_dayofweek(void)
{
  uint8_t d, m, y;
  RTC_get_date(&d,&m,&y);
  if (m < 3) {
    m += 12;
    y--;
  }
  return ( (13 * m + 3) / 5 + d + y + y / 4 - y / 100 + y / 400) % 7;
}
//=============================================================================
void print_dec(unsigned int x, unsigned char n, unsigned char fillch)
{
  unsigned char i;
  unsigned char s[10];
  for (i = 0; i < n; i++) {
    s[n - i - 1] = '0' + (x % 10);
    x /= 10;
  }
  for (i=0; i<(n - 1); i++) {
    if (s[i] == '0') s[i] = fillch; else break;
  }
  for (i=0; i<n; i++) LCD_dat(s[i]);
}
//=============================================================================
void print_hex(unsigned char x)
{
  unsigned char hi, lo;
  hi = x & 0xF0;
  hi = hi >> 4;
  hi = hi + '0';
  if (hi >'9') hi = hi + 7;
  lo = (x & 0x0F) + '0';
  if (lo > '9') lo = lo + 7;
  LCD_dat(hi); LCD_dat(lo);
}
//=============================================================================
