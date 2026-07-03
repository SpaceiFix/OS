#include "drivers.h"

#define VIDEO_ADDRESS 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0F

static int cursor_row = 0;
static int cursor_col = 0;

static const char ascii_map[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '`', '\'',  0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,   '*',   0, ' '
};

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

char get_char() {
    while ((port_byte_in(0x64) & 1) == 0);
    unsigned char scancode = port_byte_in(0x60);
    if (scancode < sizeof(ascii_map)) {
        return ascii_map[scancode];
    }
    return 0;
}
