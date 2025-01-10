#include "stdint.h"
#include "string.h"
#include "disk.h"
#include "fat32.h"

// Local context
struct fat32_ctx ctx;

int fat32_init(struct partition* p) {
	readSectors(p->startLBA, 1, &ctx.bpb);
	ctx.offsetBase = p->startLBA;

	if (ctx.bpb.bytesPerSect != SECTOR_SIZE) return 1;

	ctx.offsetFats = ctx.offsetBase + ctx.bpb.reservedSects;
	ctx.offsetClusts = ctx.offsetFats + ctx.bpb.fatCount * ctx.bpb.sectsPerFat_32;

	return 0;
}

int fat32_open(char* filename, int flags, ...) {
	// TODO: flags
	
	//struct fat32_traversed res = fat32_traverse(ctx, filename);
	//if (!res.dir) return -1;


}

void fat32_close(void* file) {}

int fat32_read(const void* buf, size_t size, size_t count, void* file) {}

