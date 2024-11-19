#define STACK_REFPLUS 1

#ifdef SLP_EVAL

#define STACK_MAGIC 0

#define REGS_TO_SAVE "r12", "r13", "r14", "r15"

static int slp_switch(void) {
    int err;
    void* rbp;
    void* rbx;
    unsigned int csr;
    unsigned short cw;
    long *stackref, stsizediff;

    __asm__ volatile ("" : : : REGS_TO_SAVE);

    // Сохранение текущих контекстов (примерно)
    __asm__ volatile ("fnstcw %0" : "=m" (cw)); // Сохранение контроля окна
    __asm__ volatile ("stmxcsr %0" : "=m" (csr)); // Сохранение MXCSR
    __asm__ volatile ("mov %rbp, %0" : "=m" (rbp)); // Сохранение rbp
    __asm__ volatile ("mov %rbx, %0" : "=m" (rbx)); // Сохранение rbx
    __asm__ ("mov %rsp, %0" : "=g" (stackref)); // Сохранение указателя стека

    {
        SLP_SAVE_STATE(stackref, stsizediff);

        __asm__ volatile (
            "add %0, %rsp\n"
            "add %0, %rbp\n"
            :
            : "r" (stsizediff)
        );

        SLP_RESTORE_STATE();
        __asm__ volatile ("xor %rax, %rax" : "=a" (err)); // Установка err в 0
    }

    __asm__ volatile ("mov %0, %rbx" : : "m" (rbx)); // Восстановление rbx
    __asm__ volatile ("mov %0, %rbp" : : "m" (rbp)); // Восстановление rbp
    __asm__ volatile ("ldmxcsr %0" : : "m" (csr)); // Восстановление MXCSR
    __asm__ volatile ("fldcw %0" : : "m" (cw)); // Восстановление контроля окна

    __asm__ volatile ("" : : : REGS_TO_SAVE);
    return err;
}

#endif
