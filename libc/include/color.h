#ifndef COLOR_H
#define COLOR_H

#define BLACK   0x00000000 // 0
#define RED     0x00ff0000 // 1
#define GREEN   0x0000ff00 // 2
#define YELLOW  0x00ffff00 // 3
#define BLUE    0x000000ff // 4
#define MAGENTA 0x00ff00ff // 5
#define CYAN    0x0000ffff // 6
#define WHITE   0x00ffffff // 7

uint32_t codeToColor(uint8_t code);

#endif
