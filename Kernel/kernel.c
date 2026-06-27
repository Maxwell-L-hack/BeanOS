#include "../drivers/screen.h"
#include "util.h"

void main() {
    clear_screen();

    int i = 0;
    for (i = 0; i < 24; i++) {
        char str[255];
        int_to_ascii(i, str);
        kprint_at(str, 0, i);
    }

    kprint_at("This text makes the kernel scroll.  row 0 will disappear. ", 60, 24);
    kprint("due to this text, the kernel will scroll again, and row 1 will disappear.");
}