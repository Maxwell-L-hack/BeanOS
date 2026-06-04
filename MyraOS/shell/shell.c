#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_INPUT 256
#define MAX_ARGS  16

static void cmd_help(void);
static void cmd_clear(void);
static void cmd_echo(int argc, char* argv[]);
static void cmd_run(int argc, char* argv[]);

void shell_run() {
    char input[MAX_INPUT];
    char* argv[MAX_ARGS];

    while (true) {
        write(1, "\x1b[32mmyra\x1b[0m$ ", 15);

        int n = read(0, input, MAX_INPUT);
        if (n <= 0) {
            continue;
        }

        input[n] = '\0';
        if (input[n - 1] == '\n') {
            input[n - 1] = '\0';
        }

        int argc = 0;
        char* tok = strtok(input, " ");
        while (tok && argc < MAX_ARGS - 1) {
            argv[argc++] = tok;
            tok = strtok(NULL, " ");
        }
        argv[argc] = NULL;
        if (argc == 0) {
            continue;
        }

        if (!strcmp(argv[0], "exit"))  { return; }
        if (!strcmp(argv[0], "help"))  { cmd_help(); continue; }
        if (!strcmp(argv[0], "clear")) { cmd_clear(); continue; }
        if (!strcmp(argv[0], "echo"))  { cmd_echo(argc, argv); continue; }
        if (!strcmp(argv[0], "run"))   { cmd_run(argc, argv); continue; }

        puts("\x1b[31mUnknown command\x1b[0m\n");
    }
}

static void cmd_help(void) {
    puts("MyraShell commands:\n");
    puts("  echo <text>   - print text\n");
    puts("  clear         - clear the screen\n");
    puts("  run <path>    - run a program (e.g. /bin/demo)\n");
    puts("  exit          - exit the shell\n");
}

static void cmd_clear(void) {
    write(1, "\x1b[2J\x1b[H", 7);
}

static void cmd_echo(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        write(1, argv[i], strlen(argv[i]));
        write(1, " ", 1);
    }
    write(1, "\n", 1);
}

static void cmd_run(int argc, char* argv[]) {
    if (argc < 2) {
        puts("\x1b[33mUsage: run <path>\x1b[0m\n");
        return;
    }

    int pid = execve(argv[1], NULL, NULL);
    if (pid < 0) {
        puts("\x1b[31mFailed to launch process\x1b[0m\n");
    }
}
