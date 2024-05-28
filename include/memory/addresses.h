#ifndef ADDRESSES_H
#define ADDRESSES_H

/*
 * Memory layout of addresses 0x0000 - 0xA000
 * 0x0000-0x0fff - Probably shouldn't mess with this? idk it feels wrong
 * 0x1000-0x17ff - Free
 * 0x1800-0x1fff - Global Variables
 * 0x2000-0x2fff - Larger structs
 * 0x3000-0x3fff - Filesystem area
 * 0x4000-0x4fff - Font area
 * 0x6000-0x84ff - Free
 * 0x8500-0x8fff - SMAP Area
 * 0x9000-0x9fff - VBE info area
 */

// Global Variables
#define SLEEP_TIMER             0x1800
#define CURRENT_PAGE_DIR_ADDR   0x1804
#define PHYS_MEM_MAX_BLOCKS     0x1808
#define PHYS_MEM_USED_BLOCKS    0x180C

// Some structs
#define TERMINAL_INFO_LOC       0x2000
#define KEYBOARD_STATE_LOC      0x2100
#define DATETIME_LOC            0x2200

// Filesystem Area 0x3000 - 0x3fff
#define FILETABLE_LOC           0x3000

// Font area 0x4000 - 0x4fff
#define FONT_LOC                0x4000
#define FONT_WIDTH              (FONT_LOC)
#define FONT_HEIGHT             (FONT_LOC + 1)

// Smap Area 0x8500 - 0x8fff
#define SMAP_ENTRY_COUNT        0x8500
#define SMAP_ENTRIES            0x8504

// VBE Info Area 0x9000 - 0x9fff
#define MODE_INFO_BLOCK_LOC     0x9000 // 512 Bytes

// Other addresses
#define KERNEL_LOC              0x100000
#define MEMMAP_AREA             0x30000

#endif
