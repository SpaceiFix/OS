void log(const char *msg){
    unsigned char *video = (unsigned char*)0xB8000;
    while(*msg){
        *video++ = *msg++;
        *video++ = 0x07;
    }
}
void kmain(){
    log("Hello World!");
    while(1){
        __asm__ volatile ("hlt");
    }
}
