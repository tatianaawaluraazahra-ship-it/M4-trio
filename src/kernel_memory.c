#include <types.h>
#include <pmm.h>
#include <vmm.h>

extern void serial_print(const char* str);
extern void kernel_panic_at(const char* msg, const char* file, int line);

static struct vmm_space kernel_space;
static uint64_t static_hhdm_offset = 0xFFFF800000000000ULL;

static void local_memzero(void *ptr, size_t size) {
    uint8_t *p = (uint8_t *)ptr;
    for (size_t i = 0; i < size; i++) {
        p[i] = 0;
    }
}

static uint64_t kernel_vmm_alloc(void *ctx) {
    (void)ctx;
    return pmm_alloc_frame(get_kernel_pmm_state());
}

static void kernel_vmm_free(void *ctx, uint64_t frame_paddr) {
    (void)ctx;
    pmm_free_frame(get_kernel_pmm_state(), frame_paddr);
}

static void *kernel_phys_to_virt(void *ctx, uint64_t paddr) {
    uint64_t hhdm = *(uint64_t *)ctx;
    return (void *)(hhdm + paddr);
}

void kernel_memory_init(void *regions, size_t region_count) {
    serial_print("[m7] Inisialisasi awal struktur kernel_main...\n");

    bool pmm_ok = pmm_init_from_map(get_kernel_pmm_state(), (const struct boot_mem_region*)regions, region_count, NULL, 0, 0);
    if (!pmm_ok) {
        kernel_panic_at("M7: Gagal inisialisasi PMM dari boot map", __FILE__, __LINE__);
    }

    uint64_t root = pmm_alloc_frame(get_kernel_pmm_state());
    if (root == VMM_INVALID_PHYS) {
        kernel_panic_at("M7: cannot allocate root page table", __FILE__, __LINE__);
    }

    void *root_virt = kernel_phys_to_virt(&static_hhdm_offset, root);
    local_memzero(root_virt, VMM_PAGE_SIZE);

    int rc = vmm_space_init(&kernel_space, root, &static_hhdm_offset,
                            kernel_vmm_alloc, kernel_vmm_free, kernel_phys_to_virt);
    if (rc != VMM_MAP_OK) {
        kernel_panic_at("M7: vmm_space_init failed", __FILE__, __LINE__);
    }

    serial_print("[m7] VMM core initialized\n");
}
