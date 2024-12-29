#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

#include "stdint.h"

#define __packed	__attribute__ ((packed))

#define HEAD_MAGIC 	0x1BADB002
#define BOOT_MAGIC	0x2BADB002

// Multiboot Header Flags
enum {
	MBH_PAGE_ALIGN	= (1<<0),
	MBH_MEMORY_INFO	= (1<<1),
	MBH_GRAPH_INFO	= (1<<2),
	MBH_ADDR_FIELDS = (1<<16),
};

struct multiboot_header {
	uint32_t magic;
	uint32_t flags;
	uint32_t checksum;
	uint32_t header_addr;
	uint32_t load_addr;
	uint32_t load_end_addr;
	uint32_t bss_end_addr;
	uint32_t entry_addr;
	uint32_t mode_type;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
} __packed;

// Multiboot Info Flags
enum {
	MBI_MEMORY	= (1<<0),
	MBI_BOOT_DEV	= (1<<1),
	MBI_CMDLINE	= (1<<2),
	MBI_MODS	= (1<<3),
	MBI_SYMBOLS	= (1<<4),
	MBI_ELF_HEAD	= (1<<5),
	MBI_MEMMAP	= (1<<6),
	MBI_DRIVES	= (1<<7),
	MBI_CONF_TAB	= (1<<8),
	MBI_BOOTLD_NAME	= (1<<9),
	MBI_APM_TABLE	= (1<<10),
	MBI_VBE_TABLE	= (1<<11),
	MBI_FB_TABLE	= (1<<12),
};

struct multiboot_info {
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;
	uint32_t syms[4];
	uint32_t mmap_length;
	uint32_t mmap_addr;
	uint32_t drives_length;
	uint32_t drives_addr;
	uint32_t config_table;
	uint32_t bootloader_name;
	uint32_t apm_table;
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint32_t vbe_mode;   
	uint32_t vbe_interface_seg;
	uint32_t vbe_interface_off;
	uint32_t vbe_interface_len;
	uint32_t framebuffer_addr;
	uint32_t framebuffer_pitch;
	uint32_t framebuffer_width;
	uint32_t framebuffer_height;
	uint32_t framebuffer_bpp;
	uint32_t framebuffer_type;
	uint32_t color_info;
} __packed;

struct multiboot_module {
	uint32_t start;
	uint32_t end;
	uint32_t cmdline;
	uint32_t __reserved;
} __packed;

struct multiboot_memmap {
	uint32_t size;
	uint64_t base;
	uint64_t len;
	uint32_t type;
} __packed;

#endif
