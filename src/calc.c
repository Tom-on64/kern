#include <stdio.h>

__attribute__ ((section ("entry")))
void main() {
    char msg[8] = { 'H','e','l','l','o','!','\n',0 };
    print(msg);

    return;
}
