#ifndef MCSOS_ARCH_CPU_H
#define MCSOS_ARCH_CPU_H
static inline void halt_cpu(void) {
    __asm__ __volatile__("cli; hlt");
}
#endif
