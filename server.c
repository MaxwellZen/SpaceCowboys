#include "networking.h"

int fds[4];
int phase[4];
char names[4][21];
int found;

void process(int i);

void phase1(int i);
void phase2(int i);
void phase3(int i);
void phase4(int i);
void phase5(int i);

int main() {
    int listener = server_setup();
    while (1) {
        for (int i = 0; i < 4; i++) {
            phase[i] = 0;
            names[i][0] = 0;
        }
        fd_set read_fds;
        int max_descriptor = listener;
        found = 0;

        while (1) {
            FD_ZERO(&read_fds);
            FD_SET(listener, &read_fds);
            for (int i = 0; i < found; i++) FD_SET(fds[i], &read_fds);
            select(max_descriptor+1, &read_fds, NULL, NULL, NULL);
            for (int i = 0; i < found; i++) {
                if (FD_ISSET(fds[i], &read_fds)) {
                    int g = fork();
                    if (!g) {
                        process(i);
                        exit(0);
                    }
                }
            }
            if (FD_ISSET(listener, &read_fds)) {
                fds[found] = server_connect(listener);
                if (fds[found] > max_descriptor) max_descriptor = fds[found];
                phase[found] = 1;
                strcpy(names[found], "Setting username...");
                found++;
                printf("Found %d clients\n", found);
            }
            if (found==4) break;
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

void process(int i) {
    if (phase[i]==0) return;
    else if (phase[i]==1) phase1(i);
    else if (phase[i]==2) phase2(i);
    else if (phase[i]==3) phase3(i);
    else if (phase[i]==4) phase4(i);
    else if (phase[i]==5) phase5(i);
}


void phase1(int i) {

}
void phase2(int i) {
    int x = 2;
    write(fds[i], &x, sizeof(int));
    write(fds[i], &found, sizeof(int));
    write(fds[i], names, 4*21*sizeof(char));
}
void phase3(int i) {

}
void phase4(int i) {

}
void phase5(int i) {

}
