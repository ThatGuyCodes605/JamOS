#include <kernel/tty.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/pic.h>
#include <kernel/isr.h>
#include <kernel/keyboard.h>
#include <kernel/console.h>

void kernel_main(void) {
    terminal_initialize();
    gdt_install();
    terminal_writestring("GDT ok\n");
    idt_install();
    terminal_writestring("IDT ok\n");
    pic_install();
    terminal_writestring("PIC ok\n");
    isr_install();
    terminal_writestring("ISR ok\n");
    keyboard_install();
    terminal_writestring("Keyboard ok\n");
    terminal_writestring("Welcome to JamOS!\n");
    console_run();
}
