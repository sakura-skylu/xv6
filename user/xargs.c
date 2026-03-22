#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    char buf[512];
    char *newargv[MAXARG];
    int i, j;
    char c;

    if(argc < 2) {
        fprintf(2, "usage: xargs command [args...]\n");
        exit(1);
    }

    for(i = 1; i < argc; i++) {
        newargv[i - 1] = argv[i];
    }

    j = 0;
    while(read(0, &c, 1) == 1) {
        if(c == '\n') {
            buf[j] = 0;

            newargv[argc - 1] = buf;
            newargv[argc] = 0;

            if(fork() == 0) {
                exec(newargv[0], newargv);
                fprintf(2, "exec failed\n");
                exit(1);
            }
            wait(0);
            j = 0;
        } else {
            buf[j++] = c;
        }
    }

    exit(0);
}