#ifndef _DRIVERS_RTC_H
#define _DRIVERS_RTC_H

#include <cpu/isr.h>
#include <stdint.h>

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

void rtcHandler(intFrame_t*);
int cmosUpdateProgress();
uint8_t getRtcRegister(uint8_t reg);
void enableRtc();
void disableRtc();

#endif
