CC = xtensa-lx106-elf-gcc
CFLAGS = -Os -Iinclude -mlongcalls -g
CUSTOMLIBDIR =
LDFLAGS = -Teagle.app.v6.ld -L$(CUSTOMLIBDIR)
ESPBAUD = 300000

# for SDK 0.9.5
LDLIBS = -nostdlib -Wl,--start-group -lmain -lupgrade -lnet80211 -lwpa -llwip -lpp -lphy -lat -Wl,--end-group -lcirom -lgcc
# for SDK 0.9.4
#LDLIBS = -nostdlib -Wl,--start-group -lmain -lupgrade -lnet80211 -lwpa -llwip -lpp -lphy -Wl,--end-group -lcirom -lgcc

test1-0x00000.bin: test1
	esptool.py elf2image $^

test1: driver/uart.o user/user_main.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

flash: test1-0x00000.bin
	esptool.py --baud $(ESPBAUD) write_flash 0 test1-0x00000.bin 0x40000 test1-0x40000.bin

connect:
	picocom -b 9600 --omap crcrlf /dev/ttyUSB0

clean:
	rm test1-0x00000.bin test1-0x40000.bin driver/uart.o user/user_main.o
