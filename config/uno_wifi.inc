################### ARDUINO UNO WIFI ###################################################
MMCU = atmega328p
VARIANT = standard
BOARD_DEFINES =  F_CPU=16000000L\
                 ARDUINO=10819\
                 ARDUINO_AVR_UNO_WIFI_DEV_ED\
                 ARDUINO_ARCH_AVR ESP_CH_UART\
                 ESP_CH_UART_BR=19200

# CORE LIBRARY: core arduino files are built as a library to mimic the .ino behaviour
CORE_PATH := avr/core_lib
CORE_VARIANT_PATH := avr/variants/$(VARIANT)
CORE_PATH_OBJ_DIR := $(OBJ_DIR)/$(CORE_PATH)
CORE_PATH_SRC_DIR := $(SRC_DIR)/$(CORE_PATH)
# Generate list of source files composing the core lib: all *.S, *.c, *.cpp files in CORE_PATH_SRC_DIR 
CORE_SRC = $(wildcard $(CORE_PATH_SRC_DIR)/*.S)
CORE_SRC += $(wildcard $(CORE_PATH_SRC_DIR)/*.c)
CORE_SRC += $(wildcard $(CORE_PATH_SRC_DIR)/*.cpp)

# Publish core library and core variant public headers
PUBLIC_HEADERS += $(wildcard $(CORE_PATH_SRC_DIR)/*.h)
PUBLIC_HEADERS += $(wildcard $(SRC_DIR)/$(CORE_VARIANT_PATH)/*.h)

# Include configuration for toolchain and uploader. Must be done after MMCU is defined
include $(CONFIG_DIR)/avr_tools_uno_wifi.inc
include $(CONFIG_DIR)/uploader_uno_wifi.inc
