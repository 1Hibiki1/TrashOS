BUILD_DIR = build

ASM_SOURCES = $(shell find src -type f -name '*.S')
CPP_SOURCES = $(shell find src -type f -name '*.cpp')
C_SOURCES = $(shell find src -type f -name '*.c')
OBJ_FILES= $(addprefix $(BUILD_DIR)/, $(CPP_SOURCES:.cpp=.o) $(C_SOURCES:.c=.o) $(ASM_SOURCES:.S=.o))

CC = arm-none-eabi-g++
CCC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

LD_SCRIPT = linker/tm4c123gh6pm.ld

# TODO: check these lol
OPTIMIZATION = -Og
CC_FLAGS = -Wno-missing-braces -Wall -Werror -Isrc/include -Isrc/include/user -Isrc/include/user/posix -ggdb -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16
CC_FLAGS += -mgeneral-regs-only
CC_FLAGS += -nostdlib -nostartfiles -ffreestanding -fno-exceptions -fno-rtti -mfloat-abi=softfp $(OPTIMIZATION) -MD -c

CCC_FLAGS = -Wno-missing-braces -Wall -Werror -Isrc/include -Isrc/include/user -Isrc/include/user/posix -ggdb -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16
CCC_FLAGS += -mfloat-abi=softfp $(OPTIMIZATION) -MD -std=c99 -c

LDFLAGS = -T $(LD_SCRIPT) -e Reset_Handler


kernel.bin: build/kernel.elf
	$(OBJCOPY) -O binary $< $(BUILD_DIR)/$@


build/kernel.elf: $(OBJ_FILES)
	$(LD) -o $@ $^ $(LDFLAGS)


# TODO: find a better way than using mkdir


$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CCC) $(CCC_FLAGS) -o $@ -c $<

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -o $@ -c $<

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(AS) -Isrc/include -ggdb $< -o $@


clean:
	rm -rf $(OBJ_FILES) $(BUILD_DIR)/*
