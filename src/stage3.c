#include <memory/physical.h>
#include <memory/virtual.h>
#include <memory/addresses.h>
#include <disk/disk.h>
#include <disk/filesys.h>
#include <screen/gfxmode.h>

__attribute__ ((section("entry")))
void main() {
    diskRead(15, 1, filetable); // Load filetable into memory; TODO: Make a filetableUpdate() function that just keeps the stored and memory verisons in sync
    
    // Physical memory manager setup
    uint32_t smapEntryCount = *(uint32_t*)SMAP_ENTRY_COUNT;
    smapEntry_t* smapEntries = (smapEntry_t*)SMAP_ENTRIES; // After a unit32_t
    uint32_t totalMemory = smapEntries[smapEntryCount - 1].baseAddress + smapEntries[smapEntryCount - 1].length - 1;

    setupPhysicalMemoryManager(MEMMAP_AREA, totalMemory);

    for (uint32_t i = 0; i < smapEntryCount; i++) {
        if (smapEntries[i].type == 1) {
            initMemoryRegion(smapEntries[i].baseAddress, smapEntries[i].length);
        }
    }

    deinitMemoryRegion(0x1000, 0x9000); // Reserve kernel memory (under 0xa000)
    deinitMemoryRegion(MEMMAP_AREA, maxBlocks / BLOCKS_PER_BYTE);

    // Load kernel into memory
    if (readFile("kernel.bin", (void*)KERNEL_LOC) != 0) { // Check if kernel.bin was loaded
        // TODO: Add error messages
        while (1);
    }

    // Virtual memory manager setup
    if (setupVirtualMemoryManager() != 0) {
        while (1);
    }

    // Identity map VBE framebuffer
    uint32_t fbSize = (gfxMode->yRes * gfxMode->linearBytesPerScanline) / PAGE_SIZE;
    if (fbSize % PAGE_SIZE > 0) { fbSize++; }
    fbSize *= 2; // Double for hardware
    for (uint32_t i = 0, fbStart = gfxMode->physicalBasePtr; i < fbSize; i++, fbStart += PAGE_SIZE) {
        mapPage((void*)fbStart, (void*)fbStart);
    }
    deinitMemoryRegion(gfxMode->physicalBasePtr, fbSize * BLOCK_SIZE);

    *(uint32_t*)PHYS_MEM_MAX_BLOCKS = maxBlocks;
    *(uint32_t*)PHYS_MEM_USED_BLOCKS = usedBlocks;

    for (uint32_t virt = KERNEL_LOC; virt < 0x400000; virt += PAGE_SIZE) {
        unmapPage((void*)virt);
    }

    __asm__ volatile ("movl %cr3, %ecx; movl %ecx, %cr3");

    *(uint32_t*)CURRENT_PAGE_DIR_ADDR = (uint32_t)currentPd;

    // Call/jump to kernel
    ((void(*)(void))0xC0000000)();
}

