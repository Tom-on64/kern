#ifndef _DRIVERS_PCSPK_H
#define _DRIVERS_PCSPK_H

#include <sys/notes.h>
#include <stdint.h>

#define PC_SPEAKER 0x61

void enableSpeaker();
void disableSpeaker();
void playNote(note_t note, uint32_t ms);
void rest(uint32_t ms);

#endif
