#include "mcsos_thread.h"
#include "mcsos/syscall.h"

// Fungsi cetak log bawaan kernel lama kamu
void klog(const char *fmt);

static mcsos_scheduler_t g_sched;
static mcsos_thread_t g_boot_thread;
static mcsos_thread_t g_thread_a;
static mcsos_thread_t g_thread_b;
static unsigned char g_stack_a[8192] __attribute__((aligned(16)));
static unsigned char g_stack_b[8192] __attribute__((aligned(16)));

// === [M10] Fungsi Callback Subsystem Pendukung ===
static uint64_t k_get_ticks(void) {
    return 42u; 
}

static void k_yield_current(void) {
    mcsos_sched_yield(&g_sched);
}

static void k_exit_current(int code) {
    (void)code;
    klog("[M10] thread requesting exit via syscall\n");
}

static int64_t k_write_serial(const char *buf, size_t len) {
    (void)len;
    klog(buf);
    return (int64_t)len;
}

// === [M10] IMPLEMENTASI LANGKAH 10: SMOKE TEST SYSCALL ===
void m10_syscall_smoke_direct(void) {
    // 1. Tes pemanggilan dispatcher C secara langsung
    int64_t r = mcsos_syscall_dispatch(MCSOS_SYS_PING, 0, 0, 0, 0, 0, 0);
    if (r != 0x2605020AL) {
        klog("CRITICAL ERROR: M10 syscall ping failed\n");
        for (;;);
    }
    klog("[M10] syscall ping direct ok\n");
}

static inline long m10_int80_ping_kernel_only(void) {
    long ret;
    // 2. Memicu gerbang IDT 0x80 secara nyata menggunakan Inline Assembly
    __asm__ volatile (
        "movq $0, %%rax\n\t"  // MCSOS_SYS_PING = 0 dimasukkan ke register rax
        "int $0x80\n\t"       // Picu interupsi gerbang syscall
        : "=a"(ret)
        :
        : "rcx", "r11", "memory"
    );
    return ret;
}

static void demo_thread_a(void *arg) {
    (void)arg;
    for (;;) {
        klog("[M9] thread A tick\n");
        mcsos_sched_yield(&g_sched);
    }
}

static void demo_thread_b(void *arg) {
    (void)arg;
    for (;;) {
        klog("[M9] thread B tick\n");
        mcsos_sched_yield(&g_sched);
    }
}

void kernel_main(void) {
    // Inisialisasi M9 Layer Scheduler
    mcsos_scheduler_init(&g_sched, &g_boot_thread);
    mcsos_thread_prepare(&g_thread_a, "demo-a", demo_thread_a, 0, g_stack_a, sizeof(g_stack_a), g_sched.next_id++);
    mcsos_thread_prepare(&g_thread_b, "demo-b", demo_thread_b, 0, g_stack_b, sizeof(g_stack_b), g_sched.next_id++);

    mcsos_sched_enqueue(&g_sched, &g_thread_a);
    mcsos_sched_enqueue(&g_sched, &g_thread_b);

    klog("[M9] scheduler initialized\n");

    // === [M10] REGISTRASI OPERASI SYSTEM CALL ===
    mcsos_syscall_ops_t ops = {
        .get_ticks = k_get_ticks,
        .yield_current = k_yield_current,
        .exit_current = k_exit_current,
        .write_serial = k_write_serial,
    };
    mcsos_syscall_init(&ops);
    klog("[M10] syscall dispatcher registered to kernel init\n");

    // === JALANKAN SMOKE TEST LANGKAH 10 ===
    m10_syscall_smoke_direct();
    
    klog("[M10] triggering int 0x80 gate test...\n");
    long asm_res = m10_int80_ping_kernel_only();
    if (asm_res == 0x2605020AL) {
        klog("[M10] int 0x80 assembly jump path SUCCESS!\n");
    } else {
        klog("[M10] WARNING: assembly gate returned wrong token value\n");
    }

    // Picu perpindahan kooperatif pertama
    mcsos_sched_yield(&g_sched);

    for (;;) {
        __asm__ volatile("hlt");
    }
}
