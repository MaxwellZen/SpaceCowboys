#include "game.h"
#include "networking.h"

int sd;
int game_index;
int players[4];
int map[height][width];

int main() {
    // connect to server
    sd = client_handshake();
	// ncurses setup
	nodelay(stdscr, TRUE);
    while (1) {
		int phase;
		read(sd, &phase, sizeof(int));
        if (phase==1) {
          get_username();
        }
        else if (phase==2) {

        }
        else if (phase==3) {
          read(sd, &game_index, sizeof(int));
          read(sd, players, sizeof(players));
          read(sd, map, sizeof(map));
        }
        else if (phase==4) {
		  int pos[4][2];
		  read(sd, pos, sizeof(pos));
          game_setup();
          refresh();
          int x, y;
		  int ch;
		  while ((ch = getch()) != ERR) {
            if (ch == KEY_LEFT) {
              x = 0;
              y = -1;
            } else if (ch == KEY_RIGHT) {
              x = 0;
              y = 1;
            } else if (ch == KEY_UP) {
              x = -1;
              y = 0;
            } else if (ch == KEY_DOWN) {
              x = 1;
              y = 0;
		  	} else if (ch == 'q') {
              // Restores terminal, exits game
              endwin();
              exit(0);
            }
		  }
          write(sd, &x, sizeof(int));
          write(sd, &y, sizeof(int));
        }
    //     else if (phase==5) {
    //
    //     }
    }

    return 0;
}

void get_username() {
  char line[100];

  printf("Hide & Seek\nWelcome Screen! Blah Blah\n");
  printf("Type 'Login' or 'Create Account': ");

  while(fgets(line, 100, stdin)) {
    if (strcmp(line, "Login\n") == 0) {
      printf("\nUsername: ");
      fgets(line, 100, stdin);
      // check_username(line, 0);
      // if (check_username(line) == -1) {
      //   printf("Username does not exist\nUsername: ");
      // }
    }
    else if (strcmp(line, "Create Account\n") == 0) {
      printf("\nNew Username: ");
      fgets(line, 100, stdin);
      // check_username(line, 1);
      // if (check_username(line) == -1) {
        // add_username(line);
      // }
    }
    else {
      printf("Invalid command\n");
      printf("Type 'Login' or 'Create Account': ");
    }
  }
}

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
