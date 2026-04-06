#ifndef _KERNEL_ISR_H
#define _KERNEL_ISR_H

#include <stdint.h>

struct regs {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

typedef void (*irq_handler_t)(struct regs *);

void isr_install(void);
void irq_install_handler(int irq, irq_handler_t handler);

#endif
