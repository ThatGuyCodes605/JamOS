#include <stddef.h>
#include <string.h>
#include <kernel/tty.h>
#include <kernel/keyboard.h>
#include <kernel/console.h>

#define JAMOS_VERSION "0.0.1"
#define PROMPT        "jamos> "
#define LINE_MAX      256
#define ARG_MAX       16

typedef int (*cmd_fn)(int argc, char **argv);

typedef struct {
	const char *name;
	const char *desc;
	cmd_fn fn;
} cmd_t;

static int cmd_help(int argc, char **argv);
static int cmd_echo(int argc, char **argv);
static int cmd_clear(int argc, char **argv);
static int cmd_ver(int argc, char **argv);
static int cmd_halt(int argc, char **argv);

static const cmd_t cmds[] = {
	{ "help",    "list commands",     cmd_help  },
	{ "echo",    "echo arguments",    cmd_echo  },
	{ "clear",   "clear the screen",  cmd_clear },
	{ "version", "show version info", cmd_ver   },
	{ "halt",    "halt the system",   cmd_halt  },
	{ NULL, NULL, NULL }
};

static int cmd_help(int argc, char **argv) {
	(void)argc; (void)argv;
	for (const cmd_t *cmd = cmds; cmd->name; cmd++) {
		terminal_writestring("  ");
		terminal_writestring(cmd->name);
		terminal_writestring("\t- ");
		terminal_writestring(cmd->desc);
		terminal_putchar('\n');
	}
	return 0;
}

static int cmd_echo(int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		terminal_writestring(argv[i]);
		if (i < argc - 1)
			terminal_putchar(' ');
	}
	terminal_putchar('\n');
	return 0;
}

static int cmd_clear(int argc, char **argv) {
	(void)argc; (void)argv;
	terminal_clear();
	return 0;
}

static int cmd_ver(int argc, char **argv) {
	(void)argc; (void)argv;
	terminal_writestring("JamOS version ");
	terminal_writestring(JAMOS_VERSION);
	terminal_putchar('\n');
	return 0;
}

static int cmd_halt(int argc, char **argv) {
	(void)argc; (void)argv;
	terminal_writestring("Halting...\n");
	for (;;) __asm__ volatile ("hlt");
	return 0;
}

/* case-insensitive strcmp */
static int streqi(const char *a, const char *b) {
	while (*a && *b) {
		char ca = (*a >= 'A' && *a <= 'Z') ? (*a - 'A' + 'a') : *a;
		char cb = (*b >= 'A' && *b <= 'Z') ? (*b - 'A' + 'a') : *b;
		if (ca != cb) return 0;
		a++; b++;
	}
	return *a == '\0' && *b == '\0';
}

static void read_line(char *buf) {
	int i = 0;
	char c;
	while (1) {
		c = keyboard_getchar();
		if (c == '\n') {
			buf[i] = '\0';
			terminal_putchar('\n');
			return;
		} else if (c == '\b') {
			if (i > 0) {
				i--;
				terminal_putchar('\b');
			}
		} else if (i < LINE_MAX - 1) {
			buf[i++] = c;
			terminal_putchar(c);
		}
	}
}

static int tokenize(char *buf, char **argv, int max) {
	int argc = 0;
	char *p = buf;
	while (*p) {
		while (*p == ' ' || *p == '\t') p++;
		if (!*p || argc >= max) break;
		argv[argc++] = p;
		while (*p && *p != ' ' && *p != '\t') p++;
		if (*p) *p++ = '\0';
	}
	argv[argc] = NULL;
	return argc;
}

void console_run(void) {
	char line[LINE_MAX];
	char *argv[ARG_MAX];

	cmd_ver(0, NULL);
	terminal_putchar('\n');

	for (;;) {
		terminal_writestring(PROMPT);
		read_line(line, LINE_MAX);

		int argc = tokenize(line, argv, ARG_MAX);
		if (!argc) continue;

		int found = 0;
		for (const cmd_t *cmd = cmds; cmd->name; cmd++) {
			if (streqi(argv[0], cmd->name)) {
				cmd->fn(argc, argv);
				found = 1;
				break;
			}
		}

		if (!found) {
			terminal_writestring("Unknown command: ");
			terminal_writestring(argv[0]);
			terminal_putchar('\n');
		}
	}
}
