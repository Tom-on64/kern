#include "screen.h"
#include "stdint.h"
#include "system.h"

uint16_t cursor = 0;

void updateCursor() {
    outb(0x3d4, 0x0f); // Low byte
    outb(0x3d5, cursor & 0xff);
    outb(0x3d4, 0x0e); // High byte
    outb(0x3d5, cursor >> 8);
}


void putc(char c, uint8_t attr) {
    if (c == '\n') {
        uint16_t line = cursor - (cursor % COLS);
        setCursorPos(line + COLS);
        return;
    } else if (c == '\b') {
        cursor--;
        int mem = VIDMEM + cursor * 2;
        *(char*)(mem) = ' ';
        *(char*)(mem+1) = attr;
        updateCursor();
        return;
    }
    
    int mem = VIDMEM + cursor * 2;
    *(char*)(mem) = c;
    *(char*)(mem+1) = attr;

    cursor++;

    updateCursor();
}

void print(char* s, uint8_t attr) { 
    char c = *s;
    while (c != '\0') {
        putc(c, attr);
        c = *(++s);
    }

    updateCursor();
}

void putcAt(char c, uint8_t attr, uint8_t col, uint8_t row) {
    uint16_t offset = calcOffset(col, row);
    *(char*)(VIDMEM + offset * 2) = c;
    *(char*)(VIDMEM + offset * 2 + 1) = attr;
}

void printAt(char* s, uint8_t attr, uint8_t col, uint8_t row) {
    uint16_t offset = calcOffset(col, row);
    
    char c = *s;
    while (c != '\0') {
        *(char*)(VIDMEM + offset * 2) = c;
        *(char*)(VIDMEM + offset * 2 + 1) = attr;
        c = *(++s);
        offset++;
    }
}

void clear(uint8_t attr) {
    for (int i = 0; i < CHARS*2; i += 2) {
        *(char*)(VIDMEM + i) = ' ';
        *(char*)(VIDMEM + i + 1) = attr;
    }
    cursor = 0;

    updateCursor();
}


void printHex(uint8_t d, uint8_t attr) {
    char hexDigits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    uint8_t low = d & 0x0f;
    uint8_t high = (d & 0xf0) >> 4;

    putc(hexDigits[low], attr);
    putc(hexDigits[high], attr);

    updateCursor();
}

void setCursorPos(uint16_t offset) {
    cursor = offset;
    updateCursor();
}

uint16_t getCursorPos() {
    return cursor;
}

uint16_t calcOffset(uint8_t col, uint8_t row) {
    return row * COLS + col;
}

void enableCursor(uint8_t start, uint8_t end) {
    outb(0x3d4, 0x0a); // Sending cursor start
    outb(0x3d5, (inb(0x3d5) & 0xc0) | start);

    outb(0x3d4, 0x0b); // Sending cursor end
    outb(0x3d5, (inb(0x3d5) & 0xe0) | end);
}

void disableCursor() {
    outb(0x3d4, 0x0a);
    outb(0x3d4, 0x0b);
}

