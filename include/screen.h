#ifndef SCREEN_H
#define SCREEN_H

#include "screen.h"
#include "stdint.h"
#include "system.h"
#include "string.h"

#define VIDMEM 0xb8000
#define CURSOR_HIGH 14
#define CURSOR_LOW 15
#define CTRLREG 0x3d4
#define DATAREG 0x3d5

// 80x25 VGA Text mode
#define ROWS 25
#define COLS 80
#define CHARS (ROWS*COLS)

uint32_t cursor = 0;

void updateCursor() {
    // Move visual cursor position
    outb(0x3d4, 0x0f); // Low byte
    outb(0x3d5, cursor & 0xff);
    outb(0x3d4, 0x0e); // High byte
    outb(0x3d5, cursor >> 8);
}

void setCursorPos(uint32_t offset) {
    cursor = offset;
    updateCursor();
}

uint32_t getCursorPos() {
    return cursor;
}

uint32_t calcOffset(uint8_t col, uint8_t row) {
    return row * COLS + col;
}

void scroll() {
    if (cursor < COLS * ROWS) return;

    for (int i = 1; i < ROWS; i++) {
        memcopy((char*)(calcOffset(0, i)*2)+VIDMEM, (char*)(calcOffset(0, i-1)*2)+VIDMEM, COLS*2);
    }

    char* lastRow = (char*)(calcOffset(0, ROWS-1)*2)+VIDMEM;
    for (int i = 0; i < COLS*2; i += 2) {
        lastRow[i] = '\0';
        lastRow[i+1] = 0x0f;
    }

    cursor = calcOffset(0, ROWS-1);
}

void putc(char c, uint8_t attr) {
    if (c == '\n') {
        uint16_t line = cursor - (cursor % COLS);
        setCursorPos(line + COLS);
        scroll();
        updateCursor();
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

    scroll();
    updateCursor();
}

void print(char* s, uint8_t attr) { 
    char c = *s;
    while (c != '\0') {
        putc(c, attr);
        c = *(++s);
    }

    scroll();
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


#endif
