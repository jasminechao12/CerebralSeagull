TOOLS   = arm-none-eabi
AS      = $(TOOLS)-as
CC      = $(TOOLS)-gcc
CXX     = $(TOOLS)-g++
LD      = $(TOOLS)-ld.bfd
OBJCOPY = $(TOOLS)-objcopy
DUMP    = $(TOOLS)-objdump -D
GDB     = $(TOOLS)-gdb
MKDIR_P = mkdir -p
CP      = cp

############### LIBRARY INCLUSION ######################

USER_PROJ       = default
FLOAT           = soft
DEBUG           = 1
USER_ARG        = 0

USER_PROJ_BUILD  = user
PROJ_BUILD       = kernel
USER_PROJ_COMMON = user_common
PROJ             = kernel
BUILD            = build
BOOT             = asm
BIN              = bin

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
	NC_FLAGS=-q 0
else
	NC_FLAGS=
endif

# Terminal colors
n := $(shell tty -s && tput sgr0)
r := $(shell tty -s && tput setaf 1)
g := $(shell tty -s && tput setaf 2)
y := $(shell tty -s && tput setaf 3)
j := $(shell tty -s && tput setaf 5)
b := $(shell tty -s && tput bold)
u := $(shell tty -s && tput smul)

# BIN INFO
HASH_KERNEL      = $(shell echo -n "$(DEBUG)$(OPTIMIZATION)$(FLOAT)" | md5sum | cut -d' ' -f1)
HASH_USER        = $(shell echo -n "$(DEBUG)$(OPTIMIZATION)$(FLOAT)$(USER_ARG)" | md5sum | cut -d' ' -f1)
BIN_DIR          = $(BUILD)/$(BIN)
BINARY           = $(PROJ)_$(USER_PROJ)_$(HASH_USER)

# KERNEL PROJ DIRS
K_BOOT_DIR       = $(PROJ)/$(BOOT)
U_BOOT_DIR       = $(USER_PROJ_COMMON)/$(BOOT)
K_INC_DIR        = $(PROJ)/include
K_LIB_DIR        = $(PROJ)/lib
K_SRC_DIR        = $(PROJ)/src
K_OBJ_DIR        = $(BUILD)/$(PROJ_BUILD)
K_OBJ_PROJ_DIR   = $(K_OBJ_DIR)/$(PROJ)_$(HASH_KERNEL)
TUSB_DIR         = tinyusb

# USER PROJ DIRS
U_COMMON_INC_DIR = $(USER_PROJ_COMMON)/include
U_COMMON_SRC_DIR = $(USER_PROJ_COMMON)/src
U_LIB_DIR        = $(USER_PROJ_COMMON)/lib
U_PROJ_DIR       = user_proj/$(USER_PROJ)/src
U_PROJ_INC_DIR   = user_proj/$(USER_PROJ)/include
U_OBJ_DIR        = $(BUILD)/$(USER_PROJ_BUILD)
U_OBJ_PROJ_DIR   = $(U_OBJ_DIR)/$(USER_PROJ)_$(HASH_USER)

# TinyUSB Sources
TUSB_SRC = $(TUSB_DIR)/src/tusb.c \
           $(TUSB_DIR)/src/common/tusb_fifo.c \
           $(TUSB_DIR)/src/device/usbd.c \
           $(TUSB_DIR)/src/device/usbd_control.c \
           $(TUSB_DIR)/src/class/hid/hid_device.c \
           $(TUSB_DIR)/src/class/cdc/cdc_device.c \
           $(TUSB_DIR)/src/portable/synopsys/dwc2/dcd_dwc2.c \
           $(TUSB_DIR)/src/portable/synopsys/dwc2/dwc2_common.c

# SRC FILES
CORE_K_SRC = $(wildcard $(K_SRC_DIR)/*.c)
K_SRC_CPP         = $(K_SRC_DIR)/blink_service.cpp
U_SRC             = $(wildcard $(U_PROJ_DIR)/*.c)
U_SRC_COMMON      = $(wildcard $(U_COMMON_SRC_DIR)/*.c)
U_BOOT            = $(wildcard $(U_BOOT_DIR)/*.S)
K_BOOT            = $(wildcard $(K_BOOT_DIR)/*.S)

# RULES / OBJECTS
K_OBJ_RULE        = $(CORE_K_SRC:$(K_SRC_DIR)/%.c=$(K_OBJ_PROJ_DIR)/%.o)
K_TUSB_OBJ_RULE   = $(TUSB_SRC:%.c=$(K_OBJ_PROJ_DIR)/%.o)
K_CPP_OBJ_RULE    = $(K_SRC_CPP:$(K_SRC_DIR)/%.cpp=$(K_OBJ_PROJ_DIR)/%.o)
K_BOOT_RULE       = $(K_BOOT:$(K_BOOT_DIR)/%.S=$(K_OBJ_PROJ_DIR)/%.o)
U_OBJ_RULE        = $(U_SRC:$(U_PROJ_DIR)/%.c=$(U_OBJ_PROJ_DIR)/%.o)
U_COMMON_OBJ_RULE = $(U_SRC_COMMON:$(U_COMMON_SRC_DIR)/%.c=$(U_OBJ_PROJ_DIR)/%.o)
U_BOOT_RULE       = $(U_BOOT:$(U_BOOT_DIR)/%.S=$(U_OBJ_PROJ_DIR)/%.o)

K_OBJECTS         = $(K_OBJ_RULE) $(K_TUSB_OBJ_RULE) $(K_CPP_OBJ_RULE) $(K_BOOT_RULE)
U_OBJECTS         = $(U_OBJ_RULE) $(U_BOOT_RULE) $(U_COMMON_OBJ_RULE)

# Path to soft float lib
SOFT_FLOAT_LIB    = $(U_LIB_DIR)/soft_float/libgcc.a

ifeq ($(FLOAT), soft)
    # Check your folder: if libm.a isn't in soft_float, remove it from this list
    U_LIB_FILES = user_common/lib/soft_float/libc.a \
                  user_common/lib/soft_float/libgcc.a
    FLOAT_ARCH  = -mfloat-abi=soft
else
    U_LIB_FILES = user_common/lib/hard_float/libc.a \
                  user_common/lib/hard_float/libm.a \
                  user_common/lib/soft_float/libgcc.a
    FLOAT_ARCH  = -mfloat-abi=hard -mfpu=fpv4-sp-d16 -march=armv7e-m
endif

ifeq ($(DEBUG), 1)
	DEFINE_MACROS = -DDEBUG -g
	OPTIMIZATION  = -O0
else
	OPTIMIZATION = -O3 -funroll-all-loops
endif

ARCH                 = $(ARG) $(FLOAT_ARCH) -mslow-flash-data -mcpu=cortex-m4 -mlittle-endian -mthumb -ffreestanding
COMPILER_ERROR_FLAGS = -std=gnu99 -Wall -Werror -Wshadow -Wextra -Wunused
C_LIB_FLAG           = -nostdlib
CCFLAGS              += $(ARCH) $(COMPILER_ERROR_FLAGS) $(C_LIB_FLAG) $(OPTIMIZATION) $(DEFINE_MACROS)
K_CCFLAGS            = $(CCFLAGS) -nostartfiles -I$(K_INC_DIR) -I$(TUSB_DIR)/src
U_CCFLAGS            = $(CCFLAGS) -I$(U_COMMON_INC_DIR) -I$(U_PROJ_INC_DIR)

# Simplified CXXFLAGS for Heuristic DSP
CXXFLAGS = $(ARCH) $(OPTIMIZATION) $(DEFINE_MACROS) -Wall -fno-rtti -fno-exceptions \
           -ffreestanding -std=c++11 -fno-threadsafe-statics -I$(K_INC_DIR) -I$(TUSB_DIR)/src

################### ROOT RULES #########################
.PHONY: setup flash clean build compile

compile: $(BIN_DIR)/$(BINARY).bin
	@printf "\n$g$b$uBuilt PROJ=$(PROJ) with USER_PROJ=$(USER_PROJ)$n$n\n"

setup:
	$(MKDIR_P) $(BUILD) $(BIN_DIR) $(U_OBJ_DIR) $(K_OBJ_DIR) $(K_OBJ_PROJ_DIR) $(U_OBJ_PROJ_DIR)
	$(MKDIR_P) $(K_OBJ_PROJ_DIR)/tinyusb/src/common
	$(MKDIR_P) $(K_OBJ_PROJ_DIR)/tinyusb/src/device
	$(MKDIR_P) $(K_OBJ_PROJ_DIR)/tinyusb/src/class/hid
	$(MKDIR_P) $(K_OBJ_PROJ_DIR)/tinyusb/src/portable/synopsys/dwc2

getargs:
	python3 util/generate_argv.py /tmp/349_arg.h $(USER_ARG)

build : getargs setup compile

flash: build
	cp util/init_template.nc /tmp/init.nc
	sed -i -e 's|<template>|$(BINARY)|g' /tmp/init.nc
	cp util/init_template.gdb /tmp/init.gdb
	sed -i -e 's|<template>|$(BINARY)|g' /tmp/init.gdb
	@printf "\n$yFlashing $(BINARY) to board...\n$n"
	cat /tmp/init.nc | nc localhost 4444 $(NC_FLAGS)
	@printf "Opening GDB...\n"
	$(GDB) -x /tmp/init.gdb

################# COMPILATION RULES ####################

$(K_OBJ_PROJ_DIR)/%.o: $(K_SRC_DIR)/%.c
	@printf "\n$b$yCompiling K_C: $<$n\n"
	$(CC) $(K_CCFLAGS) -c $< -o $@

$(K_OBJ_PROJ_DIR)/%.o: $(K_SRC_DIR)/%.cpp
	@printf "\n$b$yCompiling K_CPP: $<$n\n"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(K_OBJ_PROJ_DIR)/tinyusb/%.o: tinyusb/%.c
	@mkdir -p $(dir $@)
	$(CC) $(K_CCFLAGS) -c $< -o $@

$(K_OBJ_PROJ_DIR)/%.o: $(K_BOOT_DIR)/%.S
	@printf "\n$y$bAssembling K_ASM: $<$n\n"
	$(CC) $(K_CCFLAGS) -c $< -o $@

$(U_OBJ_PROJ_DIR)/%.o: $(U_COMMON_SRC_DIR)/%.c
	@$(CC) $(U_CCFLAGS) -c $< -o $@

$(U_OBJ_PROJ_DIR)/%.o: $(U_BOOT_DIR)/%.S
	@$(CC) $(U_CCFLAGS) -c $< -o $@

$(U_OBJ_PROJ_DIR)/%.o: $(U_PROJ_DIR)/%.c
	@$(CC) $(U_CCFLAGS) -c $< -o $@

################### BINARY RULES #######################

$(BIN_DIR)/$(BINARY).bin: $(BIN_DIR)/$(BINARY).elf
	$(OBJCOPY) $< $@ -O binary

$(BIN_DIR)/$(BINARY).elf: $(K_OBJECTS) $(U_OBJECTS)
	@cp util/linker_template.lds /tmp/linker.lds
	@sed -i -e 's|<K_OBJ_DIR>|$(K_OBJ_PROJ_DIR)|g' /tmp/linker.lds
	@sed -i -e 's|<U_OBJ_DIR>|$(U_OBJ_PROJ_DIR)|g' /tmp/linker.lds
	@printf "\n$j$bLinking $(BINARY)...$n\n"
	$(CXX) -T /tmp/linker.lds $(ARCH) $(C_LIB_FLAG) -static -o $@ \
	    $(U_OBJECTS) \
	    $(K_OBJECTS) \
	    $(U_LIB_FILES) \
	    $(U_LIB_FILES) \
	    -lgcc

clean:
	$(RM) -r $(BUILD)