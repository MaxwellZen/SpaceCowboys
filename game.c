// #include "networking.h"
#include <curses.h>
#include <time.h>
#include <stdlib.h>

#define HI '@'

void game_setup() {
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    clear();

    srand( time(NULL) );

    // map[y][x]
    int map[50][70];

    // Creating obstacles
    int x, y;
    for (x = 5; x < 70; x ++) {
      for (y = 2; y < 50; y ++) {
        if (rand() % 9 == 0) {
          map[y][x] = -2;
          mvvline(y, x, HI, 1);
        }
        else map[y][x] = 10 + (rand() % 50);
      }
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
