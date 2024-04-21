void main() {
    int wait = 0xfffffff;

    unsigned int *vidmem = *(unsigned int**)(0x5000 + 40);

    vidmem[8000] = 0x00ffffff;

    while (wait != 0) wait--;

    return;
}
