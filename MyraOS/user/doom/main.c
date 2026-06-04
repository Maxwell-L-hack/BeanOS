#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <window.h>
#include <stdio.h>
#include <fcntl.h>

#include "doomgeneric.h"

int main(int argc, char** argv) {
    char* args[] = { "doomgeneric", "-iwad", "/doom1.wad" };
    int argcount = 3;

    doomgeneric_Create(argcount, args);

    while (1) {
        doomgeneric_Tick();
    }

    return 0;
}
