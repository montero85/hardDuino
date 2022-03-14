TARGET=blink

BUILD_DIR := build
SRC_DIR := src
CONFIG_DIR := config
TARGET_DIR := $(BUILD_DIR)/bin
OBJ_DIR := $(BUILD_DIR)/objects
LIB_DIR := $(BUILD_DIR)/libs
PUBLIC_HEADERS_DIR := $(BUILD_DIR)/public_headers

# Target specific configuration (i.e. source files, public headers, ...)
include $(TARGET).inc

# Board specific configuration (i.e. board specific build define, -mmcu flag)
# Note: at the moment, configuration is fixed to Arduino Mega WiFi. It should be made configurable.
include $(CONFIG_DIR)/board_config.inc

# Build tools and their configuration (i.e. CC, CFLAGS, CXX, CXXFLAGS)
include $(CONFIG_DIR)/tools_config.inc

# Generate list of objects from TARGET source files. 
# i.e. File SRC_DIR/foo/bar.c will result into OBJ_DIR/foo/bar_c.o
# the _c, _cpp, _S suffixes are used to allow two files to hold same name 
# but different extension (i.e. foo.S, foo.c)  
OBJS := $(subst $(SRC_DIR), $(OBJ_DIR), $(SRC:.S=_s.o))
OBJS := $(subst $(SRC_DIR), $(OBJ_DIR), $(OBJS:.c=_c.o))
OBJS := $(subst $(SRC_DIR), $(OBJ_DIR), $(OBJS:.cpp=_cpp.o))

# CORE LIBRARY: core arduino files are built as a library to mimic the .ino behaviour
CORE_PATH := avr/core_lib
CORE_VARIANT_PATH := avr/variants/$(VARIANT)
CORE_PATH_OBJ_DIR := $(OBJ_DIR)/$(CORE_PATH)
CORE_PATH_SRC_DIR := $(SRC_DIR)/$(CORE_PATH)
# Generate list of source files composing the core lib: all *.S, *.c, *.cpp files in CORE_PATH_SRC_DIR 
CORE_SRC = $(wildcard $(CORE_PATH_SRC_DIR)/*.S)
CORE_SRC += $(wildcard $(CORE_PATH_SRC_DIR)/*.c)
CORE_SRC += $(wildcard $(CORE_PATH_SRC_DIR)/*.cpp)

# Core Arduino Headers are all published (despite some may be _private.h)
PUBLIC_HEADERS += $(wildcard $(CORE_PATH_SRC_DIR)/*.h)
PUBLIC_HEADERS += $(wildcard $(SRC_DIR)/$(CORE_VARIANT_PATH)/*.h)

# Generate list of core lib objects from source files. Same as $(OBJS)
CORE_OBJS := $(subst $(SRC_DIR), $(OBJ_DIR), $(CORE_SRC:.S=_S.o))
CORE_OBJS := $(subst $(SRC_DIR), $(OBJ_DIR), $(CORE_OBJS:.c=_c.o))
CORE_OBJS := $(subst $(SRC_DIR), $(OBJ_DIR), $(CORE_OBJS:.cpp=_cpp.o))

# .d files are "makefiles" with recipes telling us which headers a specific object depends on.
DEPS := $(CORE_OBJS:.o=.d)
DEPS += $(OBJS:.o=.d)

# -D (Define) preprocessor flags
DEFINES := $(BOARD_DEFINES)
DFLAGS := $(addprefix -D, $(DEFINES))

# -I (Include) preprocessor flags: include public (exported) headers and "private" headers 
# that are in the same folder as the source currently build ( $(dir $<))
EXPORTED_PUBLIC_HEADERS := $(subst $(SRC_DIR), $(PUBLIC_HEADERS_DIR), $(PUBLIC_HEADERS))
# Extract directories, remove duplicates then add the -I flag
INCLUDE = $(sort $(dir $(EXPORTED_PUBLIC_HEADERS))) $(dir $<)
IFLAGS = $(addprefix -I, $(INCLUDE)) 

# TODO: Control using with -v cmdline param
VERBOSE := @

# COOKBOOK with all the recipes
all: $(TARGET_DIR)/$(TARGET).elf

$(TARGET_DIR)/$(TARGET).elf: $(OBJS) $(LIB_DIR)/core.a
	@echo "Linking $@ from $^"
	$(VERBOSE) mkdir -p $(dir $@)
	$(VERBOSE) $(LD) $(LDFLAGS) -o $@ $^ -L $(dir $@) -lm

$(LIB_DIR)/core.a: $(CORE_OBJS)
	@echo "Generating $@:"
	$(VERBOSE) mkdir -p $(dir $@)
	$(VERBOSE) $(AR) $(ARFLAGS) $@ $^

# Before compilation...
$(OBJS): $(EXPORTED_PUBLIC_HEADERS)

# ... copy public headers into the public header folder.
$(PUBLIC_HEADERS_DIR)/%.h: $(SRC_DIR)/%.h
	@echo "Exporting $< to $(dir $@)"
	$(VERBOSE) mkdir -p $(dir $@)
	$(VERBOSE) cp $< $(dir $@) 

$(OBJ_DIR)/%_c.o: $(SRC_DIR)/%.c
	@echo "Compiling $@ from $< $(CORE_OBJ)"
	$(VERBOSE) mkdir -p $(dir $@)
	$(VERBOSE) $(CC) $(CFLAGS) $(DFLAGS) $(IFLAGS) $< -o $@

$(OBJ_DIR)/%_cpp.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $@ from $<"
	$(VERBOSE) mkdir -p $(dir $@)
	$(VERBOSE) $(CXX) $(CXXFLAGS) $(DFLAGS) $(IFLAGS) $< -o $@

$(OBJ_DIR)/%_S.o: $(SRC_DIR)/%.S
	@echo "Assembling $@ from $<"
	$(VERBOSE) mkdir -p $(dir $@)
	$(VERBOSE) $(AS) $(ASFLAGS) $(DFLAGS) $(IFLAGS) $< -o $@

.PHONY: clean
clean:
	$(VERBOSE) $(RM) -r $(BUILD_DIR)/*

# Include .d makefiles. Ignore errors ("-") if they are missing (i.e. at the beginning)
-include $(DEPS)
