#ifndef ADDRESSES_H
#define ADDRESSES_H

/* TODO: Follow this layout and/or change it to make more sense
 * Memory layout
 * 0x0000-0x0fff - Probably shouldn't mess with this? idk
 * 0x1000-0x17ff - Free
 * 0x1800-0x1fff - Global Variables
 * 0x2000-0x2fff - Terminal area
 * 0x3000-0x3fff - Keyboard area
 * 0x4000-0x5fff - Filesytem area
 * 0x6000-0x7fff - Font area
 * 0x8000-0x8fff - Free
 * 0x9000-0x9fff - VBE info area
 */

// Locations
#define KERNEL_LOC              0x100000
#define DATETIME_LOC            0x2000
#define TERMINAL_INFO_LOC       0x8000
#define KEYBOARD_STATE_LOC      0x3000
#define FILETABLE_LOC           0x4000
#define MODE_INFO_BLOCK_LOC     0x9000 // 512 Bytes

// Variables
#define CURRENT_PAGE_DIR_ADDR   0x1800
#define PHYS_MEM_MAX_BLOCKS     0x1804
#define PHYS_MEM_USED_BLOCKS    0x1808
#define SLEEP_TIMER             0x180C

// Font
#define FONT_LOC                0x6000
#define FONT_WIDTH              (FONT_LOC)
#define FONT_HEIGHT             (FONT_LOC + 1)

// Physical memory manager
#define MEMMAP_AREA             0x30000
#define SMAP_ENTRY_COUNT        0x8500
#define SMAP_ENTRIES            0x8504

#endif
