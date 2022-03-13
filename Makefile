TARGET=blink

BUILD_DIR=build
SRC_DIR:=src
CONFIG_DIR:=config
TARGET_DIR=$(BUILD_DIR)/bin
OBJ_DIR:=$(BUILD_DIR)/objects
LIB_DIR:=$(BUILD_DIR)/libs


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
CORE_PATH:=avr/core_lib
CORE_PATH_OBJ_DIR := $(OBJ_DIR)/$(CORE_PATH)
CORE_PATH_SRC_DIR := $(SRC_DIR)/$(CORE_PATH)
# Generate list of source files composing the core lib: all *.S, *.c, *.cpp files in CORE_PATH_SRC_DIR 
CORE_SRC = $(wildcard $(CORE_PATH_SRC_DIR)/*.S)
CORE_SRC += $(wildcard $(CORE_PATH_SRC_DIR)/*.c)
CORE_SRC += $(wildcard $(CORE_PATH_SRC_DIR)/*.cpp)
# Generate list of core lib objects from source files. Same as $(OBJS)
CORE_OBJS := $(subst $(SRC_DIR), $(OBJ_DIR), $(CORE_SRC:.S=_S.o))
CORE_OBJS := $(subst $(SRC_DIR), $(OBJ_DIR), $(CORE_OBJS:.c=_c.o))
CORE_OBJS := $(subst $(SRC_DIR), $(OBJ_DIR), $(CORE_OBJS:.cpp=_cpp.o))

# -D (Define) preprocessor flags
DEFINES := $(BOARD_DEFINES)
DFLAGS := $(addprefix -D, $(DEFINES))

# -I (Include) preprocessor flags. TODO: see how to do it.
IFLAGS := -I$(SRC_DIR)/avr/core_lib -I$(SRC_DIR)/avr/variants/standard

# COOKBOOK with all the recipes
all: $(TARGET_DIR)/$(TARGET).elf

$(TARGET_DIR)/$(TARGET).elf: $(OBJS) $(LIB_DIR)/core.a
	@echo "Linking $@ from $^"
	mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $^ -L $(dir $@) -lm

$(LIB_DIR)/core.a: $(CORE_OBJS)
	@echo "Generating $@:"
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/%_c.o: $(SRC_DIR)/%.c
	@echo "Compiling $@ from $< $(CORE_OBJ)"
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DFLAGS) $(IFLAGS) $< -o $@

$(OBJ_DIR)/%_cpp.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $@ from $<"
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DFLAGS) $(IFLAGS) $< -o $@

$(OBJ_DIR)/%_S.o: $(SRC_DIR)/%.S
	@echo "Assembling $@ from $<"
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $(DFLAGS) $(IFLAGS) $< -o $@

.PHONY: clean
clean:
	$(RM) -r $(OBJ_DIR)/*
	$(RM) -r $(LIB_DIR)/*
	$(RM) -r $(TARGET_DIR)/*
