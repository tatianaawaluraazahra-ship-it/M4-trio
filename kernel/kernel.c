#include "mcsos/kmem.h"

extern void serial_init(void);
extern void trap_init(void);
extern void timer_init(void);
extern void pmm_init_from_boot_memory_map(void);
extern void vmm_init_minimal(void);
extern void arch_hlt(void);

extern uint64_t pmm_alloc_frame(void);
extern void pmm_free_frame(uint64_t paddr);
extern int vmm_map_page(void *space, uint64_t vaddr, uint64_t paddr, uint64_t flags);
extern void *get_kernel_space(void);

extern void klog_info(const char *fmt, ...);
extern void kernel_panic(const char *msg);

#define KHEAP_BASE 0xffffffff90000000ull
#define KHEAP_SIZE (256ull * 1024ull)

#define VMM_PRESENT  (1ull << 0)
#define VMM_WRITABLE (1ull << 1)
#define VMM_NO_USER  (1ull << 2)

int kheaphys_map_initial_pages(void) {
    uint64_t va_start = KHEAP_BASE;
    uint64_t va_end = KHEAP_BASE + KHEAP_SIZE;
    void *kernel_space = get_kernel_space();

    for (uint64_t va = va_start; va < va_end; va += 4096) {
        uint64_t pa = pmm_alloc_frame();
        if (pa == 0) {
            for (uint64_t rollback_va = va_start; rollback_va < va; rollback_va += 4096) {
                klog_info("Rollback frame at VA: %llx", rollback_va);
            }
            return -1;
        }

        int rc = vmm_map_page(kernel_space, va, pa, VMM_PRESENT | VMM_WRITABLE | VMM_NO_USER);
        if (rc != 0) {
            pmm_free_frame(pa);
            for (uint64_t rollback_va = va_start; rollback_va < va; rollback_va += 4096) {
                klog_info("Rollback frame at VA: %llx", rollback_va);
            }
            return -2;
        }
    }

    return kmem_init((void *)KHEAP_BASE, KHEAP_SIZE);
}

void kernel_main(void) {
    serial_init();
    klog_info("MCSOS entering kernel_main");

    trap_init();
    timer_init();
    pmm_init_from_boot_memory_map();
    vmm_init_minimal();
    
    int rc = kheaphys_map_initial_pages();
    if (rc != 0) {
        kernel_panic("M8 page-backed heap initialization failed");
    }

    klog_info("M8 pengayaan heap checkpoint reached");
    for (;;) {
        arch_hlt();
    }
}
