#include <stdint.h>

struct idt_entry {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t  ist;
    uint8_t  attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[48];
static struct idt_ptr idtr;

// Tambahkan variabel pengujian yang dicari oleh kmain.c
uint64_t x86_64_idt_base_for_test = 0;
uint16_t x86_64_idt_limit_for_test = 0;

extern void isr_stub_14(void); 
#define DECLARE_IRQ(n) extern void isr_stub_##n(void);
DECLARE_IRQ(32); DECLARE_IRQ(33); DECLARE_IRQ(34); DECLARE_IRQ(35);
DECLARE_IRQ(36); DECLARE_IRQ(37); DECLARE_IRQ(38); DECLARE_IRQ(39);
DECLARE_IRQ(40); DECLARE_IRQ(41); DECLARE_IRQ(42); DECLARE_IRQ(43);
DECLARE_IRQ(44); DECLARE_IRQ(45); DECLARE_IRQ(46); DECLARE_IRQ(47);

void x86_64_idt_set_gate(uint8_t vector, void (*isr)(void), uint8_t attributes) {
    uint64_t addr = (uint64_t)isr;
    idt[vector].isr_low    = addr & 0xFFFF;
    idt[vector].kernel_cs  = 0x08;
    idt[vector].ist        = 0;
    idt[vector].attributes = attributes;
    idt[vector].isr_mid    = (addr >> 16) & 0xFFFF;
    idt[vector].isr_high   = (addr >> 32) & 0xFFFFFFFF;
    idt[vector].reserved   = 0;
}

void x86_64_idt_init(void) {
    x86_64_idt_set_gate(14, isr_stub_14, 0x8E);

    #define REGISTER_IRQ(n) x86_64_idt_set_gate(n, isr_stub_##n, 0x8E);
    REGISTER_IRQ(32); REGISTER_IRQ(33); REGISTER_IRQ(34); REGISTER_IRQ(35);
    REGISTER_IRQ(36); REGISTER_IRQ(37); REGISTER_IRQ(38); REGISTER_IRQ(39);
    REGISTER_IRQ(40); REGISTER_IRQ(41); REGISTER_IRQ(42); REGISTER_IRQ(43);
    REGISTER_IRQ(44); REGISTER_IRQ(45); REGISTER_IRQ(46); REGISTER_IRQ(47);

    idtr.limit = (sizeof(struct idt_entry) * 48) - 1;
    idtr.base  = (uint64_t)&idt;

    // Isi nilai variabel pengujian agar kmain.c lulus tes
    x86_64_idt_base_for_test = idtr.base;
    x86_64_idt_limit_for_test = idtr.limit;

    __asm__ volatile ("lidt %0" :: "m"(idtr));
}
