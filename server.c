#include "networking.h"

int main() {
    // set up listener

    while (1) {
        int fds[4];
        for (int i = 0; i < 4; i++) {
            // connect socket
            fds[i] =;
        }

        int f = fork();
        if (f) {
            for (int i = 0; i < 4; i++) close(fds[i]);
        } else {
            // do game stuff
            exit(0);
        }
    }
}
