#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "vga.h"
#include "io.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

static void update_cursor(void) {
	size_t pos = terminal_row * VGA_WIDTH + terminal_column;
	outb(0x3D4, 14);
	outb(0x3D5, (pos >> 8) & 0xFF);
	outb(0x3D4, 15);
	outb(0x3D5, pos & 0xFF);
}

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
	update_cursor();
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll(int line) {
	volatile uint16_t *dst = VGA_MEMORY + (line - 1) * VGA_WIDTH;
	volatile uint16_t *src = VGA_MEMORY + line * VGA_WIDTH;
	for (size_t x = 0; x < VGA_WIDTH; x++)
		dst[x] = src[x];
}

void terminal_delete_last_line(void) {
	volatile uint16_t *ptr = VGA_MEMORY + (VGA_HEIGHT - 1) * VGA_WIDTH;
	for (size_t x = 0; x < VGA_WIDTH; x++)
		ptr[x] = vga_entry(' ', terminal_color);
}

void terminal_putchar(char c) {
	int line;
	unsigned char uc = c;

	if (c == '\n') {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) {
			for (line = 1; line <= (int)(VGA_HEIGHT - 1); line++)
				terminal_scroll(line);
			terminal_delete_last_line();
			terminal_row = VGA_HEIGHT - 1;
		}
		update_cursor();
		return;
	}

	if (c == '\b') {
		if (terminal_column > 0) {
			terminal_column--;
		} else if (terminal_row > 0) {
			terminal_row--;
			terminal_column = VGA_WIDTH - 1;
		}
		terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
		update_cursor();
		return;
	}

	terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) {
			for (line = 1; line <= (int)(VGA_HEIGHT - 1); line++)
				terminal_scroll(line);
			terminal_delete_last_line();
			terminal_row = VGA_HEIGHT - 1;
		}
	}
	update_cursor();
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

void terminal_clear(void) {
	terminal_row = 0;
	terminal_column = 0;
	for (size_t y = 0; y < VGA_HEIGHT; y++)
		for (size_t x = 0; x < VGA_WIDTH; x++)
			terminal_buffer[y * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
	update_cursor();
}
