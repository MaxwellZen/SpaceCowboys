// #include "networking.h"
#include <curses.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define OBSTACLE '@'
#define BORDER '#'

void game_setup() {
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    clear();

    srand( time(NULL) );

    // Creating border
    int x, y;
    int up, down, left, right;

    up = 0;
    down = 25;
    left = 2;
    right = 65;

    // map[y][x]
    int map[down + 1][right + 1];

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

    for (y = up; y < down; y ++) {
      mvvline(y, left, BORDER, 1);
      mvvline(y, right, BORDER, 1);
      map[y][left] = -1;
      map[y][right] = -1;
    }

    for (x = left; x <= right; x ++) {
      mvvline(up, x, BORDER, 1);
      mvvline(down, x, BORDER, 1);
      map[up][x] = -1;
      map[down][x] = -1;
    }

}

int main() {
    // connect to server
    // int fd = ;
    // while (1) {
        int phase = 3; //read phase
        if (phase==1) {
          char line[20];

          printf("Hide & Seek\nWelcome Screen! Blah Blah\n");
          printf("Type 'Login' or 'Create Account': ");

          while(fgets(line, 20, stdin)) {
            if (strcmp(line, "Login\n") == 0) {
              printf("\nUsername: ");
              fgets(line, 20, stdin);
            }
            else if (strcmp(line, "Create Account\n") == 0) {
              printf("\nNew Username: ");
              fgets(line, 20, stdin);
            }
            else {
              printf("Invalid command\n");
              printf("Type 'Login' or 'Create Account': ");
            }
          }
          // Check if username is valid.. at users.txt?
          phase = 2; // if username is valid
        }
    //     else if (phase==2) {
    //
    //     }
        else if (phase==3) {
          game_setup();
          refresh();
          char ch = getch();
          while (ch != 'q') {
          }

          // Restores terminal, exits game
          endwin();
          exit(0);
        }
    //     else if (phase==4) {
    //
    //     }
    //     else if (phase==5) {
    //
    //     }
    // }

    return 0;
}
