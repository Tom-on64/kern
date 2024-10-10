#ifndef SOUND_PCSPK_H
#define SOUND_PCSPK_H

#include <sound/notes.h>
#include <stdint.h>

#define PC_SPEAKER 0x61

void enableSpeaker();
void disableSpeaker();
void playNote(note_t note, uint32_t ms);
void rest(uint32_t ms);

#endif
