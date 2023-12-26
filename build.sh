#!/bin/bash

# Create build dir if it doesn't exist
if !(test -d ./build); then
    mkdir ./build
fi

# Bootloader
nasm -fbin src/boot.asm -o build/boot.bin
nasm -fbin src/filetable.asm -o build/filetable.bin
cat build/boot.bin build/filetable.bin > build/bootloader.bin

# Kernel
nasm -fbin src/kernel.asm -o build/kernel.bin

# Programs
nasm -fbin src/fs/calc.asm -o build/calc.bin
nasm -fbin src/fs/edit.asm -o build/edit.bin

cat build/calc.bin build/edit.bin > build/files.bin

# Final binary
cat build/bootloader.bin build/kernel.bin build/files.bin > build/kern.bin

# Cleanup
rm build/*[!kern].bin

# Run QEMU
qemu-system-x86_64 -drive format=raw,file="./build/kern.bin",index=0,media=disk -m 256M
