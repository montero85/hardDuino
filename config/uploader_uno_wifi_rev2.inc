TOOLS_ROOT:=/home/montero85/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/
UPLOADER_DIR:=$(TOOLS_ROOT)/bin
UPLOADER_CONFIG_DIR:=$(TOOLS_ROOT)/etc
UPLOADER_CONFIG_FILE:=$(UPLOADER_CONFIG_DIR)/avrdude.conf

BOOTLOADER_HEX:=/home/montero85/.arduino15/packages/arduino/hardware/megaavr/1.8.7/bootloaders/atmega4809_uart_bl.hex

# Communication Settings
PORT:=usb
BAUD:=115200

# Tell to the upload target which file type to use
UPLOADER_BIN_TYPE:=hex

UPLOADER:=$(UPLOADER_DIR)/avrdude
UPLOADERFLAGS:=-C$(UPLOADER_CONFIG_FILE) -v -p$(MMCU) -cxplainedmini_updi -P$(PORT) -b$(BAUD) -e -D
UPLOADERFLAGSEXTRA:=-Ufuse2:w:0x01:m -Ufuse5:w:0xC9:m -Ufuse8:w:0x02:m -Uflash:w:$(BOOTLOADER_HEX):i
