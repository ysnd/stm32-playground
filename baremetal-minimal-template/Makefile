CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -Wall -O0 -g -mcpu=cortex-m3 -mthumb -ffreestanding
LDFLAGS = -Tlinker.ld -nostartfiles

TARGET = stm32

all: $(TARGET).bin

$(TARGET).elf: startup.s main.c 
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

flash:
	st-flash write $(TARGET).bin 0x08000000

clean:
	rm -f *.elf *.bin
