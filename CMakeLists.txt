cmake_minimum_required(VERSION 4.1)
project(JamOS C)

set(CMAKE_C_STANDARD 11)

include_directories(kernel/arch/i386)
include_directories(kernel/include)
include_directories(kernel/include/kernel)
include_directories(libc/include)
include_directories(libc/include/sys)

add_executable(JamOS
        kernel/arch/i386/tty.c
        kernel/arch/i386/vga.h
        kernel/include/kernel/tty.h
        kernel/kernel/kernel.c
        libc/include/sys/cdefs.h
        libc/include/stdio.h
        libc/include/stdlib.h
        libc/include/string.h
        libc/stdio/printf.c
        libc/stdio/putchar.c
        libc/stdio/puts.c
        libc/stdlib/abort.c
        libc/string/memcmp.c
        libc/string/memcpy.c
        libc/string/memmove.c
        libc/string/memset.c
        libc/string/strlen.c
)
