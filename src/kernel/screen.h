#ifndef SCREEN_H
#define SCREEN_H

#define VIDMEM 0xb8000

void putc(char c, char attr);
void print(char* s, char attr);

#endif
