#include <stdio.h>
#include <stdlib.h>

__attribute__ ((section("entry")))
void main() {
    print("Hello, World!\n");
    char in[32];
    read(in);
    print(in);
    return;
}
