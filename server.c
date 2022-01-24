#include "networking.h"

int listener;
int fds[4];
int phase[4];
char names[4][21];
int found, ready;
int map[height][width];
int pos[4][2];
int seeker;
int isseeker[4];
int alive[4];
int timedied[4];
time_t starttime;
struct tm starttm;
fd_set read_fds;

void process(int i);
void gamesetup();

void phase1(int i);
void phase2();
void phase3();
void phase4();
void phase5();

void load_usernames();
void add_username(char * line);
int user_exists(char * line);
int check_username(char * line, int n);

void INThandler(int sig);

struct account *users;
int num_users;

int main() {
	// signal handling, create listener socket
	signal(SIGINT, INThandler);
	listener = server_setup();
	if (listener==-1) {
		printf("Listener failed\n");
		exit(0);
	}

	while (1) {
		// initialize values
		for (int i = 0; i < 4; i++) {
			fds[i] = 0;
			phase[i] = 0;
			strcpy(names[i], "Waiting...");
		}
		int max_descriptor = listener;
		found = ready = 0;

		while (found<4) { // while haven't had 4 connections
			// intialize read_fds
			FD_ZERO(&read_fds);
			FD_SET(listener, &read_fds);
			for (int i = 0; i < found; i++) FD_SET(fds[i], &read_fds);
			// select
			select(max_descriptor+1, &read_fds, NULL, NULL, NULL);
			// process clients that need to be processed
			for (int i = 0; i < found; i++) {
				if (FD_ISSET(fds[i], &read_fds)) {
					phase1(i);
				}
			}
			// connect to client if listener needs to be processed
			if (FD_ISSET(listener, &read_fds)) {
				fds[found] = server_connect(listener);
				if (fds[found]==-1) {
					printf("Failed to connect to client\n");
					continue;
				}
				writeint(fds[found], 1);
				if (fds[found] > max_descriptor) max_descriptor = fds[found];
				phase[found] = 1;
				strcpy(names[found], "Setting username...");
				found++;
				phase2();
				printf("Found %d clients\n", found);
			}
		}

		// fork to separate process
		int f = fork();
		if (f) {
			for (int i = 0; i < 4; i++) {
				close(fds[i]);
				fds[i] = 0;
			}
		} else {
			close(listener);
			// wait for everyone to finish entering their usernames
			while (ready<4) {
				FD_ZERO(&read_fds);
				for (int i = 0; i < 4; i++) if (phase[i]==1) FD_SET(fds[i], &read_fds);
				select(max_descriptor+1, &read_fds, NULL, NULL, NULL);
				for (int i = 0; i < 4; i++) if (phase[i]==1 && FD_ISSET(fds[i], &read_fds)) {
					phase1(i);
				}
			}
			// execute phase 3
			for (int i = 0; i < 4; i++) phase[i]=3;
			gamesetup();
			phase3();
			// execute phase 4
			while (phase[0]==4) {
				usleep(50000);
				phase4();
			}
			// execute phase 5
			phase5();
			exit(0);
		}
	}
}

// set up game information
void gamesetup() {
	srand(time(NULL));
	// create seeker info
	for (int i = 0; i < 4; i++) isseeker[i] = 0;
	seeker = rand()%4;
	isseeker[seeker] = 1;
	for (int i = 0; i < 4; i++) alive[i] = 1;
	starttime = time(NULL);
	// create map
	for (int i = 0; i < width; i++) map[0][i] = map[height-1][i] = -1;
	for (int i = 0; i < height; i++) map[i][0] = map[i][width-1] = -1;
	for (int i = 1; i < height-1; i++) {
		for (int j = 1; j < width-1; j++) {
			map[i][j] = 50 + (rand() % 30);
			if (rand()%9 == 0) map[i][j] = -2;
		}
	}
	for (int i = 0; i < 4; i ++) timedied[i] = -1;
	for (int i = 0; i < 4; i++) {
		pos[i][0] = 3; pos[i][1] = 3;
		if (i&1) pos[i][0] = height - 4;
		if (i&2) pos[i][1] = width - 4;
		map[pos[i][0]][pos[i][1]] = 50 + (rand() % 30);
	}
}
// phase 1 server communication
void phase1(int i) {
	// first int is 1 if username has been updated
	int x;
	read(fds[i], &x, sizeof(int));
	if (x==0) return;
	// second int indicates login or create account mode
	int n;
	read(fds[i], &n, sizeof(int));
	// 21 characters indicate suggested username
	char line[namelen+1];
	line[namelen]=0;
	read(fds[i], line, (namelen+1) * sizeof(char));
	// check if username matches the login / create account status
	if (check_username(line, n)) {
		// update variables to indicate new username
		strcpy(names[i], line);
		phase[i] = 2;
		ready++;
		// communicate success
		writeint(fds[i], 1);
		// find and transmit game history
		load_usernames();
		int index = -1;
		for (int j = 0; j < num_users; j++) if (!strcmp(names[i], users[j].username)) index = j;
		writeint(fds[i], users[index].numgames);
		if (users[index].numgames>0) write(fds[i], users[index].history, users[index].numgames * sizeof(struct past_game));
		// update usernames for everyone
		phase2();
	} else {
		// communicate failure
		writeint(fds[i], 0);
		// prompt for phase 1 input
		writeint(fds[i], 1);
	}
}
// phase 2 server communication
void phase2() {
	// printf("Phase 2 usernames:\n");
	// for (int i = 0 ; i < 4;i ++) printf("[%s]\n", names[i]);
	// send # of ready players, and usernames of each player to clients in phase 2
	for (int i = 0; i < 4; i++) if (phase[i]==2) {
		writeint(fds[i], 2);
		writeint(fds[i], ready);
		write(fds[i], names, 4*21*sizeof(char));
	}
}
// phase 3 server communication
void phase3() {
	for (int i = 0; i < 4; i++) {
		// write phase and index
		writeint(fds[i], 3);
		writeint(fds[i], i);
		// write seeker info
		write(fds[i], isseeker, 4*sizeof(int));
		// write map
		write(fds[i], map, height*width*sizeof(int));
		// transition to phase 4
		phase[i] = 4;
	}
}
// phase 4 server communication
void phase4() {
	int currenttime = time(NULL) - starttime;
	// check conditions for game ending
	int alivecount = 0;
	for (int i = 0; i < 4; i++) if (alive[i]) alivecount++;
	if (alivecount==1) timedied[seeker] = currenttime;
	if (currenttime > gametime || alivecount==1) {
		printf("%d %d\n", currenttime, gametime);
		for (int i = 0; i < 4; i++) phase[i]=5;
		return;
	}
	// prompt clients for phase 4 info
	for (int i = 0; i < 4; i++) {
		writeint(fds[i], 4);
		write(fds[i], pos, 4*2*sizeof(int));
		writeint(fds[i], currenttime);
	}
	// read movement information
	for (int i = 0; i < 4; i++) {
		int nx, ny;
		read(fds[i], &nx, sizeof(int));
		read(fds[i], &ny, sizeof(int));
		if (alive[i]) {
			pos[i][0] = nx;
			pos[i][1] = ny;
		}
	}
	// check for dead hiders
	for (int i = 0; i < 4; i++) if (i != seeker && alive[i]) {
		if (abs(pos[i][0]-pos[seeker][0])+abs(pos[i][1]-pos[seeker][1]) <= 1) {
			alive[i]=0;
			pos[i][0] = pos[i][1] = -1;
			timedied[i] = time(NULL) - starttime;
		}
	}
}
// phase 5 server communication
void phase5() {
	// write results to clients
	for (int i = 0; i < 4; i++) {
		writeint(fds[i], 5);
		write(fds[i], timedied, 4*sizeof(int));
		close(fds[i]);
		fds[i] = 0;
	}
	// rewrite information file to account for recent game
	starttm = *localtime(&starttime);
	load_usernames();
	int file = open("users.data", O_WRONLY | O_TRUNC, 0777);
	writeint(file, num_users);
	for (int i = 0; i < num_users; i++) {
		write(file, users[i].username, 21*sizeof(char));
		int index = -1;
		for (int j = 0; j < 4; j++) if (!strcmp(names[j], users[i].username)) index = j;
		if (index==-1) {
			writeint(file, users[i].numgames);
			if (users[i].numgames) write(file, users[i].history, users[i].numgames*sizeof(struct past_game));
		} else {
			writeint(file, users[i].numgames+1);
			if (users[i].numgames) write(file, users[i].history, users[i].numgames*sizeof(struct past_game));
			struct past_game add;
			add.date = starttm;
			add.role = isseeker[index];
			add.result = timedied[index];
			write(file, &add, sizeof(struct past_game));
		}
	}
	close(file);
}

// Login Stuff

// load usernames into users array
void load_usernames() {
	// initialize file if necessary
	if (access("users.data", F_OK) != 0) {
		int file = open("users.data", O_WRONLY | O_CREAT, 0777);
		writeint(file, 0);
		close(file);
	}
	// free if necessary
	if (users) {
		for (int i = 0; i < num_users; i++) if (users[i].numgames) free(users[i].history);
		free(users);
		users = 0;
	}
	int file = open("users.data", O_RDONLY);
	// read num_users, initalize users array
	read(file, &num_users, sizeof(int));
	users = calloc(num_users, sizeof(struct account));
	// for each user, read their username, number of games, and game history
	for (int i = 0; i < num_users; i++) {
		read(file, users[i].username, 21*sizeof(char));
		read(file, &users[i].numgames, sizeof(int));
		// printf("%s played %d games\n", users[i].username, users[i].numgames);
		if (users[i].numgames) {
			users[i].history = calloc(users[i].numgames, sizeof(struct past_game));
			read(file, users[i].history, users[i].numgames * sizeof(struct past_game));
		}
	}
	close(file);
}
// add a username to the file
void add_username(char * line) {
	// write the username and 0 games played to the file
	int file = open("users.data", O_WRONLY | O_APPEND);
	write(file, line, 21 * sizeof(char));
	writeint(file, 0);
	close(file);
	// get the first number
	file = open("users.data", O_RDONLY);
	int k;
	read(file, &k, sizeof(int));
	close(file);
	// write the first number plus one
	file = open("users.data", O_WRONLY);
	lseek(file, 0, SEEK_SET);
	writeint(file, k + 1);
	close(file);
}
// check if user exists in the database
int user_exists(char * line) {
	if (strchr(line, '\n')) *strchr(line, '\n') = 0;
	load_usernames();
	int i = 0;
	for (i = 0; i < num_users; i ++) {
		if (strcmp(users[i].username, line) == 0) return 1;
	}
	return 0;
}
// check if username matches with intended mode
int check_username(char * line, int n) {
	int val = user_exists(line);
	if (n==CREATE && val==0) add_username(line);
	return val != n;
}

// smoothly end program at ctrl+c
void INThandler(int sig) {
	close(listener);
	for (int i = 0; i < 4; i++) if (fds[i]) close(fds[i]);
	exit(0);
}
