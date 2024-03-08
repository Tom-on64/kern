#!/bin/bash

[[ -d ./build ]] || mkdir build

rm -rf ./build/**

# Bootloader
nasm -fbin ./src/boot.asm -o ./build/boot.bin

# Kernel
nasm -felf32 ./src/kernel/entry.asm -o ./build/entry.o

for file in ./src/kernel/**.c; do
    filename=$(basename "$file" .c)
    i386-elf-gcc -ffreestanding -Wall -Wextra -m32 -c $file -o ./build/$filename.o
done

i386-elf-ld -o ./build/kernel.bin -Ttext 0x1000 --oformat binary ./build/**.o

# OS Image
cat build/boot.bin build/kernel.bin > build/os.bin
dd if=/dev/zero of=./kern.iso bs=512 count=2880
dd if=./build/os.bin of=./kern.iso conv=notrunc

# Run QEMU
qemu-system-x86_64 -drive format=raw,file="./kern.iso",index=0,media=disk -m 256M -accel tcg

