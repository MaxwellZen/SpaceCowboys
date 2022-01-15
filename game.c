// #include "networking.h"
#include <curses.h>
#include <time.h>
#include <stdlib.h>

#define OBSTACLE '@'
#define BORDER '#'

void game_setup() {
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    clear();

    srand( time(NULL) );

    // map[y][x]
    int map[50][70];


    // Creating border
    int x, y;
    // Dimensions of the rectangle map: up, down, left, right
    int up, down, left, right;

    up = 0;
    down = 25;
    left = 2;
    right = 65;

    for (y = up; y < down; y ++) {
      mvvline(y, left, BORDER, 1);
      mvvline(y, right, BORDER, 1);
    }

    for (x = left; x <= right; x ++) {
      mvvline(up, x, BORDER, 1);
      mvvline(down, x, BORDER, 1);
    }

    // Creating obstacles
    for (x = left; x < right; x ++) {
      for (y = up; y < down; y ++) {
        if (rand() % 9 == 0) {
          map[y][x] = -2;
          mvvline(y, x, OBSTACLE, 1);
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
