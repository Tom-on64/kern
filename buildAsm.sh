#!/bin/bash

# Create ./build dir if it doesn't exist
if !(test -d ././build); then
    mkdir ././build
    echo "Created ./build/ directory!"
fi

# Bootloader
nasm -fbin src/boot.asm -o ./build/boot.bin
nasm -fbin src/filetable.asm -o ./build/filetable.bin

# Kernel
nasm -fbin src/kernel.asm -o ./build/kernel.bin

# Programs
nasm -fbin src/fs/calc.asm -o ./build/calc.bin
nasm -fbin src/fs/edit.asm -o ./build/edit.bin

cat ./build/calc.bin ./build/edit.bin > ./build/files.bin

# Final binary
cat ./build/boot.bin ./build/filetable.bin ./build/kernel.bin ./build/files.bin > ./build/temp.bin
dd if=/dev/zero of=./build/kern.iso bs=512 count=2880
dd if=./build/temp.bin of=./build/kern.iso conv=notrunc
rm -rf ./build/**.bin

echo "Built kern.iso!"

# Run QEMU
echo "Running QEMU..."
qemu-system-x86_64 -drive format=raw,file="./build/kern.iso",index=0,media=disk -m 256M
