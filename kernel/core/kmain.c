#include <stdint.h>
void kmain(void) {
    volatile uint8_t *port = (volatile uint8_t *)0x3f8;
    const char *str = "MCSOS M2: Trio Group Booted!\r\n";
    while (*str) {
        while (!(*(port + 5) & 0x20));
        *port = *str++;
    }
    for (;;) { __asm__ volatile ("hlt"); }
}
