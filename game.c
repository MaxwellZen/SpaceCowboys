#include "game.h"
#include "networking.h"

int sd;
int game_index;
int players[4];
int map[height][width];
char line[1000];
int username_mode;
char names[4][namelen+1];

int main() {
	// connect to server
	sd = client_handshake();
	// ncurses setup
	nodelay(stdscr, TRUE);

	get_username_mode();
	printf("username_mode: %d", username_mode);
	
	while (1) {
		int phase;
		read(sd, &phase, sizeof(int));
		printf("Phase: %d\n", phase);
		if (phase==1) {
			get_username();
			writeint(sd, 1);
			writeint(sd, username_mode);
			write(sd, line, (namelen+1) * sizeof(char));
			int result = 696969;
			read(sd, &result, sizeof(int));
			printf("Results: %d\n", result);
			if (result==0) {
				if (username_mode==LOGIN) printf("Username does not exist\n");
				else if (username_mode==CREATE) printf("Username already exists\n");
			}
		}
		else if (phase==2) {
			for (int i = 0; i < 4; i++) read(sd, names[i], (namelen+1) * sizeof(char));
			printf("Waiting Room:\n");
			for (int i = 0; i < 4; i++) {
				printf("%s\n", names[i]);
			}
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

void get_username_mode() {
	printf("Hide & Seek\nWelcome Screen! Blah Blah\n");
	printf("Type 'Login' or 'Create Account': ");
	fgets(line, 1000, stdin);
	while (strcmp(line, "Login\n") && strcmp(line, "Create Account\n")) {
		printf("Invalid command\n");
		printf("Type 'Login' or 'Create Account': ");
		fgets(line, 1000, stdin);
	}
	if (strcmp(line, "Login\n")==0) username_mode = LOGIN;
	else username_mode = CREATE;
}

void get_username() {
	do {
		if (username_mode==LOGIN) printf("\nUsername: ");
		else if (username_mode==CREATE) printf("\nNew Username: ");
		fgets(line, 1000, stdin);
		if (strlen(line)==1 || strlen(line)>21) {
			printf("Username must be between 1 and 20 characters long\n");
		}
	} while (strlen(line)==1 || strlen(line)>21);
	*strchr(line, '\n') = 0;
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
