#include "drivers.h"
#include "stddef.h"
#include "commands.h"

void mkfs(void);
int check_fs(void);

int str_compare(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

// TYERMINAL COMMANDS 
void help_command(const char *args) {
    kprint("Available commands:\n");
    for (int i = 0; commands[i].name != NULL; i++) {
        kprint(" - ");
        kprint(commands[i].name);
        kprint(" - ");
        kprint(commands[i].description);
        kprint("\n");
    }
}

void echo_command(const char *args) {
    if (args == NULL || *args == '\0') {
        kprint("Usage: echo <message>\n");
        return;
    }

    kprint("you wrote: ");
    kprint(args);
    kprint("\n");
  
}

void clear_command(const char *args) {
    clear_screen();
}

void read_test(const char *args) {
    char test_buffer[513];
    kprint("Reading sector 33 via LBA28 :money:\n");
    ata_read_sectors(test_buffer, 33, 1);
    test_buffer[512] = '\0';
    kprint("Sector content: ");
    kprint(test_buffer);
    kprint("\n");
}

void mkfs_command(const char *args) {
    mkfs();
}

void fs_status(const char *args) {
    if (check_fs()) {
        kprint("ponOS DETECTED!! Good BOY\n");
    } else {
        kprint("Something went wrong, ponOS not found\n");
    }
}

// TERMINAL COMMANDS END

int str_start_with(const char *str, const char *prefix) {
    while (*prefix) {
        if (*prefix != *str) return 0;
        prefix++;
        str++;
    }
    return 1; // true
}

void execute_command(const char *input) {

    while (*input == ' ') input++; // Пропуск начальных пробелов uwu

    for (int i = 0; commands[i].name != NULL; i++) {
        int cmd_len = 0;
        while (commands[i].name[cmd_len] != '\0') cmd_len++;  // length of command name UWU :3

        if (str_start_with(input, commands[i].name)) {
            char next_char = input[cmd_len];

            if (next_char == '\0' || next_char == ' ') {
                const char *args = "";

                if (next_char == ' ') {
                    args = input + cmd_len;
                    while (*args == ' ') args++; 
                }
                
                commands[i].function(args);
                return;
            }
        }
    }
    kprint("Unknown command: ");
    kprint(input);
    kprint("\n");
}


/*
 * Точка входа в ядро (вызывается из ассемблерного кода kernel_entry.asm).
 * Функция никогда не должна возвращать управление, так как ОС работает циклично.
 */
void kmain(void) {
    init_terminal();

    // Первое что ты видишь
    kprint("TarhunOS (v0.42)\n");
    kprint("Type 'help' to get started.\n\n");
    kprint("[NO_ONE@Not_Linux_Terminal_trust]$ ");

    char command_buffer[256];
    int buffer_idx = 0;    


    while (1) {
        char c = get_char();

        if (c == 0) continue;

        if (c == '\n') {
            kprint_char('\n');
            command_buffer[buffer_idx] = '\0';

            execute_command(command_buffer);

            buffer_idx = 0;
            kprint("[NO_ONE@Not_Linux_Terminal_trust]$ ");
        }
        // Система стирания символов
        else if (c == '\b') {
            if (buffer_idx > 0) {
                buffer_idx--;
                kprint_char('\b');
            }
        }
        else {
            // Защита от переполнения буфера
            if (buffer_idx < 254) {
                command_buffer[buffer_idx++] = c;
                kprint_char(c);
            }
        }
    }
}