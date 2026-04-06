#ifndef _KERNEL_PIC_H
#define _KERNEL_PIC_H

void pic_install(void);
void pic_send_eoi(unsigned char irq);

#endif
