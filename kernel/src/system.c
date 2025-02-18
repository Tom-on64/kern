#include <kernel.h>

void cpuid(uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
	__asm__ volatile ("cpuid"
		: "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
		: "a"(*eax)
		: "memory");
}

uint8_t inb(uint16_t port) {
	uint8_t r;
	__asm__ volatile ("inb %1, %0" : "=a"(r) : "dN"(port));
	return r;
}

void outb(uint16_t port, uint8_t data) {
	__asm__ volatile ("outb %1, %0" :: "dN"(port), "a"(data));
}

uint16_t inw(uint16_t port) {
	uint16_t r;
	__asm__ volatile ("inw %1, %0" : "=a"(r) : "dN"(port));
	return r;
}

void outw(uint16_t port, uint16_t data) {
	__asm__ volatile ("outw %1, %0" :: "dN"(port), "a"(data));
}

uint32_t inl(uint16_t port) {
	uint32_t r;
	__asm__ volatile ("inl %1, %0" : "=a"(r) : "dN"(port));
	return r;
}

void outl(uint16_t port, uint32_t data) {
	__asm__ volatile ("outl %1, %0" :: "dN"(port), "a"(data));
}

