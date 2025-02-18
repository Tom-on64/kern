#include <kernel.h>
#include <system.h>
#include <serial.h>

int serial_init(int dev) {
	outb(dev + 1, 0x00);	// Disable serial interrupts
	outb(dev + 3, 0x80);	// Enable DLAB (set baud rate divisor)
	outb(dev + 0, 0x03);	// Set divisor to 3 (low byte) 38400 baud
	outb(dev + 1, 0x00);	//                  (high byte)
	outb(dev + 3, 0x03);	// 8 bits, no parity, one stop bit
	outb(dev + 2, 0xC7);	// Enable FIFO, clear them, with 14 byte threshold
	outb(dev + 4, 0x0B);	// IRQs enabled, RTS/DSR set
	outb(dev + 4, 0x1E);	// Set in loopback mode, test the serial chip
	outb(dev + 0, 0xAE);	// Test serial chip (send byte 0xAE and check if it returns)
	
	if (inb(dev + 0) != 0xAE) return 1;
	outb(dev + 4, 0x0F);	// non-loopback, IRQs enabled, OUT#1 and OUT#2 bits enabled
	
	return 0;
}

int serial_txem(int dev) { 
	return inb(dev + 5) & 0x20;
}

void serial_send(int dev, char data) {
	while (serial_txem(dev) == 0);
	outb(dev, data);
}

int serial_rcvd(int dev) {
	return inb(dev + 5) & 1;
}

char serial_recv(int dev) {
	while (serial_rcvd(dev) == 0);
	return inb(dev);
}

void serial_write(int dev, char* buf, size_t len) {
	for (size_t i = 0; i < len; i++) {
		if (buf[i] == '\n') serial_send(dev, '\r');
		serial_send(dev, buf[i]);
	}
}

void serial_print(int dev, char* s) {
	size_t len = 0;
	char* p = s;
	while (*p++ != '\0') len++;
	serial_write(dev, s, len);
}

