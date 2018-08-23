: Installed Arduino IDE's toolchain's path.
@echo off
set PATH=%PATH%;C:\Program Files (x86)\Arduino\hardware\tools\avr\bin

@echo on
avr-gcc -c hbrdg.c -Os -Wall -mmcu=atmega88 -mtiny-stack
avr-gcc -o hbrdg.elf hbrdg.o
avr-objcopy -O ihex hbrdg.elf hbrdg.hex