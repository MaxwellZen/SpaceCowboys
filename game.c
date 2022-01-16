#include "game.h"
#include "networking.h"

int sd;
int game_index;
int players[4];
int map[height][width];
char line[1000];

int main() {
	// connect to server
	sd = client_handshake();
	// ncurses setup
	nodelay(stdscr, TRUE);

	printf("Hide & Seek\nWelcome Screen! Blah Blah\n");
	printf("Type 'Login' or 'Create Account': ");
	fgets(line, 1000, stdin);
	while (strcmp(line, "Login\n") && strcmp(line, "Create Account\n")) {
		printf("Invalid command\n");
		printf("Type 'Login' or 'Create Account': ");
		fgets(line, 1000, stdin);
	}

	while (1) {
		int phase = 4;
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
			int x = 0, y = 0;
            mvaddch(y, x, 'P');
			int ch;
			while ((ch = getch()) != ERR) {
				if (ch == KEY_LEFT) y -= 1;
                else if (ch == KEY_RIGHT) y += 1;
                else if (ch == KEY_UP) x -= 1;
                else if (ch == KEY_DOWN) x += 1;
                else if (ch == 'q') {
					// Restores terminal, exits game
					endwin();
				}
                mvaddch(y, x, 'P');
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
	while(fgets(line, 1000, stdin)) {
		if (strcmp(line, "Login\n") == 0) {
			printf("\nUsername: ");
			fgets(line, 1000, stdin);
		}
		else if (strcmp(line, "Create Account\n") == 0) {
			printf("\nNew Username: ");
			fgets(line, 1000, stdin);
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
}

void game_display() {
	// Creating border
	int x, y;

	// Creating obstacles
	for (x = 0; x < height; x ++) {
		for (y = 0; y < width; y ++) {
			if (map[x][y]==-2) mvvline(x, y, OBSTACLE, 1);
			else if (map[x][y]==-2) mvvline(x, y, BORDER, 1);
			else if (map[x][y]%2==0) mvvline(x, y, FLOOR1, 1);
			else mvvline(x, y, FLOOR2, 1);
		}
	}
}
