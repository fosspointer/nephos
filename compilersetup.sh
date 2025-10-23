#!/bin/bash 
set -e
export PREFIX=$(realpath "./cross")

if [ $? -ne 0 ]; then
    echo "Error: could not find the 'cross' directory"
    exit -1
fi 

GCC_MIRROR="https://ftp.gnu.org/gnu/gcc/gcc-14.2.0/"
GCC_PACKAGE_NAME="gcc-14.2.0"

BINUTILS_MIRROR="https://ftp.gnu.org/gnu/binutils/"
BINUTILS_PACKAGE_NAME="binutils-2.44"

PACKAGE_ENDING=".tar.gz"

mkdir -p $PREFIX || true
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
threads=$(nproc)

# BINUTILS
cd $PREFIX
wget --limit-rate=0 -nc "$BINUTILS_MIRROR$BINUTILS_PACKAGE_NAME$PACKAGE_ENDING"
tar -xf $BINUTILS_PACKAGE_NAME$PACKAGE_ENDING
cd $BINUTILS_PACKAGE_NAME
mkdir build || true
cd build
../configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j$threads && make install

# GCC
cd $PREFIX
wget --limit-rate=0 -nc "$GCC_MIRROR$GCC_PACKAGE_NAME$PACKAGE_ENDING"
tar -xf $GCC_PACKAGE_NAME$PACKAGE_ENDING
cd $GCC_PACKAGE_NAME
mkdir build || true
cd build
../configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make -j$threads all-gcc
make -j$threads all-target-libgcc
make -j$threads install-gcc
make -j$threads install-target-libgcc