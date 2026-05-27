#include "mcsos/kmem.h"

extern void *kernel_phys_to_virt(void *ctx, uint64_t paddr);
extern uint64_t pmm_alloc_frame(void *state);
extern void *get_kernel_pmm_state(void);
extern int vmm_map_page(void *space, uint64_t vaddr, uint64_t paddr, uint64_t flags);

struct kmem_header {
    uint64_t magic;
    size_t size;
    struct kmem_header *next;
    int is_free;
    uint8_t padding[8];
};

static void *g_heap_base = NULL;
static size_t g_heap_bytes = 0;
static int g_kmem_ready = 0;

static void *local_memset(void *ptr, int value, size_t num) {
    uint8_t *p = (uint8_t *)ptr;
    for (size_t i = 0; i < num; i++) {
        p[i] = (uint8_t)value;
    }
    return ptr;
}

#ifdef MCSOS_HOST_TEST
void kmem_reset_for_test(void) {
    g_heap_base = NULL;
    g_heap_bytes = 0;
    g_kmem_ready = 0;
}
#endif

int kmem_init(void *base, size_t bytes) {
    if (g_kmem_ready) return -1;
    if (base == NULL || bytes < sizeof(struct kmem_header) + KMEM_ALIGN) return -2;

    uintptr_t addr = (uintptr_t)base;
    uintptr_t header_size = sizeof(struct kmem_header);
    uintptr_t payload_addr = addr + header_size;
    uintptr_t aligned_payload = (payload_addr + (KMEM_ALIGN - 1)) & ~(uintptr_t)(KMEM_ALIGN - 1);
    uintptr_t final_header_addr = aligned_payload - header_size;

    size_t offset = final_header_addr - addr;
    if (bytes <= offset + header_size) return -2;

    g_heap_base = (void *)final_header_addr;
    g_heap_bytes = bytes - offset;

    struct kmem_header *first = (struct kmem_header *)g_heap_base;
    first->magic = KMEM_MAGIC;
    first->size = g_heap_bytes - header_size;
    first->next = NULL;
    first->is_free = 1;

    g_kmem_ready = 1;
    return 0;
}

void *kmem_alloc(size_t bytes) {
    if (!g_kmem_ready || bytes == 0) return NULL;

    size_t aligned_size = (bytes + (KMEM_ALIGN - 1)) & ~(KMEM_ALIGN - 1);
    struct kmem_header *curr = (struct kmem_header *)g_heap_base;
    size_t header_size = sizeof(struct kmem_header);

    while (curr != NULL) {
        if (curr->magic != KMEM_MAGIC) return NULL;

        if (curr->is_free && curr->size >= aligned_size) {
            size_t needed_space = aligned_size + header_size;
            needed_space = (needed_space + (KMEM_ALIGN - 1)) & ~(KMEM_ALIGN - 1);

            if (curr->size >= needed_space + header_size + KMEM_ALIGN) {
                struct kmem_header *next_block = (struct kmem_header *)((uintptr_t)curr + needed_space);
                next_block->magic = KMEM_MAGIC;
                next_block->size = curr->size - needed_space;
                next_block->next = curr->next;
                next_block->is_free = 1;

                curr->size = needed_space - header_size;
                curr->next = next_block;
            }
            curr->is_free = 0;
            return (void *)((uintptr_t)curr + header_size);
        }
        curr = curr->next;
    }
    return NULL;
}

void *kmem_calloc(size_t count, size_t bytes) {
    if (count == 0 || bytes == 0) return NULL;
    if (count > (size_t)-1 / bytes) return NULL;

    size_t total = count * bytes;
    void *ptr = kmem_alloc(total);
    if (ptr != NULL) {
        local_memset(ptr, 0, total);
    }
    return ptr;
}

int kmem_free_checked(void *ptr) {
    if (!g_kmem_ready) return -1;
    if (ptr == NULL) return 0;

    uintptr_t ptr_addr = (uintptr_t)ptr;
    uintptr_t base_addr = (uintptr_t)g_heap_base;
    if (ptr_addr <= base_addr || ptr_addr >= base_addr + g_heap_bytes) {
        return -3; 
    }

    struct kmem_header *block = (struct kmem_header *)(ptr_addr - sizeof(struct kmem_header));
    if (block->magic != KMEM_MAGIC) return -4;
    if (block->is_free) return -2;

    block->is_free = 1;

    struct kmem_header *curr = (struct kmem_header *)g_heap_base;
    while (curr != NULL) {
        if (curr->magic != KMEM_MAGIC) return -4;
        while (curr->is_free && curr->next != NULL && curr->next->is_free) {
            if (curr->next->magic != KMEM_MAGIC) return -4;
            curr->size += sizeof(struct kmem_header) + curr->next->size;
            curr->next = curr->next->next;
        }
        curr = curr->next;
    }
    return 0;
}

void kmem_get_stats(kmem_stats_t *out) {
    if (out == NULL) return;
    local_memset(out, 0, sizeof(kmem_stats_t));
    if (!g_kmem_ready) return;

    out->total_bytes = g_heap_bytes;
    struct kmem_header *curr = (struct kmem_header *)g_heap_base;

    while (curr != NULL) {
        if (curr->magic != KMEM_MAGIC) return;
        out->block_count++;
        if (curr->is_free) {
            out->free_count++;
            out->free_bytes += curr->size;
            if (curr->size > out->largest_free) {
                out->largest_free = curr->size;
            }
        } else {
            out->used_bytes += curr->size;
        }
        curr = curr->next;
    }
}

int kmem_validate(void) {
    if (!g_kmem_ready) return -1;
    struct kmem_header *curr = (struct kmem_header *)g_heap_base;
    uintptr_t base_addr = (uintptr_t)g_heap_base;

    while (curr != NULL) {
        uintptr_t curr_addr = (uintptr_t)curr;
        if (curr_addr < base_addr || curr_addr >= base_addr + g_heap_bytes) {
            return -2;
        }
        if (curr->magic != KMEM_MAGIC) {
            return -3;
        }
        curr = curr->next;
    }
    return 0;
}
