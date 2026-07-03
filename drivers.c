#include "drivers.h"

#define MOD_SHIFT 1
#define VIDEO_ADDRESS 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0F

static int cursor_row = 0;
static int cursor_col = 0;

static unsigned char key_modifiers = 0;

struct KeyEntry {
    char normal;
    char shift;
};

static const struct KeyEntry keymap[] = {
    [0x00] = {0, 0},     [0x01] = {27, 27},   [0x02] = {'1', '!'}, [0x03] = {'2', '@'},
    [0x04] = {'3', '#'}, [0x05] = {'4', '$'}, [0x06] = {'5', '%'}, [0x07] = {'6', '^'},
    [0x08] = {'7', '&'}, [0x09] = {'8', '*'}, [0x0A] = {'9', '?'}, [0x0B] = {'0', ')'},
    [0x0C] = {'-', '_'}, [0x0D] = {'=', '+'}, [0x0E] = {'\b', '\b'},[0x0F] = {'\t', '\t'},
    [0x10] = {'q', 'Q'}, [0x11] = {'w', 'W'}, [0x12] = {'e', 'E'}, [0x13] = {'r', 'R'},
    [0x14] = {'t', 'T'}, [0x15] = {'y', 'Y'}, [0x16] = {'u', 'U'}, [0x17] = {'i', 'I'},
    [0x18] = {'o', 'O'}, [0x19] = {'p', 'P'}, [0x1A] = {'[', '{'}, [0x1B] = {']', '}'},
    [0x1C] = {'\n', '\n'},[0x1D] = {0, 0},     [0x1E] = {'a', 'A'}, [0x1F] = {'s', 'S'},
    [0x20] = {'d', 'D'}, [0x21] = {'f', 'F'}, [0x22] = {'g', 'G'}, [0x23] = {'h', 'H'},
    [0x24] = {'j', 'J'}, [0x25] = {'k', 'K'}, [0x26] = {'l', 'L'}, [0x27] = {';', ':'},
    [0x28] = {'\'', '"'}, [0x29] = {'`', '~'}, [0x2A] = {0, 0},     [0x2B] = {'\\', '|'},
    [0x2C] = {'z', 'Z'}, [0x2D] = {'x', 'X'}, [0x2E] = {'c', 'C'}, [0x2F] = {'v', 'V'},
    [0x30] = {'b', 'B'}, [0x31] = {'n', 'N'}, [0x32] = {'m', 'M'}, [0x33] = {',', '<'},
    [0x34] = {'.', '>'}, [0x35] = {'/', '?'}, [0x36] = {0, 0},     [0x39] = {' ', ' '}
}; // лучшая таблица символов 2.0

char get_char(void) {
    while ((port_byte_in(0x64) & 1) == 0);
    unsigned char scancode = port_byte_in(0x60);

    if (scancode == 0x2A || scancode == 0x36) {
        key_modifiers |= MOD_SHIFT;
        return 0;
    }

    if (scancode == 0x2A + 0x80 || scancode == 0x36 + 0x80) {
        key_modifiers &= ~MOD_SHIFT;
        return 0;
    }

    if (scancode >= 0x80) {
        return 0;
    }

    if (scancode < sizeof(keymap) / sizeof(struct KeyEntry)) {
        struct KeyEntry entry = keymap[scancode];

        if (key_modifiers & MOD_SHIFT) {
            return entry.shift;
        }
        return entry.normal;
    }

    return 0;
}


unsigned char port_byte_in(unsigned short port) {
    unsigned char result;
    __asm__ volatile("inb %w1, %%al" : "=a" (result) : "d" (port));
    return result;
}

void port_byte_out(unsigned short port, unsigned char data) {
    __asm__ volatile("outb %%al, %%dx" : : "a" (data), "d" (port));
}

unsigned short port_word_in(unsigned short port) {
    unsigned short result;
    __asm__ volatile ("inw %w1, %%ax" : "=a"(result) : "d"(port));
    return result;
}
void port_word_out(unsigned short port, unsigned short data) {
    __asm__ volatile("outw %0, %1" : : "a"(data), "d"(port));
}
void ata_read_sectors(void *target_buffer, unsigned int lba, unsigned char sectors_count) {
    port_byte_out(0x1F2, sectors_count);

    port_byte_out(0x1F3, (unsigned char)lba);
    port_byte_out(0x1F4, (unsigned char)(lba >> 8));
    port_byte_out(0x1F5, (unsigned char)(lba >> 16));

    port_byte_out(0x1F6, (unsigned char)((lba >> 24) & 0x0F) | 0xE0);

    port_byte_out(0x1F7, 0x20);

    unsigned short *buffer = (unsigned short *)target_buffer;

    for (int s = 0; s < sectors_count; s++) {
        volatile int timeout = 0;
        while ((port_byte_in(0x1F7) & 0x80) != 0 && timeout < 100000) timeout++;

        timeout = 0;
        while ((port_byte_in(0x1F7) & 0x08) == 0 && timeout < 100000) timeout++;

        for (int i = 0; i < 256; i++) {
            *buffer = port_word_in(0x1F0);
            buffer++;
        }
    }
}

void ata_write_sectors(void *source_buffer, unsigned int lba, unsigned char sectors_count) {
    port_byte_out(0x1F2, sectors_count);
    port_byte_out(0x1F3, (unsigned char)lba);
    port_byte_out(0x1F4, (unsigned char)(lba >> 8));
    port_byte_out(0x1F5, (unsigned char)(lba >> 16));
    port_byte_out(0x1F6, (unsigned char)((lba >> 24) & 0x0F) | 0xE0);

    port_byte_out(0x1F7, 0x30);

    unsigned short *buffer = (unsigned short *)source_buffer;

    for (int s = 0; s < sectors_count; s++) {
        volatile int timeout = 0;
        while ((port_byte_in(0x1F7) & 0x80) != 0 && timeout < 100000) timeout++;
        timeout = 0;
        while ((port_byte_in(0x1F7) & 0x08) == 0 && timeout < 100000) timeout++;

        for (int i = 0; i < 256; i++) {
            port_word_out(0x1F0, *buffer);
            buffer++;
        }
    }
}


static void update_cursor() {
    unsigned short position = cursor_row * MAX_COLS + cursor_col;
    port_byte_out(0x3D4, 0x0F);
    port_byte_out(0x3D5, (unsigned char)(position & 0xFF));
    port_byte_out(0x3D4, 0x0E);
    port_byte_out(0x3D5, (unsigned char)((position >> 8) & 0xFF));
}

void init_terminal() {
    clear_screen();
}

void clear_screen() {
    volatile unsigned short *video = (volatile unsigned short*)VIDEO_ADDRESS;
    for (int i = 0; i < MAX_ROWS * MAX_COLS; i++) {
        video[i] = (WHITE_ON_BLACK << 8) | ' ';
    }
    cursor_row = 0;
    cursor_col = 0;
    update_cursor();
}

void kprint_char(char c) {
    volatile unsigned short *video = (volatile unsigned short*)VIDEO_ADDRESS;

    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
    } else if (c == '\b') {
        if (cursor_col > 0) {
            cursor_col--;
            int offset = cursor_row * MAX_COLS + cursor_col;
            video[offset] = (WHITE_ON_BLACK << 8) | ' ';
        }
    } else {
        int offset = cursor_row * MAX_COLS + cursor_col;
        video[offset] = (WHITE_ON_BLACK << 8) | (unsigned char)c;
        cursor_col++;
    }

    if (cursor_col >= MAX_COLS) {
        cursor_col = 0;
        cursor_row++;
    }

    if (cursor_row >= MAX_ROWS) {
        for (int r = 1; r < MAX_ROWS; r++) {
            for (int col = 0; col < MAX_COLS; col++) {
                video[(r - 1) * MAX_COLS + col] = video[r * MAX_COLS + col];
            }
        }
        for (int col = 0; col < MAX_COLS; col++) {
            video[(MAX_ROWS - 1) * MAX_COLS + col] = (WHITE_ON_BLACK << 8) | ' ';
        }
        cursor_row = MAX_ROWS - 1;
    }
    update_cursor();
}

void kprint(const char *str) {
    while (*str) {
        kprint_char(*str++);
    }
}
