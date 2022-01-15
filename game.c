// #include "networking.h"
#include <curses.h>
#include <time.h>
#include <stdlib.h>

#define HI  	  '@'

void game_setup() {
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    clear();

    srand( time(NULL) );

    // Creating obstacles

    int x, y;
    for (x = 10; x < 50; x ++) {
        if (rand() % 5 == 0) mvvline(10, x, HI, LINES / 5);
    }

}

int main() {
    // connect to server
    // int fd = ;
    // while (1) {
    //     int phase = ;//read phase
    //     if (phase==1) {
    //
    //     }
    //     else if (phase==2) {
    //
    //     }
    //     else if (phase==3) {
    //
    //     }
    //     else if (phase==4) {
    //
    //     }
    //     else if (phase==5) {
    //
    //     }
    // }

    game_setup();
    refresh();
    char ch = getch();
    while (ch != 'q') {
    }

    // Restores terminal, exits game
    endwin();
    exit(0);

    return 0;
}
