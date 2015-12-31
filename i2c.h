#ifndef I2C_H
#define I2C_H	          
//=============================================================================
#include "pins.h"
//=============================================================================
// define macros
//=============================================================================
#define I2C_SDA_PORT	  PORT(I2C_SDA)	
#define SDA_PIN 	      I2C_SDA_PIN	    
#define I2C_SCL_PORT	  PORT(I2C_SCL)	    
#define SCL_PIN 	      I2C_SCL_PIN	    
// ============================================================================
#define I2C_SDA_OUT_REG   PORT(I2C_SDA)
#define I2C_SCL_OUT_REG   PORT(I2C_SCL)
#define I2C_SDA_DDR_REG   DDR(I2C_SDA)
#define I2C_SCL_DDR_REG   DDR(I2C_SCL)
#define I2C_SDA_PIN_REG   PIN(I2C_SDA)
#define I2C_SCL_PIN_REG   PIN(I2C_SCL)
#define SCL_1()   I2C_SCL_DDR_REG &= ~(1<<SCL_PIN)
#define SCL_0()   I2C_SCL_DDR_REG |= (1<<SCL_PIN)
#define SDA_1()   I2C_SDA_DDR_REG &= ~(1<<SDA_PIN)
#define SDA_0()   I2C_SDA_DDR_REG |= (1<<SDA_PIN)
#define RELEASE_I2C_BUS() { SCL_1(); SDA_1(); }
// ============================================================================
#define I2C_DELAY	   ( (I2C_DELAY_TIME * (F_CPU / 60000)) / 100 )  
#define I2C_TIMEOUT	   ( (I2C_TIMEOUT_TIME * (F_CPU / 60000)) / 100 ) 
#define I2C_DELAY_TIME	  				 5	    
#define I2C_TIMEOUT_TIME  				 1000	
#define I2C_READ	 	  				 1
#define I2C_WRITE	      				 0
#define I2C_QUIT	      				 0
#define I2C_CONTINUE      				 1
#define I2C_NO_ERROR      				 0
#define I2C_ERROR_DEVICE_BUSY			 1
#define I2C_ERROR_DEVICE_NOT_RESPONDING  2
#define I2C_ACK                          1
#define I2C_NOACK                        0
// Functions prototype
void delay(unsigned long us);
void I2C_init(void);
void I2C_start(void);
unsigned char I2C_write(unsigned char data);
void I2C_stop (void);
unsigned char I2C_read(unsigned char ack);
#endif
