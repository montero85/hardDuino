################### ARDUINO UNO WIFI REV2 ###################################################

MMCU = atmega4809
VARIANT = uno2018
BOARD_DEFINES = F_CPU=16000000L\
                ARDUINO=10819\
                ARDUINO_AVR_UNO_WIFI_REV2\
                ARDUINO_ARCH_MEGAAVR\
                MILLIS_USE_TIMERB3
                
# CORE LIBRARY: core arduino files are built as a library to mimic the .ino behaviour
CORE_PATH := mega_avr/core_lib
CORE_VARIANT_SRC_PATH := $(SRC_DIR)/mega_avr/variants/$(VARIANT)
CORE_PATH_OBJ_DIR := $(OBJ_DIR)/$(CORE_PATH)
CORE_PATH_SRC_DIR := $(SRC_DIR)/$(CORE_PATH)
# Generate list of source files composing the core lib: all *.S, *.c, *.cpp files in CORE_PATH_SRC_DIR 
CORE_SRC = $(wildcard $(CORE_PATH_SRC_DIR)/*.S)
CORE_SRC += $(wildcard $(CORE_PATH_SRC_DIR)/*.c)
CORE_SRC += $(wildcard $(CORE_PATH_SRC_DIR)/*.cpp)
CORE_SRC += $(wildcard $(CORE_PATH_SRC_DIR)/api/*.cpp)

SRC+=$(CORE_VARIANT_SRC_PATH)/variant.c

OTHER_INCLUDE_PATHS= $(CORE_PATH_SRC_DIR)/api/deprecated $(CORE_PATH_SRC_DIR) $(CORE_VARIANT_SRC_PATH) 

# Include configuration for toolchain and uploader. Must be done after MMCU is defined
include $(CONFIG_DIR)/avr_tools_uno_wifi_rev2.inc
include $(CONFIG_DIR)/uploader_uno_wifi_rev2.inc

