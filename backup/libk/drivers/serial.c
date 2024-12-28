#include <drivers/serial.h>
#include <cpu/system.h>
#include <stdio.h>
#include <stdarg.h>

int serialEnable(int port) {
    outb(port + 1, 0x00);   // Disable all interrupts
    outb(port + 3, 0x80);   // Enable DLAB (set baud rate divisor)
    outb(port + 0, 0x03);   // Set divisor to 3 (low byte) 38400 baud
    outb(port + 1, 0x00);   //                  (high byte)
    outb(port + 3, 0x03);   // 8 bits, no parity, one stop bit
    outb(port + 2, 0xC7);   // Enable FIFO, clear them, with 14 byte threshold
    outb(port + 4, 0x0B);   // IRQs enabled, RTS/DSR set
    outb(port + 4, 0x1E);   // Set in loopback mode, test the serial chip
    outb(port + 0, 0xAE);   // Test serial chip (send byte 0xAE and check if it returns)

    if (inb(port + 0) != 0xAE) return 1;
    outb(port + 4, 0x0F);   // non-loopback, IRQs enabled, OUT#1 and OUT#2 bits enabled

    return 0;
}

int setupSerial() {
    if (serialEnable(PORT_COM1) != 0) { return 1; }
    outb(PORT_COM1 + 1, 0x01);

    return 0;
}

static int serialTransimitEmpty(int port) { return inb(port + 5) & 0x20; }
static int serialRecieved(int port) { return inb(port + 5) & 0x01; }

void serialWrite(int port, char c) { while (!serialTransimitEmpty(port)); outb(port, c); }
char serialRead(int port) { while (!serialRecieved(port)); return inb(port); }
char serialReadAsync(int port) { return inb(port); }

void _dbgPutc(char c, void* arg) {
    (void)arg;
    serialWrite(PORT_COM1, c);
    if (c == '\n') serialWrite(PORT_COM1, '\r');
}
int vdebugf(const char* fmt, va_list va) { return vpprintf(_dbgPutc, NULL, fmt, va); }
int debugf(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    int ret = vdebugf(fmt, va);
    va_end(va);
    return ret;
}

