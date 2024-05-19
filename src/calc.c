#include <time.h>
#include <stdio.h>

__attribute__ ((section ("entry")))
void main() {
    char* msg = read();
    print(msg);
    
    sleep(4500);

    return;
}
