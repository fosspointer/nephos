CXX := cross/bin/i686-elf-g++
LD := cross/bin/i686-elf-ld
AS := nasm -g
CXXFLAGS := -nostdlib -nostartfiles -ffreestanding -fno-exceptions -fno-rtti -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -O0 -std=c++23 -c
LDFLAGS := -T linker.ld -nostdlib
SRC_DIR := src
BUILD_DIR := build

KERNEL_CPP_SOURCES := $(wildcard $(SRC_DIR)/kernel/*.cpp)
KERNEL_CPP_HEADERS := $(wildcard $(SRC_DIR)/kernel/*.hpp)
KERNEL_ASM_SOURCES := $(wildcard $(SRC_DIR)/kernel/*.asm)
KERNEL_CPP_OBJECTS := $(KERNEL_CPP_SOURCES:$(SRC_DIR)/kernel/%.cpp=$(BUILD_DIR)/kernel/%.o)
KERNEL_ASM_OBJECTS := $(KERNEL_ASM_SOURCES:$(SRC_DIR)/kernel/%.asm=$(BUILD_DIR)/kernel/%.o)

BUILD_IMAGE := $(BUILD_DIR)/boot.img

.PHONY: always clean all boot kernel image
all: always boot kernel image
test: all
	qemu-system-x86_64 -drive file=$(BUILD_IMAGE),index=0,if=floppy,format=raw
always:
	mkdir -p $(BUILD_DIR)/boot
	mkdir -p $(BUILD_DIR)/kernel
boot: $(BUILD_DIR)/boot/boot.bin
kernel: $(BUILD_DIR)/kernel.bin
image: $(BUILD_IMAGE)

$(BUILD_DIR)/boot/boot.bin: $(SRC_DIR)/boot/boot.asm
	$(AS) -f bin -o $@ $<

$(BUILD_IMAGE): boot kernel
	dd if=/dev/zero of=$(BUILD_IMAGE) bs=512 count=2880
	mkfs.fat -F 12 $(BUILD_IMAGE)
	dd if=$(BUILD_DIR)/boot/boot.bin of=$(BUILD_IMAGE) conv=notrunc

	mcopy -i $(BUILD_IMAGE) $(BUILD_DIR)/kernel.bin "::kernel.bin"

$(KERNEL_CPP_OBJECTS): $(KERNEL_CPP_HEADERS)

$(KERNEL_CPP_OBJECTS): $(BUILD_DIR)/kernel/%.o: $(SRC_DIR)/kernel/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

$(KERNEL_ASM_OBJECTS): $(BUILD_DIR)/kernel/%.o: $(SRC_DIR)/kernel/%.asm
	$(AS) -f elf32 -o $@ $<

$(BUILD_DIR)/kernel.bin: $(KERNEL_CPP_HEADERS) $(KERNEL_CPP_OBJECTS) $(KERNEL_ASM_OBJECTS)
	$(LD) -o $(BUILD_DIR)/kernel.elf $(LDFLAGS) $(KERNEL_ASM_OBJECTS) $(KERNEL_CPP_OBJECTS)
	objcopy -O binary $(BUILD_DIR)/kernel.elf $(BUILD_DIR)/kernel.bin

clean:
	@if [ -d "$(BUILD_DIR)" ]; then \
		echo "Cleaning up $(BUILD_DIR)..."; \
		rm -rf $(BUILD_DIR); \
	else \
		echo "$(BUILD_DIR) does not exist, nothing to clean."; \
	fi