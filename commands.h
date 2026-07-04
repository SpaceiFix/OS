#ifndef COMMANDS_H
#define COMMANDS_H

typedef struct {
    const char *name;
    const char *description;
    void (*function)(const char *args);
} Command;

void help_command(const char *args);
void echo_command(const char *args);
void clear_command(const char *args);
void read_test_command(const char *args);
void mkfs_command(const char *args);
void fs_status_command(const char *args);
void shutdown_command(const char *args);
void touch_command(const char *args);
void ls_command(const char *args);
void cat_command(const char *args);
void write_command(const char *args);
void disk_debug_command(const char *args);
void rm_command(const char *args);

static Command commands[] = {
    {"help", "Display available commands", help_command},
    {"echo", "Print a message", echo_command},
    {"clear", "Clear the screen", clear_command},
    {"read-test", "Test reading from disk", read_test_command},
    {"mkfs", "Create a new filesystem", mkfs_command},
    {"fs-status", "Check the status of the filesystem", fs_status_command},
    {"shutdown", "Shutdown the system", shutdown_command},
    {"touch", "Create an empty file", touch_command},
    {"ls", "List files in root directory", ls_command},
    {"cat", "Display file content", cat_command},
    {"write", "Write text to file", write_command},
    {"disk-debug", "Hardware disk read/write test", disk_debug_command},
    {"rm", "Delete a file from disk", rm_command},
    {(void*)0,    (void*)0,}
};

#endif
