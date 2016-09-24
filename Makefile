BOARD_TAG = leonardo
ARDUINO_LIBS = HID HID-Project TimerOne

PROJECT = $(notdir $(patsubst %/,%, $(dir $(realpath $(firstword ${MAKEFILE_LIST})))))
OBJDIR = /tmp/${USER}-build-${PROJECT}-${BOARD_TAG}

include ${ARDMK_DIR}/Arduino.mk
