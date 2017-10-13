MCU=$(shell if [ `whereami` = work ]; then echo atmega168; else echo atmega328p; fi)

BUF_SZ=$(shell if [ `whereami` = work ]; then echo 800; else echo 1800; fi)
  # ATmega168 and ATmega328p are pin-compatible and only differ in the size of internal memory
  # (1KB SRAM and 2KB SRAM correspondingly)
# Quote from http://www.rowetel.com/?p=1776
#   A large FIFO buffer is required on the Arduino program (1800 bytes) to get good
#   quality audio with no gaps through RS232 serial. This is almost all of the
#   micro-controllers 2k RAM.

avrplay:
	avr-gcc -mmcu=$(MCU) -g -Os -c $@.c -DBUF_SZ=$(BUF_SZ)
	avr-gcc -mmcu=$(MCU) -g -o $@.elf $@.o
	avr-objcopy -O ihex $@.elf $@.hex
	avrdude -c usbasp -p $(MCU) -U flash:w:$@.hex -qq

play: play.c
	clang -o play play.c -lm
