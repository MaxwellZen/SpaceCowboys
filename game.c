#include "game.h"
#include "networking.h"

int sd;
int found;
int game_index;
int players[4];
int map[height][width];
char line[1000];
int username_mode;
int result;
char names[4][namelen+1];
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
struct account user;
int mapshown;
char ip[1000];

int main() {
	// connect to server
	printf("Enter an IP address: ");
	fgets(ip, 1000, stdin);
	if (strchr(ip, '\n')) *strchr(ip, '\n')=0;
	sd = client_handshake(ip);
	if (sd==-1) {
		printf("Connection failed\n");
		exit(0);
	}

	// set regular cursor and do signal handling
	curs_set(1);
	signal(SIGINT, INThandler);

	// prompt user for Login or Create Account
	get_username_mode();

	while (1) {
		// read phase
		int phase;
		read(sd, &phase, sizeof(int));

		// phase 1: user enters username
		if (phase==1) {
			curs_set(1);
			get_username();
			// write info packet to server
			writeint(sd, 1);
			writeint(sd, username_mode);
			write(sd, line, 21 * sizeof(char));
			// read result
			read(sd, &result, sizeof(int));
			if (result==0) { // unsuccessful login attempt
				char hold[21];
				strcpy(hold, line);
				if (username_mode==LOGIN) printf("Username does not exist\nWould you like to create an account with this username? (y/n):");
				else if (username_mode==CREATE) printf("Username already exists\nWould you like to login to the account with this username? (y/n):");
				fgets(line, 1000, stdin);
				while (line[0] != 'y' && line[0] != 'Y' && line[0] != 'n' && line[0] != 'N') {
					printf("(y/n): ");
					fgets(line, 1000, stdin);
				}
				if (line[0]=='y' || line[0]=='Y') {
					username_mode = 1 - username_mode;
					read(sd, &phase, sizeof(int));
					writeint(sd, 1);
					writeint(sd, username_mode);
					write(sd, hold, 21 * sizeof(char));
					read(sd, &result, sizeof(int));
					strcpy(user.username, hold);
					get_history();
				}
			} else { // successful login attempt
				strcpy(user.username, line);
				get_history();
			}
		}
		// phase 2: waiting room
		else if (phase==2) {
			curses_setup();
			read(sd, &found, sizeof(int));
			read(sd, names, 4*21*sizeof(char));
			phase2_display();
			curs_set(0);
			refresh();
		}
		// phase 3: set up game
		else if (phase==3) {
			curs_set(0);
			read(sd, &game_index, sizeof(int));
			read(sd, players, 4*sizeof(int));
			read(sd, map, height*width*sizeof(int));
			flashlight[0] = 0; flashlight[1] = 2;
		}
		// phase 4: play game
		else if (phase==4) {
			curs_set(0);
			if (mapshown) {
				for (int i = 0; i < 4; i++) display_square(pos[i][0], pos[i][1]);
				read(sd, pos, 4*2*sizeof(int));
				read(sd, &currenttime, sizeof(int));
				for (int i = 0; i < 4; i++) display_player(i);
				attron(COLOR_PAIR(2));
				mvhline(0, 0, BORDER, width);
				mvhline(height-1, 0, BORDER, width);
				attroff(COLOR_PAIR(2));
				display_messages();
				int ch; while ((ch = getch()) != ERR) {}
				writeint(sd, -1); writeint(sd, -1);
			} else {
				// read positions and time
				read(sd, pos, 4*2*sizeof(int));
				read(sd, &currenttime, sizeof(int));
				// display game
				game_display();
				refresh();
				int x=pos[game_index][0], y=pos[game_index][1];
				if (x==-1) mapshown = 1;
				int ch;
				// read in movement keys
				while ((ch = getch()) != ERR) {
					int nx=x, ny=y;
					if (ch == KEY_LEFT) ny--;
	                else if (ch == KEY_RIGHT) ny++;
	                else if (ch == KEY_UP) nx--;
	                else if (ch == KEY_DOWN) nx++;
					else if (ch == 'z') {flashlight[0] -= 0.5; flashlight[1] -= 0.5;}
					else if (ch == 'x') {flashlight[0] += 0.5; flashlight[1] += 0.5;}
					if (x != -1 && map[nx][ny] >= 0) x=nx, y=ny;
				}
				// write movement information to server
				writeint(sd, x); writeint(sd, y);
			}
		}
		// phase 5: game over
	    else if (phase==5) {
			curs_set(1);
			undo_curses_setup();
			read(sd, timedied, 4*sizeof(int));
			mapshown = 0;
			display_results();
			close(sd); // close socket
			// option to restart game
			printf("Continue to new game? (y/n): ");
			fgets(line, 1000, stdin);
			while (line[0] != 'y' && line[0] != 'Y' && line[0] != 'n' && line[0] != 'N') {
				printf("(y/n): ");
				fgets(line, 1000, stdin);
			}
			if (line[0]=='y' || line[0]=='Y') {
				// if restarting game, then start new connection and manually simulate phase 1
				printf("Entering new game:\n");
				sd = client_handshake(ip);
				read(sd, &phase, sizeof(int));
				writeint(sd, 1);
				writeint(sd, LOGIN);
				write(sd, user.username, 21 * sizeof(char));
				read(sd, &result, sizeof(int));
				get_history();
			} else {
				// else, exit program
				printf("Thank you for playing!\n");
				exit(0);
			}
	    }
	}

	return 0;
}

// prompt user for "Login" or "Create Account"
void get_username_mode() {
	printf(YEL BRIGHT REV "%s %s   SPACE COWBOYS   %s %s\n\n" RESET, hider, seeker, seeker, hider);
	printf(YEL "Welcome!! Enter your information below. \n\n" RESET);
	printf(GRN "Type 'Login' or 'Create Account': " RESET);
	fgets(line, 1000, stdin);
	while (strcmp(line, "Login\n") && strcmp(line, "Create Account\n")) {
		printf("Invalid command\n");
		printf(GRN "Type 'Login' or 'Create Account': " RESET);
		fgets(line, 1000, stdin);
	}
	if (strcmp(line, "Login\n")==0) username_mode = LOGIN;
	else username_mode = CREATE;
}

// prompt user for username
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

// read in user game history
void get_history() {
	read(sd, &user.numgames, sizeof(int));
	free(user.history);
	user.history = calloc(user.numgames, sizeof(struct past_game));
	read(sd, user.history, user.numgames * sizeof(struct past_game));
}

// setup ncurses mode
void curses_setup() {
	curs_set(0);
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

// exit ncurses mode
void undo_curses_setup() {
	curs_set(1);
	echo();
	nocbreak();
	endwin();
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
}

// print a game (for game history)
void print_game(struct past_game game) {
	struct tm tm = game.date;
	printw("%02d-%02d %02d:%02d:%02d | ", tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	if (game.role) {
		printw("SEEKER | ");
		if (game.result==-1) printw("LOST");
		else printw("WON IN %d SECONDS", game.result);
	} else {
		printw("HIDER  | ");
		if (game.result==-1) printw("SURVIVED");
		else printw("DIED IN %d SECONDS", game.result);
	}
}

// display phase 2 waiting room screen
void phase2_display() {
	clear();
	for (int i = 0; i < 6; i++) {
		move(i+1, 1);
		attron(COLOR_PAIR(3));
		printw("%s", title[i]);
		attroff(COLOR_PAIR(3));
	}
	move(8, 5);
	printw("%d / 4 players ready", found);
	move(10, 5);
	printw("Players:");
	for (int i = 0; i < 4; i++) {
		move(11+i, 5);
		printw("%s", names[i]);
	}
	move(8, 32);
	printw("Previous games:");
	if (user.numgames>0) {
		int i = 0; int offset = 0;
		if (user.numgames > 12) offset = user.numgames - 12;
		for (; i + offset < user.numgames; i++) {
			move(10+i, 32);
			print_game(user.history[i]);
		}
	} else {
		move(10, 32);
		printw("No games played");
	}
}

// check if coordinates are within radius of player coordinates
int in_radius(double x, double y) {
	x -= pos[game_index][0];
	y -= pos[game_index][1];
	y /= 2;
	return pow(x,2)+pow(y,2) <= pow(radius,2);
}
// check if coordinates are within correct angle of player coordinates
int in_flashlight(double x, double y) {
	x -= pos[game_index][0];
	y -= pos[game_index][1];
	y /= 2;
	return x*cos(flashlight[0]) >= y*sin(flashlight[0]) && x*cos(flashlight[1]) >= y*sin(flashlight[1]);
}

// display one square of the board
void display_square(int x, int y) {
	if (map[x][y]==-2) {
		attron(COLOR_PAIR(1));
		mvaddch(x, y, OBSTACLE);
		attroff(COLOR_PAIR(1));
	}
	else if (map[x][y]==-1) {
		attron(COLOR_PAIR(2));
		mvaddch(x, y, BORDER);
		attroff(COLOR_PAIR(2));
	}
	else if (map[x][y]%2==0) mvaddch(x, y, FLOOR1);
	else mvaddch(x, y, FLOOR2);
}
// display one player
void display_player(int i) {
	if (players[i]) mvaddch(pos[i][0], pos[i][1], 'X');
	else mvaddch(pos[i][0], pos[i][1], 'O');
}
// display game messages
void display_messages() {
	// display time
	move(0, 2);
	printw("Time Left: %d s", gametime - currenttime);

	// display role
	char id[20];
	if (players[game_index]) strcpy(id, "You are a SEEKER");
	else if (pos[game_index][0]==-1) strcpy(id, "You are DEAD");
	else strcpy(id, "You are a HIDER");
	move(height-1, (width - strlen(id)) / 2);
	printw("%s", id);

	// display number of hiders left
	id[0] = 0;
	int alive = 0;
	for (int i = 0; i < 4; i++) if (pos[i][0] != -1) alive++;
	sprintf(id, "%d Hiders Left", alive-1);
	move(0, 65);
	printw("%s", id);
}
// display game screen
void game_display() {
	clear();
	int x, y;

	// display environment
	for (x = 0; x < height; x ++) {
		for (y = 0; y < width; y ++) {
			if (pos[game_index][0]==-1 || (in_radius(x, y) && in_flashlight(x, y))) {
				display_square(x, y);
			}
		}
	}

	// display players
	for (int i = 0; i < 4; i++) {
		if (pos[game_index][0]==-1 || (in_radius(pos[i][0], pos[i][1]) && in_flashlight(pos[i][0], pos[i][1]))) {
			display_player(i);
		}
	}

	display_messages();
}
// display game results
void display_results() {
	// determine who won
	char winner[] = "Seeker";
	int seekerwon = 1;
	for (int i = 1; i < 4; i ++) {
		if (players[i]==0 && timedied[i] == -1) {
			strcpy(winner, "Hiders");
			seekerwon = 0;
			break;
		}
	}

	// print game information
	printf(YEL BRIGHT REV "\nGame Over! The %s won!\n\n" RESET, winner);
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
}

// handle ctrl+c signal
void INThandler(int sig) {
	curs_set(1);
	endwin();
	close(sd);
	exit(0);
}
