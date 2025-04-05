#include <kernel.h>
#include <string.h>

#include <bitmap.h>

void bmap_set(bitmap_t* map, size_t bit, int val) {
	uint32_t* p = (uint32_t*)map->map;
	if (val) p[bit / 32] |= (1 << (bit % 32));
	else p[bit / 32] &= ~(1 << (bit % 32));
}

int bmap_get(bitmap_t* map, size_t bit) {
	uint32_t* p = (uint32_t*)map->map;
	return (p[bit / 32] & (1 << (bit % 32))) ? 1 : 0; 
}

void bmap_setarea(bitmap_t* map, size_t base, size_t len, int val) {
	uint32_t align = base / BLOCK_SIZE;
	uint32_t count = len / BLOCK_SIZE;

	while (count-- > 0) {
		bmap_set(map, align++, val);
		map->used += (val) ? 1 : -1;
	}
}

void bmap_setall(bitmap_t* map, int val) {
	uint8_t b = (val) ? 0xff : 0;
	memset(map->map, b, map->bytes);
	if (val) map->used = map->blocks;
	else map->used = 0;
}

size_t bmap_findarea(bitmap_t* map, size_t len, int val) {
	if (len == 0) return BMAP_NOT_FOUND;

	uint32_t found = 0;
	for (size_t i = 0; i < map->blocks; i++) {
		if (bmap_get(map, i) == val) found++;
		else found = 0;

		if (found >= len) return i - found;
	}

	return BMAP_NOT_FOUND;
}

