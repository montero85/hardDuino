
include $(CONFIG_DIR)/avr_tools_common.inc

# Override LDFLAGS in common configuration
LDFLAGS=-Wall -Os -g -flto -fuse-linker-plugin -Wl,--gc-sections -Wl,--section-start=.text=0x200 -mmcu=$(MMCU)
