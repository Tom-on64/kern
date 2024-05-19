#ifndef PCSPK_H
#define PCSPK_H

#include <stdint.h>
#include <sound/notes.h>
#include <interrupt/pit.h>
#include <ports/io.h>
#include <time.h>

#define PC_SPEAKER 0x61

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

#endif
