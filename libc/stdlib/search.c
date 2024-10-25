#include <stdlib.h>
#include <stddef.h>

void* bsearch(const void* key, const void* base, size_t nitem, size_t size, int (*compar)(const void*, const void*)) {
    size_t left = 0;
    size_t right = nitem;

    while (left < right) {
        size_t mid = left + (right - left) / 2;
        void* midEl = (char*)base + mid * size;

        int cmp = compar(key, midEl);

        if (cmp < 0) {
            right = mid;
        } else if (cmp > 0) {
            left = mid + 1;
        } else {
            return midEl;
        }
    }

    return NULL;
}

