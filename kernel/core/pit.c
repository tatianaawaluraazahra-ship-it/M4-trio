#include <pit.h>
#include <io.h>

volatile uint64_t g_ticks = 0;

void pit_configure_hz(uint32_t hz) {
    uint32_t divisor = 1193182 / hz;
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0_DATA, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0_DATA, (uint8_t)((divisor >> 8) & 0xFF));
}

void timer_on_irq0(void) {
    g_ticks++;
    // Log dinonaktifkan langsung agar tidak memicu undefined symbol
}
