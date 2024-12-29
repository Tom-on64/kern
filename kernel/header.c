#include "multiboot.h"

#define FLAGS	(MBH_PAGE_ALIGN | MBH_GRAPH_INFO)

__attribute__ ((section(".multiboot")))
struct multiboot_header mbh = {
	.magic = HEAD_MAGIC,
	.flags = FLAGS,
	.checksum = -(HEAD_MAGIC + FLAGS),
	// We don't need the a.out format fields
	.mode_type = 0,
	.width = 1920,
	.height = 1080,
	.depth = 32,
};
