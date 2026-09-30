CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE    := arm-none-eabi-size

OBJ_DIR := obj
BUILD   := build
MAP_DIR := map
TARGET  := firmware

CFLAGS   := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
            -O0 -g -Wall -ffunction-sections -fdata-sections
INCLUDES := -Iinc
LDFLAGS  := -T linker.ld -nostdlib -nostartfiles -Wl,--gc-sections \
            -Wl,-Map=$(MAP_DIR)/$(TARGET).map

# Auto-discover every .c and .s in src/
SRCS := $(wildcard src/*.c src/*.s)
OBJS := $(patsubst src/%,$(OBJ_DIR)/%.o,$(SRCS))

# define interface and target for OPENOCD
OPENOCD_INTERFACE := interface/stlink.cfg
OPENOCD_TARGET    := target/stm32f4x.cfg

all: $(BUILD)/$(TARGET).bin size

$(OBJ_DIR) $(BUILD) $(MAP_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.c.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

$(OBJ_DIR)/%.s.o: src/%.s | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/$(TARGET).elf: $(OBJS) | $(BUILD) $(MAP_DIR)
	$(CC) $(OBJS) $(CFLAGS) $(LDFLAGS) -o $@

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

size: $(BUILD)/$(TARGET).elf
	$(SIZE) $<

clean:
	rm -rf $(OBJ_DIR) $(BUILD) $(MAP_DIR)

flash: $(BUILD)/$(TARGET).elf
	openocd -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) \
		-c "program $< verify reset exit"

doc:
	mkdocs gh-deploy

-include $(OBJS:.o=.d)

.PHONY: all clean flash doc size
