#!/bin/bash

[[ -d ./build ]] || mkdir build

rm -rf ./build/**

# Bootloader
echo "Assembling ./src/boot.asm..."
nasm -fbin ./src/boot.asm -o ./build/boot.bin

# Kernel
echo "Assembling ./src/entry.asm..."
nasm -felf32 ./src/entry.asm -o ./build/entry.o

for file in ./src/*.c; do
    filename=$(basename "$file" .c)
    echo "Compiling $file..."
    i386-elf-gcc -ffreestanding -Wall -Wextra -m32 -Iinclude -c "$file" -o "./build/$filename.o"
done

i386-elf-ld -o ./build/kernel.bin -Tkernel.ld --oformat binary ./build/*.o

# OS Image
echo "Creating kern.iso disk image..."
cat build/boot.bin build/kernel.bin > build/os.bin
dd if=/dev/zero of=./kern.iso bs=512 count=2880
dd if=./build/os.bin of=./kern.iso conv=notrunc

echo -e "\nDone!\n"

# Run QEMU
qemu-system-x86_64 -drive format=raw,file="./kern.iso",index=0,media=disk -m 256M -accel tcg

