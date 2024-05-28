#ifndef RTC_H
#define RTC_H

#include <interrupt/pic.h>
#include <interrupt/idt.h>
#include <screen/text.h>
#include <stdbool.h>
#include <string.h>

#define CMOS_ADDRESS 0x70
#define CMOS_DATA    0x71

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} __attribute__ ((packed)) datetime_t;

static datetime_t* datetime = (datetime_t*)DATETIME_LOC;

bool cmosUpdateProgress() {
    outb(CMOS_ADDRESS, 0x8a); // Read status reg A, disable NMI
    return inb(CMOS_DATA) & 0x80;
}

uint8_t getRtcRegister(uint8_t reg) {
    outb(CMOS_ADDRESS, reg | 0x80); // Disable NMI when sending
    return inb(CMOS_DATA);
}

void enableRtc() {
    uint8_t regB = getRtcRegister(0x0b);
    outb(CMOS_ADDRESS, 0x8b);
    outb(CMOS_DATA, regB | 0x40); // Set bit 6 - periodic int at 1024Hz
    
    getRtcRegister(0x0c); // Clear pending interrupts
}

void disableRtc() {
    __asm__ volatile ("cli");
    uint8_t regB = getRtcRegister(0x0b);
    outb(CMOS_ADDRESS, 0x8b);
    outb(CMOS_DATA, regB & 0xbf); // Clear bit 6
    __asm__ volatile ("sti");
}

__attribute__ ((interrupt)) void rtcHandler(intFrame_t* iframe) {
    datetime_t newDatetime, oldDatetime;
    static uint16_t rtcTicks = 0;
    
    __asm__ volatile ("cli");
    rtcTicks++;
    if (rtcTicks % 1024 == 0) { // 1 Second has passed
        while (cmosUpdateProgress()); // Wait
        
        newDatetime.second = getRtcRegister(0);
        newDatetime.minute = getRtcRegister(2);
        newDatetime.hour   = getRtcRegister(4);
        newDatetime.day    = getRtcRegister(7);
        newDatetime.month  = getRtcRegister(8);
        newDatetime.year   = getRtcRegister(9);

        do {
            oldDatetime = newDatetime;

            while (cmosUpdateProgress()); // Wait
            newDatetime.second = getRtcRegister(0);
            newDatetime.minute = getRtcRegister(2);
            newDatetime.hour   = getRtcRegister(4);
            newDatetime.day    = getRtcRegister(7);
            newDatetime.month  = getRtcRegister(8);
            newDatetime.year   = getRtcRegister(9);

        } while (
            newDatetime.second != oldDatetime.second ||
            newDatetime.minute != oldDatetime.minute ||
            newDatetime.hour   != oldDatetime.hour ||
            newDatetime.day    != oldDatetime.day ||
            newDatetime.month  != oldDatetime.month ||
            newDatetime.year   != oldDatetime.year
        );

        uint8_t regB = getRtcRegister(0x0b);
        if (!(regB & 0x04)) {
            newDatetime.second = (newDatetime.second & 0x0f) + ((newDatetime.second >> 4) * 10);
            newDatetime.minute = (newDatetime.minute & 0x0f) + ((newDatetime.minute >> 4) * 10);
            newDatetime.hour   = ((newDatetime.hour & 0x0F) + (((newDatetime.hour & 0x70) / 16) * 10)) | (newDatetime.hour & 0x80); // 12/24 Hour format bs
            newDatetime.day    = (newDatetime.day    & 0x0f) + ((newDatetime.day    >> 4) * 10);
            newDatetime.month  = (newDatetime.month  & 0x0f) + ((newDatetime.month  >> 4) * 10);
            newDatetime.year   = (newDatetime.year   & 0x0f) + ((newDatetime.year   >> 4) * 10);
        }

        // Convert 12 to 14 hour format
        if (!(regB & 0x02) && (newDatetime.hour & 0x80)) {
            newDatetime.hour = ((newDatetime.hour & 0x7f) + 12) % 24;
        }

        newDatetime.year += 2000; // Add current century
        
        //*datetime = newDatetime;

        datetime->second = newDatetime.second;
        datetime->minute = newDatetime.minute;
        datetime->hour   = newDatetime.hour;
        datetime->day    = newDatetime.day;
        datetime->month  = newDatetime.month;
        datetime->year   = newDatetime.year;

        if (true) { // TODO: Probably get rid of this idk
            char buf[20] = { 0 }; // "12/12 2012 12:12:12" - Max string 20 chars with null byte
            
            strcat(buf, itoa(datetime->day, 10));
            strcat(buf, "/");
            strcat(buf, itoa(datetime->month, 10));
            strcat(buf, " ");
            strcat(buf, itoa(datetime->year, 10));
            strcat(buf, " ");
            if (datetime->hour < 10) strcat(buf, "0");
            strcat(buf, itoa(datetime->hour, 10));
            strcat(buf, ":");
            if (datetime->minute < 10) strcat(buf, "0");
            strcat(buf, itoa(datetime->minute, 10));
            strcat(buf, ":");
            if (datetime->second < 10) strcat(buf, "0");
            strcat(buf, itoa(datetime->second, 10));

            // The clock is a little offset down (about 0.4 of a character) It does look nice though!
            uint8_t len = strlen(buf);
            uint32_t x = gfxMode->xRes - len - 1;
            char* str = &buf[0];
            for (uint8_t i = 0; i != len; i++, x++) {
                putcAt(str[i], x, 0);
            }
        }
    }

    getRtcRegister(0x0c);
    sendPicEOI(8); 
    
    __asm__ volatile ("sti");
}

void setupRtc() { // IRQ 8 - Real Time Clock
    idtSetGate(0x28, rtcHandler, INT_GATE_FLAGS);
    unsetIrqMask(8);
    enableRtc();
}

#endif
