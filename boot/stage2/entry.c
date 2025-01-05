#include "stdint.h"
#include "stdio.h"
#include "attr.h"
#include "disk.h"
#include "menu.h"
#include "kbd.h"

void drawPartitions(struct MBR* mbr, uint8_t sel);
void partitonInfo(struct partition* part);

__section("entry")
noreturn void main() {
	keyboardInit();

	struct MBR mbr;
	readSectors(0, 1, &mbr);

	// Select bootable partition by default
	uint8_t selected = 0;
	for (size_t i = 0; i < 4; i++) {
		if (mbr.partitions[i].bootable != BOOTABLE) continue;
		selected = i;
		break;
	}
	
	// Initial draw
	setAttr(0x0f);
	cls();
	hideCursor();
	rect(0, 0, COLS, ROWS);
	hline(0, 2, COLS);
	setAttr(0x02);
	text(COLS / 2, 1, "KBOOT");
	drawPartitions(&mbr, selected);
	partitonInfo(&mbr.partitions[selected]);

	while (1) {
		// Check for up/down arrow keys or enter
		uint8_t scancode = getScancode();
		if (scancode == SC_ENTER) { // Boot selected
			if (mbr.partitions[selected].type != 0) break;

			// Don't allow booting an unused partition
			setAttr(0x4f);
			text(COLS / 2, ROWS / 2 + 6, "Unused Partition");
		} else if (scancode != SC_EXTENDED) continue; // Not an arrow key
		
		scancode = getScancode();
		if (scancode == SCE_ARROW_UP) selected--;
		else if (scancode == SCE_ARROW_DOWN) selected++;
		else continue;

		if (selected == 0xff) selected = 3;
		else if (selected == 4) selected = 0;

		// Redraw
		drawPartitions(&mbr, selected);
		partitonInfo(&mbr.partitions[selected]);
	}

	setAttr(0x0f);
	cls();
	showCursor();
	printf("Booting from Partition %d...", selected);

	while (1);
}

void drawPartitions(struct MBR* mbr, uint8_t sel) {
	uint32_t basey = ROWS / 2 - 2;
	for (size_t i = 0; i < 4; i++) {
		if (i == sel) setAttr(0xf0);
		else if (mbr->partitions[i].type == 0) setAttr(0x04);
		else setAttr(0x0f);

		text(COLS / 2, basey + i, " [ Partition %d ] ", i+1);
	}
}

void partitonInfo(struct partition* part) {
	uint32_t basey = ROWS / 2 + 6;
	setAttr(0x02);

	// Clear previous text
	fill(1, basey, COLS - 2, 3, '\0');

	if (part->type == 0) {
		text(COLS / 2, basey, "Unused Partition");
		return;
	}

	text(COLS / 2, basey, "Type: %02x%s", part->type, 
		(part->bootable == BOOTABLE) ? "  Bootable" : "");
	text(COLS / 2, basey + 1, "Start LBA: %d; End LBA: %d", 
		part->startLBA, part->startLBA + part->size);
	text(COLS / 2, basey + 2, "Length: 0x%x", part->size);
}

