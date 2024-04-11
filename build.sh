#!/bin/bash

[[ -d ./build ]] || mkdir build

rm -rf ./build/**

# Bootloader
echo "Assembling ./src/boot.asm..."
nasm -fbin ./src/boot.asm -o ./build/boot.bin

# Kernel
echo "Assembling ./src/kernel/entry.asm..."
nasm -felf32 ./src/kernel/entry.asm -o ./build/entry.o

for file in ./src/kernel/*.c; do
    filename=$(basename "$file" .c)
    echo "Compiling $file..."
    i386-elf-gcc -ffreestanding -Wall -Wextra -m32 -c "$file" -o "./build/$filename.o"
done

echo "Linking kernel object files..."
mv ./build/disk.o ./build/f_disk.o # Very temporary fix to issue with disk.o not linking or something weird
i386-elf-ld -o ./build/kernel.bin -Ttext 0x1000 --oformat binary ./build/*.o

# OS Image
echo "Creating kern.iso disk image..."
cat build/boot.bin build/kernel.bin > build/os.bin
dd if=/dev/zero of=./kern.iso bs=512 count=2880
dd if=./build/os.bin of=./kern.iso conv=notrunc
dd if=./fs/test.txt of=./kern.iso bs=512 seek=21 conv=notrunc   # We skip 21 (10.5kB) because that's the bootloader (0.5kB) and kernel (10kB)

echo -e "\nDone!\n"

# Run QEMU
qemu-system-x86_64 -drive format=raw,file="./kern.iso",index=0,media=disk -m 256M -accel tcg

