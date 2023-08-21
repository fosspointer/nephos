ASM := nasm
SRC_DIR := src
BUILD_DIR := build

.PHONY: always clean bootloader bootloader_floppy all
all: always bootloader bootloader_floppy

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/*

bootloader: $(BUILD_DIR)/bootloader.bin

bootloader_floppy: bootloader $(BUILD_DIR)/bootloader_floppy.img

$(BUILD_DIR)/bootloader.bin: $(SRC_DIR)/bootloader.asm
	$(ASM) -isrc/ -f bin $(SRC_DIR)/bootloader.asm -o $(BUILD_DIR)/bootloader.bin
$(BUILD_DIR)/bootloader_floppy.img: $(BUILD_DIR)/bootloader.bin
	cp $(BUILD_DIR)/bootloader.bin $(BUILD_DIR)/bootloader_floppy.img
	truncate -s 1440k $(BUILD_DIR)/bootloader_floppy.img
