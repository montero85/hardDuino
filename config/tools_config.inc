TOOLS_DIR=/home/montero85/arduino-1.8.19/hardware/tools/avr/bin

# C Compiler and flags
CC=$(TOOLS_DIR)/avr-gcc
CFLAGS=-c -g -Os -Wall -std=gnu11 -ffunction-sections -fdata-sections -MMD -flto -fno-fat-lto-objects -mmcu=$(MMCU)

# C++ Compiler and flags
CXX=$(TOOLS_DIR)/avr-g++
CXXFLAGS=-c -g -Os -Wall -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -flto -MMD -mmcu=$(MMCU)

# Assembler and flags
AS=$(TOOLS_DIR)/avr-gcc
ASFLAGS=-c -g -x assembler-with-cpp -flto -MMD -mmcu=$(MMCU)

# AR Tools and flags
AR=$(TOOLS_DIR)/avr-gcc-ar
ARFLAGS=rcs

# Linker and flags
LD=$(TOOLS_DIR)/avr-gcc
LDFLAGS=-Wall -Os -g -flto -fuse-linker-plugin -Wl,--gc-sections -mmcu=$(MMCU)

# Print size
PRINTSIZE:=$(TOOLS_DIR)/avr-size -A

# Obj-copy to generate hex and eep
OBjCOPY=$(TOOLS_DIR)/avr-objcopy
OBjCOPYFLAGSEEP=-O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0
OBjCOPYFLAGSHEX=-O ihex -R .eeprom
OBjCOPYFLAGSBIN=-O binary -R .eeprom
