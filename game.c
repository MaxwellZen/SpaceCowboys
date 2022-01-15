// #include "networking.h"
#include <curses.h>
#include <time.h>
#include <stdlib.h>


void game_setup() {
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    clear();

    // srand( time(NULL) );



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
    char ch = getch();
    game_setup();
    while (ch != 'q') {
    }
    endwin();
    exit(0);

    return 0;
}
