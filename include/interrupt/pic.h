#ifndef PIC_H
#define PIC_H

#include <ports/io.h>

// PIC I/O Ports
#define PIC_1_CMD  0x20
#define PIC_1_DATA 0x21
#define PIC_2_CMD  0xA0
#define PIC_2_DATA 0xA1

// IRQs
#define NEW_IRQ_0 0x20 // IRQ 0 : int 32
#define NEW_IRQ_8 0x28 // IRQ 8 : int 40

// Commands
#define PIC_EOI 0x20 // End Of Interrupt
#define PIC_DISABLE 0xff

void sendPicEOI(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC_2_CMD, PIC_EOI);
    }
    outb(PIC_1_CMD, PIC_EOI);
}

void disableIrqs() {
    outb(PIC_2_DATA, PIC_DISABLE);
    outb(PIC_1_DATA, PIC_DISABLE);
}

void setIrqMask(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC_1_DATA;
    } else {
        irq -= 8;
        port = PIC_2_DATA;
    }

    value = inb(port) | (1 << irq);
    outb(port, value);
}

void unsetIrqMask(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC_1_DATA;
    } else {
        irq -= 8;
        port = PIC_2_DATA;
    }

    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

void remapPic() {
    uint8_t pic1Mask = inb(PIC_1_DATA);
    uint8_t pic2Mask = inb(PIC_2_DATA);

    // Send Init Control Word 1-4 to both PICs
    // ICW 1: bit 0 - send up to 4 ICWs, bit 4 - Init PIC
    outb(PIC_1_CMD, 0x11);
    ioWait(); // For slower machines
    outb(PIC_2_CMD, 0x11);
    ioWait();

    // ICW 2: Where to map IRW bases in IDT
    outb(PIC_1_DATA, NEW_IRQ_0);
    ioWait();
    outb(PIC_2_DATA, NEW_IRQ_8);
    ioWait();

    // ICW 3: Which IRQ in PIC1 is PIC2; Mapping PIC2 to IRQ 2
    outb(PIC_1_DATA, 0x04);
    ioWait();
    outb(PIC_2_DATA, 0x02);
    ioWait();

    // ICW 4 - Set x86 mode
    outb(PIC_1_DATA, 0x01);
    ioWait();
    outb(PIC_2_DATA, 0x01);
    ioWait();

    outb(PIC_1_DATA, pic1Mask);
    outb(PIC_2_DATA, pic2Mask);
}

#endif
