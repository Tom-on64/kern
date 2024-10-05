#ifndef MEMORY_ADDRESSES_H
#define MEMORY_ADDRESSES_H

/*
 * Memory layout of addresses 0x0000 - 0x12000
 * 0x0000-0x0fff - Probably shouldn't mess with this? idk it feels wrong
 * 0x1000-0x17ff - Free
 * 0x1800-0x1fff - Global Variables
 * 0x2000-0x2fff - Larger structs
 * 0x3000-0x3fff - Free
 * 0x4000-0x4fff - Font area
 * 0x6000-0x6fff - VBE info area
 * 0x7000-0x7bff - SMAP Area
 * 0x7c00-0x8bff - Bootblock
 * 0x8c00-0x9bff - Superblock
 * 0x9c00-0xafff - Free
 * 0xb000-0xbfff - Bootloader first INode
 * 0xc000-0xcfff - Scratch INode block
 */

// Global Variables 0x1800-0x1fff
#define SLEEP_TIMER             0x1800
#define CURRENT_PAGE_DIR_ADDR   0x1804
#define PHYS_MEM_MAX_BLOCKS     0x1808
#define PHYS_MEM_USED_BLOCKS    0x180C

// Some structs 0x2000-0x2fff
#define TERMINAL_INFO_LOC       0x2000
#define KEYBOARD_STATE_LOC      0x2100
#define DATETIME_LOC            0x2200

// Filesystem Area 0x3000 - 0x3fff
#define OPEN_FILE_TABLE_LOC     0x3000

// Font area 0x4000 - 0x4fff
#define FONT_LOC                0x4000

// VBE Info Area 0x6000 - 0x6fff
#define MODE_INFO_BLOCK_LOC     0x6000 // 512 Bytes

// Smap Area 0x7000 - 0x7bff
#define SMAP_ENTRY_COUNT        0x7000
#define SMAP_ENTRIES            0x7004

// Bootblock 0x7c00-0x9bff
#define BOOTBLOCK_LOC           0x7c00

// Superblock 0x9c00-0xabff
#define SUPERBLOCK_LOC          0x8c00

// First INode from bootloader
#define BOOT_FIRST_INODE_LOC    0xb000
#define SCRATCH_BLOCK_LOC       0xc000

// Other addresses
#define KERNEL_LOC              0x100000
#define KERNEL_MALLOC_AREA      0x300000
#define MEMMAP_AREA             0x30000
#define OPEN_FILE_LOC           0xd000

#endif
