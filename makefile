MCU=atmega328p
F_CPU=8000000
CC=avr-gcc
OBJCOPY=avr-objcopy

SRCDIR      := src
INCDIR      := include
OBJDIR      := obj
TARGETDIR   := bin

SRCFILES := $(wildcard $(SRCDIR)/*.c)
OBJFILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCFILES))

CFLAGS= -std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU}
FUSES = -U lfuse:w:0xe2:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m
AVRDUDE = avrdude -c avrisp -P /dev/ttyACM0 -b 19200 -p $(MCU)

run: all flash

all:	$(TARGETDIR)/main.hex

$(TARGETDIR)/main.elf:	$(OBJFILES)
												mkdir -p $(TARGETDIR)
												$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o:	$(SRCDIR)/%.c
								mkdir -p $(OBJDIR)
								$(CC) $(CFLAGS) -I $(INCDIR) -c $< -o $@

# file targets
$(TARGETDIR)/main.hex:	$(TARGETDIR)/main.elf
												rm -f $(TARGETDIR)/main.hex
												avr-objcopy -j .text -j .data -O ihex $(TARGETDIR)/main.elf $(TARGETDIR)/main.hex

flash:
				$(AVRDUDE) -U flash:w:$(TARGETDIR)/main.hex:i

fuse:
				$(AVRDUDE) $(FUSES)

clean:
				rm -rf $(OBJDIR)
				rm -rf $(TARGETDIR)
				rm -rf $(INCDIR)/*.h.gch
