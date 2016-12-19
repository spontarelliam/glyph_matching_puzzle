BOARD_TAG    = uno
USER_LIB_PATH += /home/adam/Arduino/libraries
ARDUINO_LIBS += MFRC522 \
		Adafruit_NeoPixel \
		SPI

include $(ARDMK_DIR)/Arduino.mk
