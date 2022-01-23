#include "game.h"
#include "networking.h"

int sd;
int found;
int game_index;
int players[4];
int map[height][width];
char line[1000];
int username_mode;
char names[4][namelen+1];
char username[namelen+1];
int pos[4][2];
int currenttime;
int timedied[4];
double flashlight[2];
char title[6][79] = {
"   _____ ____  ___   ____________   __________ _       ______  ______  _______",
"  / ___// __ \\/   | / ____/ ____/  / ____/ __ \\ |     / / __ )/ __ \\ \\/ / ___/",
"  \\__ \\/ /_/ / /| |/ /   / __/    / /   / / / / | /| / / __  / / / /\\  /\\__ \\ ",
" ___/ / ____/ ___ / /___/ /___   / /___/ /_/ /| |/ |/ / /_/ / /_/ / / /___/ / ",
"/____/_/   /_/  |_\\____/_____/   \\____/\\____/ |__/|__/_____/\\____/ /_//____/  ",
"                                                                              "};

int main() {
	// connect to server
	sd = client_handshake();
	// printf("%d\n", sd);
	if (sd==-1) {
		printf("Connection failed\n");
		exit(0);
	}
	// ncurses setup
	// Bring cursor back, checking for control c
	printf("\e[?25h");
	signal(SIGINT, INThandler);

	get_username_mode();

	while (1) {
		int phase;
		read(sd, &phase, sizeof(int));
		// printf("Phase: %d\n", phase);
		if (phase==1) {
			curs_set(1);
			get_username();
			writeint(sd, 1);
			writeint(sd, username_mode);
			write(sd, line, 21 * sizeof(char));
			int result = 696969;
			read(sd, &result, sizeof(int));
			// printf("Results: %d\n", result);
			if (result==0) {
				if (username_mode==LOGIN) printf("Username does not exist\n");
				else if (username_mode==CREATE) printf("Username already exists\n");
			} else {
				strcpy(username, line);
				curses_setup();
			}
		}
		else if (phase==2) {
			curs_set(0);
			read(sd, &found, sizeof(int));
			read(sd, names, 4*21*sizeof(char));
			phase2_display();
			refresh();
		}
		else if (phase==3) {
			curs_set(0);
			read(sd, &game_index, sizeof(int));
			read(sd, players, 4*sizeof(int));
			read(sd, map, height*width*sizeof(int));
			flashlight[0] = 0; flashlight[1] = 2;
		}
		else if (phase==4) {
			curs_set(0);
			read(sd, pos, 4*2*sizeof(int));
			read(sd, &currenttime, sizeof(int));
			game_display();
			refresh();
			int y = 1, x = 3;
			int py = 1, px = 3;
			int dx=0, dy=0;
			int ch;
			char ghost[] = "\U0001F47B";
			char s[] = { 0xf0, 0x9f, 0x98, 0x8e, 0};
			while ((ch = getch()) != ERR) {
				if (ch == KEY_LEFT) dy--;
                else if (ch == KEY_RIGHT) dy++;
                else if (ch == KEY_UP) dx--;
                else if (ch == KEY_DOWN) dx++;
				else if (ch == 'z') {flashlight[0] -= 0.5; flashlight[1] -= 0.5;}
				else if (ch == 'x') {flashlight[0] += 0.5; flashlight[1] += 0.5;}
			}
			write(sd, &dx, sizeof(int));
			write(sd, &dy, sizeof(int));
		}
	    else if (phase==5) {
			curs_set(1);
			undo_curses_setup();
			read(sd, timedied, 4*sizeof(int));
			char winner[] = "Seeker";
			int seekerwon = 1;
			for (int i = 1; i < 4; i ++) {
				if (players[i]==0 && timedied[i] == -1) {
					strcpy(winner, "Hiders");
					seekerwon = 0;
					break;
				}
			}

			printf(YEL BRIGHT REV "Game Over! The %s won!\n\n" RESET, winner);
			printf(GRN "Stats: \n" RESET);

			for (int i = 0; i < 4; i ++) {
				if (players[i]) {
					printf("Player %d (Seeker): ", i);
					if (seekerwon) printf("WON\n");
					else printf("LOST\n");
				} else {
					if (timedied[i] != -1) printf("Player %d: %d seconds\n", i, timedied[i]);
					else printf("Player %d: SURVIVED\n", i);
				}
			}
			close(sd);
			printf("Continue to new game? (y/n): ");
			fgets(line, 1000, stdin);
			while (line[0] != 'y' && line[0] != 'Y' && line[0] != 'n' && line[0] != 'N') {
				printf("(y/n): ");
				fgets(line, 1000, stdin);
			}
			if (line[0]=='y' || line[0]=='Y') {
				printf("Entering new game:\n");
				sd = client_handshake();
				read(sd, &phase, sizeof(int));
				writeint(sd, 1);
				writeint(sd, LOGIN);
				write(sd, username, 21 * sizeof(char));
				int result;
				read(sd, &result, sizeof(int));
				printf("Waiting for users...\n");
			} else {
				printf("Thank you for playing!\n");
				exit(0);
			}
	    }
	}

	return 0;
}

void get_username_mode() {
	printf(YEL BRIGHT REV "%s %s   SPACE COWBOYS   %s %s\n\n" RESET, hider, seeker, seeker, hider);
	printf(YEL "Welcome!! Enter your information below. \n\n" RESET);
	printf(GRN "Type 'Login' or 'Create Account': " RESET);
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

void curses_setup() {
	initscr();
	keypad(stdscr, TRUE);
	cbreak();
	noecho();
	clear();
	nodelay(stdscr, TRUE);

	// Adding color
	start_color();
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
}

void undo_curses_setup() {
	echo();
	nocbreak();
	endwin();
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
}

void phase2_display() {
	clear();
	for (int i = 0; i < 6; i++) {
		move(i+1, 1);
		attron(COLOR_PAIR(3));
		printw("%s", title[i]);
		attroff(COLOR_PAIR(3));
	}
	move(8, 5);
	printw("Waiting for players...");
	move(9, 5);
	printw("%d / 4 players ready", found);
	move(11, 5);
	printw("Players:");
	for (int i = 0; i < 4; i++) {
		move(12+i, 5);
		printw("%s", names[i]);
	}
}

int in_radius(double x, double y) {
	x -= pos[game_index][0];
	y -= pos[game_index][1];
	y /= 2;
	return pow(x,2)+pow(y,2) <= pow(radius,2);
}

int in_flashlight(double x, double y) {
	x -= pos[game_index][0];
	y -= pos[game_index][1];
	y /= 2;
	return x*cos(flashlight[0]) >= y*sin(flashlight[0]) && x*cos(flashlight[1]) >= y*sin(flashlight[1]);
}

void game_display() {
	clear();
	// Creating border
	int x, y;

	// Creating obstacles
	for (x = 0; x < height; x ++) {
		for (y = 0; y < width; y ++) {
			if (pos[game_index][0]==-1 || (in_radius(x, y) && in_flashlight(x, y))) {
				if (map[x][y]==-2) {
					attron(COLOR_PAIR(1));
					mvvline(x, y, OBSTACLE, 1);
					attroff(COLOR_PAIR(1));
				}
				else if (map[x][y]==-1) {
					attron(COLOR_PAIR(2));
					mvvline(x, y, BORDER, 1);
					attroff(COLOR_PAIR(2));
				}
				else if (map[x][y]%2==0) mvvline(x, y, FLOOR1, 1);
				else mvvline(x, y, FLOOR2, 1);
			}
		}
	}
	for (int i = 0; i < 4; i++) {
		if (pos[game_index][0]==-1 || (in_radius(pos[i][0], pos[i][1]) && in_flashlight(pos[i][0], pos[i][1]))) {
			if (players[i]) mvaddch(pos[i][0], pos[i][1], 'X');
			else mvaddch(pos[i][0], pos[i][1], 'O');
		}
	}
	// display time
	move(0, 2);
	printw("Time Left: %d s", gametime - currenttime);
	char id[20];
	if (players[game_index]) strcpy(id, "You are a SEEKER");
	else if (pos[game_index][0]==-1) strcpy(id, "You are DEAD");
	else strcpy(id, "You are a HIDER");
	move(height-1, (width - strlen(id)) / 2);
	printw("%s", id);
	id[0] = 0;
	int alive = 0;
	for (int i = 0; i < 4; i++) if (pos[i][0] != -1) alive++;
	sprintf(id, "%d Hiders Left", alive-1);
	move(0, 65);
	printw("%s", id);
}



void INThandler(int sig) {
	printf("\e[?25h");
	endwin();
	close(sd);
	exit(0);
}
