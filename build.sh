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

# Final binary
nasm -fbin src/calc.asm -o build/calc.bin
nasm -fbin src/message.asm -o build/message.bin
cat build/bootloader.bin build/kernel.bin build/calc.bin build/message.bin > build/os.bin

# Run QEMU
qemu-system-x86_64 -drive format=raw,file="./build/os.bin",index=0,media=disk -m 256M
