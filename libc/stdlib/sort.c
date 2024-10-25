#include <stdlib.h>
#include <stddef.h>
#include <string.h>

void swap(void* a, void* b, size_t size) {
    char temp[size];
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

void* partition(void* base, size_t nitem, size_t size, int (*compar)(const void*, const void*)) {
    char* pivot = (char*)base + (nitem - 1) * size;
    char* i = (char*)base;

    for (char* j = (char*)base; j < pivot; j += size) {
        if (compar(j, pivot) < 0) {
            swap(i, j, size);
            i += size;
        }
    }

    swap(i, pivot, size);
    return i;
}

void qsort(void* base, size_t nitem, size_t size, int (*compar)(const void*, const void*)) {
    if (nitem <= 1) { return; }

    char* pivot = (char*)partition(base, nitem, size, compar);
    size_t lsize = (pivot - (char*)base) / size;
    size_t rsize = nitem - lsize - 1;

    qsort(base, lsize, size, compar);
    qsort(pivot + size, rsize, size, compar);
}

