#include <stdint.h>
#include <pic.h>
#include <pit.h>

// Deklarasi fungsi inisialisasi eksternal bawaan kernel kamu
extern void serial_init(void);
extern void x86_64_idt_init(void);

// Fungsi manipulasi CPU inline assembly aman
static inline void cpu_cli(void) {
    __asm__ volatile ("cli" ::: "memory");
}

static inline void cpu_sti(void) {
    __asm__ volatile ("sti" ::: "memory");
}

void kmain(void) {
    // 1. cli -> Matikan interupsi global terlebih dahulu untuk keamanan
    cpu_cli();

    // 2. serial_init -> Nyalakan komunikasi log serial
    serial_init();

    // 3. idt_init -> Pasang tabel gerbang interupsi (IDT)
    x86_64_idt_init();

    // 4. pic_remap -> Atur ulang pemetaan jalur interupsi PIC
    pic_remap(0x20, 0x28);

    // 5. pic_mask_all -> Tutup seluruh jalur IRQ hardware sebagai safe default
    pic_mask_all();

    // 6. pic_unmask_irq0 -> Buka gerbang khusus untuk IRQ0 (Timer PIT)
    pic_unmask_irq(0);

    // 7. pit_configure -> Set detak jam internal ke frekuensi 100 Hz
    pit_configure_hz(100);

    // 8. sti -> Jalur infrastruktur siap, nyalakan kembali interupsi global
    cpu_sti();

    // 9. hlt loop -> Loop abadi menghemat daya CPU sambil menunggu detak jam masuk
    while (1) {
        __asm__ volatile ("hlt");
    }
}
