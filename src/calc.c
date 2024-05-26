#include <stdio.h>
#include <stdlib.h>

__attribute__ ((section("entry")))
void main() {
    print("Hello, World!\n");
    return;
}
