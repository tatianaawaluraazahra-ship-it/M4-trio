#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>
#include <mcsos/arch/cpu.h>
#include <stdint.h>
#include <stddef.h>

#define MCSOS_NAME "MCSOS"
#define MCSOS_VERSION "v0.3.0"
#define MCSOS_MILESTONE "M3"

extern char __kernel_start[];
extern char __kernel_end[];

static inline uint64_t cpu_read_rflags(void) {
    uint64_t rflags;
    __asm__ volatile ("pushfq; pop %0" : "=rm"(rflags) : : "memory");
    return rflags;
}

static void m3_selftest(void) {
    KERNEL_ASSERT(__kernel_end > __kernel_start);
    KERNEL_ASSERT(sizeof(uintptr_t) == 8u);
    log_writeln("[M3] selftest: basic invariants passed");
}

void kmain(void) {
    log_init();
    
    log_write(MCSOS_NAME);
    log_write(" ");
    log_write(MCSOS_VERSION);
    log_write(" ");
    log_write(MCSOS_MILESTONE);
    log_writeln(" kernel entered");

    log_key_value_hex64("kernel_start", (uint64_t)(uintptr_t)__kernel_start);
    log_key_value_hex64("kernel_end", (uint64_t)(uintptr_t)__kernel_end);
    log_key_value_hex64("rflags", cpu_read_rflags());

    m3_selftest();

#ifdef MCSOS_M3_TRIGGER_PANIC
    KERNEL_PANIC("intentional M3 panic test", 0x4D43534F533033u);
#else
    log_writeln("[M3] panic path installed; intentional panic disabled");
    log_writeln("[M3] ready for QEMU smoke test and GDB audit");
#endif

    hang();
}
