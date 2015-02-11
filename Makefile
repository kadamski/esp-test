SDKVER       ?= "0.9.4"
APP          ?= hello1
CUSTOMLIBDIR ?=

CC = xtensa-lx106-elf-gcc
CFLAGS = -Os -Iinclude -mlongcalls -g -DICACHE_FLASH #-DUSE_OPTIMIZE_PRINTF
LDFLAGS = -Teagle.app.v6.ld
ESPBAUD = 300000
SDKLIBS = main upgrade net80211 wpa lwip pp phy

ifdef $(CUSTOMLIBDIR)
	LDFLAGS += -L$(CUSTOMLIBDIR)
endif
ifeq ($(SDKVER), "0.9.5")
	SDKLIBS += at
endif

LDLIBS  = -nostdlib -Wl,--start-group
LDLIBS += $(foreach lib, $(SDKLIBS), -l$(lib))
LDLIBS += -Wl,--end-group -lcirom -lgcc

$(APP).elf-0x00000.bin: $(APP).elf
	esptool.py elf2image $^

hello1.elf: driver/uart.o hello1/user_main.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

exceptions.elf: driver/uart.o exceptions/user_main.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

timer1.elf: driver/uart.o timer1/user_main.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

task1.elf: driver/uart.o task1/user_main.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

flash: $(APP).elf-0x00000.bin
	esptool.py --baud $(ESPBAUD) write_flash 0 $(APP).elf-0x00000.bin 0x40000 $(APP).elf-0x40000.bin

connect:
	picocom -b 9600 --omap crcrlf /dev/ttyUSB0

clean:
	rm -f *.elf *.elf-0x[04]0000.bin driver/uart.o hello1/user_main.o exceptions/user_main.o timer1/user_main.o
	rm -f task1/user_main.o
