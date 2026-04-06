#include <stdint.h>
#include <kernel/keyboard.h>
#include <kernel/isr.h>
#include "io.h"

#define KEYBOARD_DATA 0x60

static const char scancode_table[] = {
    0,    27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '-',  '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o',  'p', '[', ']', '\n', 0,  'a', 's', 'd', 'f', 'g', 'h',
    'j',  'k', 'l', ';', '\'', '`', 0,  '\\','z', 'x', 'c', 'v',
    'b',  'n', 'm', ',', '.', '/', 0,  '*', 0,  ' ', 0
};

static const char scancode_shift[] = {
    0,    27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
    '_',  '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O',  'P', '{', '}', '\n', 0,  'A', 'S', 'D', 'F', 'G', 'H',
    'J',  'K', 'L', ':', '"',  '~', 0,  '|', 'Z', 'X', 'C', 'V',
    'B',  'N', 'M', '<', '>', '?', 0,  '*', 0,  ' ', 0
};

static volatile char key_buffer = 0;
static int shift = 0;

static void keyboard_handler(struct regs *r) {
    (void)r;
    uint8_t scancode = inb(KEYBOARD_DATA);

    if (scancode & 0x80) {
        /* key release */
        scancode &= 0x7F;
        if (scancode == 0x2A || scancode == 0x36)
            shift = 0;
    } else {
        /* key press */
        if (scancode == 0x2A || scancode == 0x36) {
            shift = 1;
        } else if (scancode < sizeof(scancode_table)) {
            char c = shift ? scancode_shift[scancode] : scancode_table[scancode];
            if (c)
                key_buffer = c;
        }
    }
}

void keyboard_install(void) {
    irq_install_handler(1, keyboard_handler);
}

char keyboard_getchar(void) {
    char c;
    while (!key_buffer)
        __asm__ volatile ("hlt");
    c = key_buffer;
    key_buffer = 0;
    return c;
}
