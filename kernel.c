#include "drivers.h"
#include "fs.h"
#include "commands.h"

void mkfs(void);
int check_fs(void);
int shutdown(void);

int str_compare(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2) {
            return (int)((unsigned char)*s1 - (unsigned char)*s2);
        }
        s1++;
        s2++;
    }
    return (int)((unsigned char)*s1 - (unsigned char)*s2);
}

int str_start_with(const char *str, const char *prefix) {
    while (*prefix) {
        if (*prefix != *str) return 0;
        prefix++;
        str++;
    }
    return 1;
}

void help_command(const char *args) {
    kprint("Available commands:\n");
    for (int i = 0; commands[i].name != (void*)0; i++) {
        kprint(" - ");
        kprint(commands[i].name);
        kprint(" - ");
        kprint(commands[i].description);
        kprint("\n");
    }
}

void echo_command(const char *args) {
    if (args == (void*)0 || *args == '\0') {
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

void read_test_command(const char *args) {
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

void fs_status_command(const char *args) {
    if (check_fs()) {
        kprint("ponOS DETECTED!! Good BOY\n");
    } else {
        kprint("Something went wrong, ponOS not found\n");
    }
}

void shutdown_command(const char *args) {
    shutdown();
}

void touch_command(const char *args) {
    if (args == (void*)0 || *args == '\0') {
        kprint("Usage: touch <filename>\n");
        return;
    }
    sys_touch(args);
}

void ls_command(const char *args) {
    sys_ls();
}

void cat_command(const char *args) {
    if (args == (void*)0 || *args == '\0') {
        kprint("Usage: cat <filename>\n");
        return;
    }
    sys_cat(args);
}

void write_command(const char *args) {
    if (args == (void*)0 || *args == '\0') {
        kprint("Usage: write <filename> <text>\n");
        return;
    }

    char filename[16];
    for (int idx = 0; idx < 16; idx++) {
        filename[idx] = '\0';
    }

    int i = 0;
    while (args[i] != ' ' && args[i] != '\0' && i < 15) {
        filename[i] = args[i];
        i++;
    }
    filename[i] = '\0';

    while (args[i] == ' ') {
        i++;
    }

    if (args[i] == '\0') {
        kprint("Usage: write <filename> <text>\n");
        return;
    }

    const char *text = args + i;
    sys_write_file(filename, text);
}

void rm_command(const char *args) {
    if (args == (void*)0 || *args == '\0') {
        kprint("Usage: rm <filename>\n");
        return;
    }
    sys_rm(args);
}

void execute_command(const char *input) {
    while (*input == ' ') input++;

    for (int i = 0; commands[i].name != (void*)0; i++) {
        int cmd_len = 0;
        while (commands[i].name[cmd_len] != '\0') cmd_len++;

        if (str_start_with(input, commands[i].name)) {
            char next_char = input[cmd_len];

            if (next_char == '\0' || next_char == ' ') {
                const char *args = "";

                if (next_char == ' ') {
                    args = input + cmd_len;
                    while (*args == ' ') args++;
                }

                char clean_args[128];
                int a_idx = 0;

                while (args[a_idx] != '\0' && args[a_idx] != '\r' && args[a_idx] != '\n' && a_idx < 127) {
                    clean_args[a_idx] = args[a_idx];
                    a_idx++;
                }
                clean_args[a_idx] = '\0';

                while (a_idx > 0 && clean_args[a_idx - 1] == ' ') {
                    a_idx--;
                    clean_args[a_idx] = '\0';
                }

                commands[i].function(clean_args);
                return;
            }
        }
    }

    kprint("Unknown command: ");
    kprint(input);
    kprint("\n");
}


void disk_debug_command(const char *args) {
    char write_buf[512];
    char read_buf[512];

    for (int i = 0; i < 512; i++) {
        write_buf[i] = '\0';
        read_buf[i] = '\0';
    }

    int len = 0;
    const char *msg = "ponOS Hardware Disk Write Test Successful";
    while (msg[len] != '\0' && len < 511) {
        write_buf[len] = msg[len];
        len++;
    }

    kprint("Writing debug pattern to sector 50...\n");
    ata_write_sectors(write_buf, 50, 1);

    for (volatile int i = 0; i < 2000000; i++);

    kprint("Reading back from sector 50...\n");
    ata_read_sectors(read_buf, 50, 1);

    kprint("Result: ");
    kprint(read_buf);
    kprint("\n");
}


void kmain(void) {
    init_terminal();

    kprint("ponOS (v0.67)\n");
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
        else if (c == '\b') {
            if (buffer_idx > 0) {
                buffer_idx--;
                kprint_char('\b');
            }
        }
        else {
            if (buffer_idx < 254) {
                command_buffer[buffer_idx++] = c;
                kprint_char(c);
            }
        }
    }
}
