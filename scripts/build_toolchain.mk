TARGET := i686-elf
TOOLCHAIN_DIR := toolchain
TOOLCHAIN_PREFIX := $(TOOLCHAIN_DIR)/$(TARGET)

FIND_VERSION_REGEX := "((\.){0,1}[0-9])+"
FIND_VERSION_COMMAND := grep -Po $(FIND_VERSION_REGEX)

BINUTILS_FTP_PATH := https://ftp.gnu.org/gnu/binutils
BINUTILS_FILENAME := binutils-2.41.tar.gz
BINUTILS_FTP_URL := $(BINUTILS_FTP_PATH)/$(BINUTILS_FILENAME)
BINUTILS_VERSION := $(shell echo "$(BINUTILS_FILENAME)" | $(FIND_VERSION_COMMAND))
BINUTILS_BUILD_DIRNAME := binutils_build-$(BINUTILS_VERSION)
BINUTILS_BUILD_DIR :=  $(TOOLCHAIN_DIR)/$(BINUTILS_BUILD_DIRNAME)

GCC_FTP_PATH := https://ftp.gnu.org/gnu/gcc/gcc-13.2.0
GCC_VERSION := $(shell echo "$(GCC_FTP_PATH)" | $(FIND_VERSION_COMMAND))
GCC_FTP_FILENAME := gcc-$(GCC_VERSION).tar.gz
GCC_FTP_URL := $(GCC_FTP_PATH)/$(GCC_FILENAME)

test:
	@echo "gcc: $(GCC_VERSION)"
	@echo "binutils: $(BINUTILS_VERSION)"

all: checkup toolchain_binutils toolchain_gcc
	Built binutils (version $(BINUTILS_VERSION)), gcc (version $(GCC_VERSION))

checkup:
	[ -d $(TOOLCHAIN_DIR) ] && rm -rf $(TOOLCHAIN_DIR)/* 

clean:
	rm -rf toolchain

toolchain_binutils: checkup
	mkdir toolchain
	cd toolchain \
		&& wget $(BINUTILS_FTP_URL)	\
		&& tar -xf $(BINUTILS_FILENAME) \
		&& mkdir binutils_build-$(BINUTILS_VERSION) \
	cd toolchain/binutils_build-$(BINUTILS_VERSION) && ../binutils-$(BINUTILS_VERSION)/configure \
		--prefix="" \
		--target="" \
		--with-sysroot \
		--disable-nls \
		--disable-werror
	$(MAKE) -j10 -C ./toolchain/binutils-
	$(MAKE) install

toolchain_gcc: checkup toolchain_binutils
