#ifndef MCSOS_ARCH_CPU_H
#define MCSOS_ARCH_CPU_H

static inline void hlt(void) {
    __asm__ volatile ("hlt");
}

static inline void cli(void) {
    __asm__ volatile ("cli");
}

static inline void hang(void) {
    cli();
    for (;;) {
        hlt();
    }
}

#endif
