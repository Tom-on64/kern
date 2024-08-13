#include <memory/physical.h>
#include <memory/virtual.h>
#include <memory/addresses.h>
#include <disk/disk.h>
#include <fs/impl.h>
#include <fs/fs.h>
#include <screen/gfxmode.h>

void bigError();

__attribute__ ((section("entry")))
void main() {
    // Physical memory manager setup
    uint32_t smapEntryCount = *(uint32_t*)SMAP_ENTRY_COUNT;
    smapEntry_t* smapEntries = (smapEntry_t*)SMAP_ENTRIES;
    smapEntries += smapEntryCount - 1;
    uint32_t totalMemory = smapEntries->baseAddress + smapEntries->length - 1;

    setupPhysicalMemoryManager(MEMMAP_AREA, totalMemory);

    smapEntries = (smapEntry_t*)SMAP_ENTRIES;
    for (uint32_t i = 0; i < smapEntryCount; i++) {
        if (smapEntries[i].type == 1) {
            initMemoryRegion(smapEntries[i].baseAddress, smapEntries[i].length);
        }
    }

    deinitMemoryRegion(0x1000, 0x11000); // Reserve kernel memory (under 0x12000)
    deinitMemoryRegion(MEMMAP_AREA, maxBlocks / BLOCKS_PER_BYTE);

    // Load root dir, find kernel INode and parse it
    superblock_t* sb = (superblock_t*)SUPERBLOCK_LOC;
    sb->rootInodePtr = BOOT_FIRST_INODE_LOC + sizeof(inode_t);
    inode_t* root = (inode_t*)sb->rootInodePtr;
    diskRead(root->extent[0].block * 8, root->extent[0].length * 8, (void*)SCRATCH_BLOCK_LOC);

    inode_t* inode;
    if ((inode = getInode("kernel.bin")) == NULL) {
        // kernel.bin doesn't exist??? very bad -> error
        bigError(0xDEAD);
    } else { loadFile(inode, (void*)KERNEL_LOC); }

    if ((inode = getInode("term16n.fnt")) != NULL) {
        loadFile(inode, (void*)FONT_LOC);
    } else if ((inode = getInode("testfont.fnt")) != NULL) {
        loadFile(inode, (void*)FONT_LOC);
    } else {
        // No font found, bad.
        bigError(0xBeef);
    }

    // Virtual memory manager setup
    if (setupVirtualMemoryManager() != 0) {
        bigError(0xCafe);
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
    bigError(0xffffffff);
}

void rect(uint32_t c, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    uint8_t* vidmem = (uint8_t*)gfxMode->physicalBasePtr;
    uint8_t bytesPerPx = gfxMode->bpp / 8;
    uint32_t convColor = convertColor(c);

    for (uint32_t dx = 0; dx < w; dx++) {
        for (uint32_t dy = 0; dy < h; dy++) {
            uint32_t offset = (((y + dy) * gfxMode->xRes) + (x + dx)) * bytesPerPx;

            for (uint8_t i = 0; i < gfxMode->bpp; i++) {
                vidmem[offset + i] = (uint8_t)(convColor >> (i*8));
            }
        }
    }
}

/*
 * Something went VERY wrong. 
 * Eg. kernel.bin is missing, no filesystem, Virtual Memory failed, etc.
 */
void bigError(uint32_t err) {
    // Red BG
    rect(0x00ff0000, 0, 0, gfxMode->xRes, gfxMode->yRes);
    // E
    rect(0x00ffffff, 20, 10, 10, 50);
    rect(0x00ffffff, 30, 10, 30, 10);
    rect(0x00ffffff, 30, 30, 30, 10);
    rect(0x00ffffff, 30, 50, 30, 10);
    // R
    rect(0x00ffffff, 70, 10, 10, 50);
    rect(0x00ffffff, 70, 10, 30, 10);
    rect(0x00ffffff, 70, 30, 30, 10);
    rect(0x00ffffff, 90, 10, 10, 30);
    rect(0x00ffffff, 80, 40, 10, 10);
    rect(0x00ffffff, 90, 50, 10, 10);
    // R
    rect(0x00ffffff, 110, 10, 10, 50);
    rect(0x00ffffff, 110, 10, 30, 10);
    rect(0x00ffffff, 110, 30, 30, 10);
    rect(0x00ffffff, 130, 10, 10, 30);
    rect(0x00ffffff, 120, 40, 10, 10);
    rect(0x00ffffff, 130, 50, 10, 10);
    // O
    rect(0x00ffffff, 150, 10, 10, 50);
    rect(0x00ffffff, 170, 10, 10, 50);
    rect(0x00ffffff, 150, 10, 30, 10);
    rect(0x00ffffff, 150, 50, 30, 10);
    // R
    rect(0x00ffffff, 190, 10, 10, 50);
    rect(0x00ffffff, 190, 10, 30, 10);
    rect(0x00ffffff, 190, 30, 30, 10);
    rect(0x00ffffff, 210, 10, 10, 30);
    rect(0x00ffffff, 200, 40, 10, 10);
    rect(0x00ffffff, 210, 50, 10, 10);

    __asm__ volatile ("cli; hlt" :: "a"(err));
}

