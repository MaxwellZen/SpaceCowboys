#include "game.h"
#include "networking.h"

int sd;
int game_index;
int players[4];
int map[height][width];
char line[1000];
int username_mode;
char names[4][namelen+1];
int pos[4][2];
int currenttime;
int timedied[4];
double flashlight[2];

int main() {
	// connect to server
	sd = client_handshake();
	printf("%d\n", sd);
	if (sd==-1) {
		printf("Connection failed\n");
		exit(0);
	}
	// ncurses setup
	// Bring cursor back, checking for control c
	printf("\e[?25h");
	signal(SIGINT, INThandler);

	get_username_mode();
	printf("username_mode: %d\n", username_mode);

	while (1) {
		int phase;
		read(sd, &phase, sizeof(int));
		// printf("Phase: %d\n", phase);
		if (phase==1) {
			printf("\e[?25h");
			get_username();
			writeint(sd, 1);
			writeint(sd, username_mode);
			write(sd, line, 21 * sizeof(char));
			int result = 696969;
			read(sd, &result, sizeof(int));
			printf("Results: %d\n", result);
			if (result==0) {
				if (username_mode==LOGIN) printf("Username does not exist\n");
				else if (username_mode==CREATE) printf("Username already exists\n");
			} else {
				printf("Waiting for users...\n");
			}
		}
		else if (phase==2) {
			printf("\e[?25h");
			for (int i = 0; i < 4; i++) for (int j = 0; j < 21; j++) read(sd, &names[i][j], sizeof(char));
			// for (int i = 0; i < 4; i++) read(sd, names[i], (namelen+1) * sizeof(char));
			// printf("Waiting Room:\n");
			// for (int i = 0; i < 4; i++) {
			// 	printf("[%s]\n", names[i]);
			// }
		}
		else if (phase==3) {
			printf("\e[?25h");
			read(sd, &game_index, sizeof(int));
			read(sd, players, 4*sizeof(int));
			read(sd, map, height*width*sizeof(int));
			flashlight[0] = 0; flashlight[1] = 2;
		}
		else if (phase==4) {
			printf("\e[?25l");
			read(sd, pos, 4*2*sizeof(int));
			read(sd, &currenttime, sizeof(int));
			game_setup();
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
			endwin();
			printf("\e[?25h");
			read(sd, timedied, 4*sizeof(int));
			char winner[] = "Seeker";
			for (int i = 1; i < 4; i ++) {
				if (players[i]==0 && timedied[i] == -1) strcpy(winner, "Hiders");
			}

			printf(YEL BRIGHT REV "Game Over! The %s won!\n\n" RESET, winner);
			printf(GRN "Stats: \n" RESET);

			if (strcmp(winner, "Seeker") == 0) printf("Player 0 (Seeker): WON\n");
			else printf("Player 0 (Seeker): LOST\n");

			for (int i = 1; i < 4; i ++) {
				if (timedied[i] != -1) printf("Player %d: %d seconds\n", i, timedied[i]);
				else printf("Player %d: SURVIVED\n", i);
			}
			exit(0);
	    }
	}

	return 0;
}

void get_username_mode() {
	printf(YEL BRIGHT REV "%s %s   Hide & Seek   %s %s\n\n" RESET, hider, seeker, seeker, hider);
	printf(YEL "Welcome to SPACE COWBOYS!! \n\n" RESET);
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

void game_setup() {
	initscr();
	keypad(stdscr, TRUE);
	cbreak();
	noecho();
	clear();
	nodelay(stdscr, TRUE);
	srand( time(NULL) );
	// for (int i = 0; i < height; i++) map[i][0] = map[i][width-1] = -1;
	// for (int i = 0; i < width; i++) map[0][i] = map[height-1][i] = -1;
	// for (int i = 1; i < height-1; i++) for (int j = 1; j < width-1; j++) if (rand()%9==0) map[i][j]=-2; else map[i][j] = rand()%10;
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
	// Creating border
	int x, y;

	// Adding color
	start_color();
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);

	// Creating obstacles
	for (x = 0; x < height; x ++) {
		for (y = 0; y < width; y ++) {
			if (in_radius(x, y) && in_flashlight(x, y)) {
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
		if (in_radius(pos[i][0], pos[i][1]) && in_flashlight(pos[i][0], pos[i][1])) {
			if (players[i]) mvaddch(pos[i][0], pos[i][1], 'X');
			else mvaddch(pos[i][0], pos[i][1], 'O');
		}
	}
	// mvaddch(pos[game_index][0], pos[game_index][1], 'O');
	// display time
	move(0, 0);
	printw("Time Left: %d s", gametime - currenttime);
}



void INThandler(int sig) {
	printf("\e[?25h");
	endwin();
	exit(0);
}
