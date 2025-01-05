#ifndef _MENU_H
#define _MENU_H

// Box drawing characters
#define SHADE_LIGHT	0xB0
#define SHADE_MEDIUM	0xB1
#define SHADE_DARK	0xB2
#define LINE_V		0xB3
#define LINE_T_LEFT	0xB4
#define CORNER_TR	0xBF
#define CORNER_BL	0xC0
#define LINE_T_UP	0xC1
#define LINE_T_DOWN	0xC2
#define LINE_T_RIGHT	0xC3
#define LINE_H		0xC4
#define LINE_CROSS	0xC5
#define CORNER_BR	0xD9
#define CORNER_TL	0xDA
#define BLOCK		0xDB
#define BLOCK_UPPER	0xDC
#define BLOCK_LEFT	0xDD
#define BLOCK_RIGHT	0xDE
#define BLOCK_LOWER	0xDF
#define INT_UPPER	0xF4
#define INT_LOWER	0xF5

void rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void fill(uint32_t x, uint32_t y, uint32_t w, uint32_t h, char c);
void hline(uint32_t x, uint32_t y, uint32_t len);
void vline(uint32_t x, uint32_t y, uint32_t len);
void text(uint32_t x, uint32_t y, char* fmt, ...);

#endif
