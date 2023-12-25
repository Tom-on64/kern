%define ENDL 0x0A, 0x0D

db  "This is a very cool message!", ENDL, ENDL, \
    "Kern is a 32-bit (16-bit for now) OS based on the x86 cpu architecture."

times 512-($-$$) db 0
