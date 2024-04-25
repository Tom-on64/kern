#ifndef PHYSICAL_MEM_MANAGER_H
#define PHYSICAL_MEM_MANAGER_H

typedef struct smapEntry_s {
    uint64_t baseAddress;
    uint64_t length;
    uint32_t type;
    uint32_t acpiAttr;
} __attribute__ ((packed)) smapEntry_t;

void printPhysicalMemmap() {
    uint32_t entryCount = *(uint32_t*)0x8500;
    smapEntry_t* smapEntry = (smapEntry_t*)0x8504;

    for (uint32_t i = 0; i < entryCount; i++) {
        print("Region ");
        print(itoa(i, 10));
        print(" - base: 0x");
        print(itoa(smapEntry->baseAddress, 16));
        print(" length: 0x");
        print(itoa(smapEntry->length, 16));
        print(" type: ");
        print(itoa(smapEntry->type, 10));

        switch (smapEntry->type) {
            case 1: print(" (Available)"); break;
            case 2: print(" (Reserved)"); break;
            case 3: print(" (ACPI Reclaim)"); break;
            case 4: print(" (ACPI NVS)"); break;
            default: print(" (Reserved)"); break;
        }

        putc('\n');
        smapEntry++;
    }

    smapEntry--; // We incremented it after the last entry

    print("\nTotal memory: 0x");
    print(itoa(smapEntry->baseAddress + smapEntry->length - 1, 16));
    print(" Bytes\n");
}


#endif
