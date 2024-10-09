#include <ports/serial.h>
#include <ports/io.h>
#include <stdint.h>

int setupSerial(uint32_t port) {
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

int serialTransmitEmpty(uint32_t port) {
    return inb(port + 5) & 0x20;
}

int serialRecieved(uint32_t port) {
    return inb(port + 5) & 0x01;
}

void serialWrite(uint32_t port, char c) {
    while (serialTransmitEmpty(port) == 0);
    outb(port, c);
}

void serialPrint(uint32_t port, char* s) {
    while (*s != '\0') {
        if (*s == '\n') serialWrite(port, '\r');
        serialWrite(port, *s++);
    }
}

char serialRead(uint32_t port) {
    while (serialRecieved(port));
    return inb(port);
}

