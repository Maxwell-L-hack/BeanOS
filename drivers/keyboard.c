#include "keyboard.h"
#include "../CPU/ports.h"
#include "../CPU/isr.h"
#include "screen.h"
#include "../libc/string.h"
#include "../libc/function.h"
#include "../Kernel/kernel.h"

#define BACKSPACE 0x0E
#define ENTER 0x1c


static char key_buffer[256];

#define SC_MAX 57
const char *sc_name[] = { "ERROR, "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char sc_ascii[] = { '?', '?', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};
static void keyboard_callback(registers_t regs) {
    u8 scancode = port_byte_in(0x60);
    char *sc_ascii;
    int_to_ascii(scancode, sc_ascii);
    kprint("Keyboard scancode: ");
    kprint(sc_ascii);
    kprint(", ");
    print_letter(scancode);
    kprint("\n");
}

void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
}