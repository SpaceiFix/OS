
typedef struct {
    char *name;
    char *description;
    void (*function)(const char *args);
} Command;


// декларируем комманды что бы компилятор знал о них до их использования :3 (UWU)
void help_command(const char *args);
void echo_command(const char *args);
void clear_command(const char *args);
void fs_status(const char *args);
void read_test(const char *args);
void mkfs_command(const char *args);

Command commands[] = {
    {"help", "Display available commands", help_command},
    {"echo", "Print a message", echo_command},
    {"clear", "Clear the screen", clear_command},
    {"read-test", "Test reading from disk", read_test},
    {"mkfs", "Create a new filesystem", mkfs_command},
    {"fs-status", "Check the status of the filesystem", fs_status},
    {NULL, NULL}
};

