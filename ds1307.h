#ifndef RTC_H
#define RTC_H
//=============================================================================
// RTC defines
#define DS1307_ADDR_READ  0xD1 
#define DS1307_ADDR_WRITE 0xD0 
#define DS1307_CONTROL    0x07
#define DS1307_DATE       0x04
#define CH                0x07
#define DS1307_PORT_OUT   PINA
#define DS1307_PIN_OUT    PA6
//=============================================================================
// RTC function
//=============================================================================
void RTC_init(void);
void RTC_write_reg(unsigned char addr,unsigned char data);
unsigned char RTC_read_reg(unsigned char addr);
void RTC_get_time(unsigned char *hour,unsigned char *min,unsigned char *sec);
void RTC_set_time(unsigned char hour, unsigned char min, unsigned char sec);
void RTC_get_date(unsigned char *date,unsigned char *month,unsigned char *year);
void RTC_set_date(unsigned char date,unsigned char month,unsigned char year);
unsigned char bcd2bin(unsigned char bcd);
unsigned char bin2bcd(unsigned char bin);
void RTC_print_time(void);
void RTC_print_date(void);
void print_dec(unsigned int x, unsigned char n, unsigned char fillch);
void print_hex(unsigned char x);
uint8_t RTC_dayofweek(void);
//=============================================================================
#endif /* RTC_H */
