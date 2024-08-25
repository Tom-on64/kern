#include <stdio.h>

__attribute__ ((section("entry")))
int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    printf("%s\n", argv[1]);

    return 0;
}
