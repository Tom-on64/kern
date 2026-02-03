#include <bootloader.h>
#include <kernel.h>

#include <splash.h>

#define BITMAP_LEN 24
#define PX_SIZE 2

uint8_t bitmap[BITMAP_LEN] = {
	0xff, 0x04, 0x0a, 0x11, 0x00, 0x0e, 0x15, 0x15,
	0x09, 0x00, 0x1f, 0x08, 0x10, 0x10, 0x08, 0x00,
	0x1f, 0x08, 0x10, 0x10, 0x0f, 0x00, 0x01, 0x00,
};

static inline void putpx(uint32_t* fb, uint64_t pitch, uint64_t x, uint64_t y, uint32_t c) {
	for (uint64_t dx = 0; dx < PX_SIZE; dx++) {
		for (uint64_t dy = 0; dy < PX_SIZE; dy++) {
			fb[((y * PX_SIZE) + dy) * pitch + ((x * PX_SIZE) + dx)] = c;
		}
	}
}

void splash(void) {
	uint32_t* fb = bootloader.fb_entries[0]->address;
	uint64_t width = bootloader.fb_entries[0]->width;
	uint64_t height = bootloader.fb_entries[0]->height;
	uint64_t pitch = bootloader.fb_entries[0]->pitch / 4;

	uint64_t x = (width  - BITMAP_LEN * PX_SIZE) / 2 / PX_SIZE;
	uint64_t y = (height - 8          * PX_SIZE) / 2 / PX_SIZE;

	for (size_t dx = 0; dx < BITMAP_LEN; dx++) {
		for (uint8_t dy = 0; dy < 8; dy++) {
			if (bitmap[dx] & (1 << (7 - dy))) putpx(fb, pitch, x + dx, y + dy, 0xFFC6D6D6);
		}
	}
}

