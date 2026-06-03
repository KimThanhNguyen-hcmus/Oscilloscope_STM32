TARGET = project
BUILD_DIR = Build
# Toolchain 
CC = arm-none-eabi-gcc 
OBJCOPY = arm-none-eabi-objcopy 
SZ = arm-none-eabi-size
# Flags
CPU = -mcpu=cortex-m3
MCU = $(CPU) -mthumb
OPT = -O0
CFLAGS = $(MCU) $(OPT) -Wall -g
CFLAGS += -IInc

LDSCRIPT = stm32f108cx.ld
LDFLAGS = $(MCU) -T$(LDSCRIPT) -nostdlib -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
C_SOURCES = $(wildcard Src/*.c)
OBJECTS = $(patsubst Src/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) | $(BUILD_DIR)
		$(CC) $(OBJECTS) $(LDFLAGS) -o $@
		$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
		$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
		$(OBJCOPY) -O binary -S $< $@

$(BUILD_DIR)/%.o: Src/%.c | $(BUILD_DIR)
		$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR):
		if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
.PHONY: clean flash
flash: $(BUILD_DIR)/$(TARGET).elf
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $< verify reset exit"
clean:
	-rmdir /s /q $(BUILD_DIR)