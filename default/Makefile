###############################################################################
# Makefile for the project maxi32new
###############################################################################

## General Flags
PROJECT = maxi32new
MCU = atmega32
TARGET = maxi32new.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=maxi32new.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = main.o lcd_lib.o i2c.o ds1307.o kbd.o enc.o rtos.o interface.o ds18x20.o rc5.o tda7313.o tuner.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) maxi32new.hex maxi32new.eep maxi32new.lss size

## Compile
main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lcd_lib.o: ../lcd_lib.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

i2c.o: ../i2c.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ds1307.o: ../ds1307.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

kbd.o: ../kbd.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

enc.o: ../enc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtos.o: ../rtos.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

interface.o: ../interface.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ds18x20.o: ../ds18x20.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rc5.o: ../rc5.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

tda7313.o: ../tda7313.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

tuner.o: ../tuner.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) maxi32new.elf dep/* maxi32new.hex maxi32new.eep maxi32new.lss maxi32new.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

