CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy

CFLAGS  := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O0 -g -Wall
INCLUDES := -Iinc
LDFLAGS := -T linker.ld -nostdlib -nostartfiles -Wl,--gc-sections

SRCS  := startup.s src/main.c src/gpio.c src/rcc.c
OBJ_DIR := obj
BUILD   := build

TARGET := firmware

# Route every source's .o into obj/, flattening any subdirectory
OBJS := $(addprefix $(OBJ_DIR)/,$(notdir $(SRCS)))
OBJS := $(OBJS:.c=.o)
OBJS := $(OBJS:.s=.o)

OPENOCD_INTERFACE := interface/stlink.cfg
OPENOCD_TARGET     := target/stm32f4x.cfg

all: $(BUILD)/$(TARGET).bin

$(OBJ_DIR) $(BUILD):
	mkdir -p $@

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: %.s | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/$(TARGET).elf: $(OBJS) | $(BUILD)
	$(CC) $(OBJS) $(CFLAGS) $(LDFLAGS) -o $@

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(OBJ_DIR) $(BUILD)

flash: $(BUILD)/$(TARGET).elf
	openocd -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) \
		-c "program $< verify reset exit"

doc:
	mkdocs gh-deploy

.PHONY: all clean flash doc
