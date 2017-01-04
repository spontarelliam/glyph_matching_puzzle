ARDUINO_DIR = /usr/share/arduino
ARDMK_DIR = /usr/share/arduino
AVR_TOOLS_DIR = /usr
ARDUINO_CORE_PATH = /usr/share/arduino/hardware/archlinux-arduino/avr/cores/arduino
BOARDS_TXT = /usr/share/arduino/hardware/archlinux-arduino/avr/boards.txt
ARDUINO_VAR_PATH = /usr/share/arduino/hardware/archlinux-arduino/avr/variants
BOOTLOADER_PARENT = /usr/share/arduino/hardware/archlinux-arduino/avr/bootloaders
ARDUINO_PLATFORM_LIB_PATH += /usr/share/arduino/hardware/archlinux-arduino/avr/libraries
AVRDUDE      = /usr/bin/avrdude
AVRDUDE_CONF = /etc/avrdude.conf
USER_LIB_PATH = /home/smed/Arduino/libraries

BOARD_TAG    = uno

ARDUINO_LIBS += MFRC522 \
		SPI \
		Adafruit_NeoPixel

include /usr/share/arduino/Arduino.mk
