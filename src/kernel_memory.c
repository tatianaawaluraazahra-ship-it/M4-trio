#include <types.h>
#include <pmm.h>

extern void serial_print(const char* str);
extern void kernel_panic_at(const char* msg, const char* file, int line);

static struct pmm_state kernel_pmm;
static uint8_t kernel_pmm_bitmap[PMM_BITMAP_BYTES] __attribute__((aligned(4096)));

void kernel_memory_init(void *regions, size_t region_count) {
    serial_print("[m6] Memulai inisialisasi Physical Memory Manager...\n");

    bool ok = pmm_init_from_map(&kernel_pmm,
                                (const struct boot_mem_region*)regions,
                                region_count,
                                kernel_pmm_bitmap,
                                sizeof(kernel_pmm_bitmap),
                                PMM_MAX_PHYS_BYTES);
    if (!ok) {
        kernel_panic_at("pmm_init_from_map failed", __FILE__, __LINE__);
    }
    serial_print("[m6] PMM bitmap frame allocator initialized successfully.\n");

    uint64_t f = pmm_alloc_frame(&kernel_pmm);
    if (f == PMM_INVALID_FRAME) {
        kernel_panic_at("pmm_alloc_frame returned invalid", __FILE__, __LINE__);
    }
    serial_print("[m6] Alokasi frame uji coba berhasil.\n");

    if (!pmm_free_frame(&kernel_pmm, f)) {
        kernel_panic_at("pmm_free_frame failed", __FILE__, __LINE__);
    }
    serial_print("[m6] Pelepasan frame uji coba berhasil dikembalikan.\n");
    serial_print("[m6] INTEGRASI MODUL M6 SUKSES TOTAL!\n");
}
