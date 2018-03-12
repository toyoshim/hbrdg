CC	= avr-gcc
OBJCOPY	= avr-objcopy
SIZE	= avr-size
CFLAGS	= -Os -Wall -mmcu=atmega88 -mtiny-stack
TARGET	= hbrdg
OBJS	= hbrdg.o

all: $(TARGET).hex

program: $(TARGET).hex
	hidspx $<

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

