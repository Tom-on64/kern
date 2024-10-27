#include <drivers/pcspk.h>
#include <drivers/timer.h>
#include <cpu/system.h>
#include <sys/notes.h>
#include <stdint.h>
#include <time.h>

void enableSpeaker() {
    uint8_t temp = inb(PC_SPEAKER);
    outb(PC_SPEAKER, temp | 0x03); // Set first 2 bits
}

void disableSpeaker() {
    uint8_t temp = inb(PC_SPEAKER);
    outb(PC_SPEAKER, temp & 0xfc); // Set first 2 bits
}

void playNote(note_t note, uint32_t ms) {
    setPitPhase(2, 3, note);
    sleep(ms);
}

void rest(uint32_t ms) {
    setPitPhase(2, 3, 30000); // Unhearable noise
    sleep(ms);
}

