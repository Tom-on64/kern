#ifndef SCREEN_H
#define SCREEN_H

#include "stdint.h"

#define VIDMEM 0xb8000
#define CURSOR_HIGH 14
#define CURSOR_LOW 15
#define CTRLREG 0x3d4
#define DATAREG 0x3d5

// 80x25 VGA Text mode
#define ROWS 25
#define COLS 80
#define CHARS (ROWS*COLS)

#define CURSOR '_'

void putc(char c, uint8_t attr);
void print(char* s, uint8_t attr);
void putcAt(char c, uint8_t attr, uint8_t row, uint8_t col);
void printAt(char* s, uint8_t attr, uint8_t row, uint8_t col);
void clear(uint8_t attr);

void setCursorPos(uint16_t offset);
uint16_t getCursorPos();
uint16_t calcOffset(uint8_t row, uint8_t col);

void setCursorChar(char cursor);
char getCursorChar();

#endif
