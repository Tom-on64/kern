#ifndef _BITMAP_H
#define _BITMAP_H

#include <kernel.h>

#define BLOCK_SIZE	4096
#define BMAP_NOT_FOUND	((size_t)-1)

typedef struct bitmap_t {
	void* map;
	size_t blocks;	// Eqiv. to # of bits
	size_t bytes;	// ceil(blocks / 8)
	size_t used;	// # of bits set to 0
	int ready;
} bitmap_t;

void	bmap_set(bitmap_t* map, size_t bit, int val);
int	bmap_get(bitmap_t* map, size_t bit);
void	bmap_setarea(bitmap_t* map, size_t base, size_t len, int val);
void	bmap_setall(bitmap_t* map, int val);
size_t	bmap_findarea(bitmap_t* map, size_t len, int val);

#endif
