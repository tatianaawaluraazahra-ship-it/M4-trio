.section .text

.global cpu_read_rflags
.type cpu_read_rflags, @function
cpu_read_rflags:
    pushfq
    popq %rax
    ret
.size cpu_read_rflags, . - cpu_read_rflags

.global cpu_halt_forever
.type cpu_halt_forever, @function
cpu_halt_forever:
1:
    hlt
    jmp 1b
.size cpu_halt_forever, . - cpu_halt_forever
