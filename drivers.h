#ifndef DRIVERS_H
#define DRIVERS_H

unsigned char port_byte_in(unsigned short port);
void port_byte_out(unsigned short port, unsigned char data);
void port_word_out(unsigned short port, unsigned short data);
void ata_read_sectors(void *target_buffer, unsigned int lba, unsigned char sectors_count);
void ata_write_sectors(void *source_buffer, unsigned int lba, unsigned char sectors_count);

void init_terminal();
void kprint(const char *str);
void kprint_char(char c);
void clear_screen();

char get_char();

#endif
