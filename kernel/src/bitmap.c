#include <kernel.h>
#include <string.h>

#include <bitmap.h>

void* bmap_ptr(bitmap_t* map, size_t bit) {
	return (void*)(map->membase + bit * BLOCK_SIZE);
}

void bmap_set(bitmap_t* map, size_t bit, int val) {
	uint32_t* p = (uint32_t*)map->map;
	if (val) p[bit / 32] |= (1 << (bit % 32));
	else p[bit / 32] &= ~(1 << (bit % 32));
	map->used += (val) ? 1 : -1;
}

int bmap_get(bitmap_t* map, size_t bit) {
	uint32_t* p = (uint32_t*)map->map;
	return (p[bit / 32] & (1 << (bit % 32))) ? 1 : 0; 
}

void bmap_set_area(bitmap_t* map, size_t base, size_t len, int val) {
	uint32_t align = dceil(base, BLOCK_SIZE);
	uint32_t count = dceil(len, BLOCK_SIZE);

	while (count-- > 0) bmap_set(map, align++, val);
}

size_t bmap_get_area(bitmap_t* map, size_t len, int val) {
	if (len == 0) return BMAP_NOT_FOUND;

	uint32_t found = 0;
	for (size_t i = 0; i < map->blocks; i++) {
		if (bmap_get(map, i) == val) found++;
		else found = 0;

		if (found >= len) return i - found + 1;
	}

	return BMAP_NOT_FOUND;
}

void bmap_set_all(bitmap_t* map, int val) {
	uint8_t b = (val) ? 0xff : 0;
	for (size_t i = 0; i < map->bytes; i++) ((uint8_t*)map->map)[i] = b;
	if (val) map->used = map->blocks;
	else map->used = 0;
}

