#ifndef ADDRESSES_H
#define ADDRESSES_H

// Locations
#define KERNEL_LOC              0x100000
#define FILETABLE_LOC           0x7000

// Physical memory manager
#define MEMMAP_AREA             0x30000
#define SMAP_ENTRY_COUNT        0x8500
#define SMAP_ENTRIES            0x8504
#define PHYS_MEM_MAX_BLOCKS     0x1804
#define PHYS_MEM_USED_BLOCKS    0x1808

// Virtual memory manager
#define CURRENT_PAGE_DIR_ADDR   0x1800

#endif
