#include "networking.h"

int listener;
int fds[4];
int phase[4];
char names[4][21];
int found, ready;
int map[height][width];
int ipos[4][2];
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
void phase3(int i);
void phase4();
void phase5();

void load_usernames();
void add_username(char * line);
int user_exists(char * line);
int check_username(char * line, int n);

void INThandler(int sig);

// struct user * arr;
struct account *arr;
int num_users;

int main() {

	signal(SIGINT, INThandler);
	listener = server_setup();
	if (listener==-1) {
		printf("Listener failed\n");
		exit(0);
	}
	while (1) {
		for (int i = 0; i < 4; i++) {
			fds[i] = 0;
			phase[i] = 0;
			strcpy(names[i], "Waiting...");
		}
		int max_descriptor = listener;
		found = ready = 0;

		while (found<4) {
			FD_ZERO(&read_fds);
			FD_SET(listener, &read_fds);
			for (int i = 0; i < found; i++) FD_SET(fds[i], &read_fds);
			select(max_descriptor+1, &read_fds, NULL, NULL, NULL);
			for (int i = 0; i < found; i++) {
				if (FD_ISSET(fds[i], &read_fds)) {
					process(i);
				}
			}
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

		int f = fork();
		if (f) {
			for (int i = 0; i < 4; i++) {
				close(fds[i]);
				fds[i] = 0;
			}
		} else {
			while (ready<4) {
				FD_ZERO(&read_fds);
				for (int i = 0; i < 4; i++) if (phase[i]==1) FD_SET(fds[i], &read_fds);
				select(max_descriptor+1, &read_fds, NULL, NULL, NULL);
				for (int i = 0; i < 4; i++) if (phase[i]==1 && FD_ISSET(fds[i], &read_fds)) {
					process(i);
				}
			}
			for (int i = 0; i < 4; i++) phase[i]=3;
			gamesetup();
			for (int i = 0; i < 4; i++) phase3(i);
			while (phase[0]==4) {
				usleep(50000);
				phase4();
			}
			phase5();
			exit(0);
		}
	}
}

void process(int i) {
	if (phase[i]==0) return;
	else if (phase[i]==1) phase1(i);
	else if (phase[i]==2) phase2();
	else if (phase[i]==3) phase3(i);
	else if (phase[i]==4) phase4(i);
	else if (phase[i]==5) phase5(i);
}
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
		ipos[i][0] = 3; ipos[i][1] = 3;
		if (i&1) ipos[i][0] = height - 4;
		if (i&2) ipos[i][1] = width - 4;
		map[ipos[i][0]][ipos[i][1]] = 50 + (rand() % 30);
	}
}

void phase1(int i) {
	int x;
	read(fds[i], &x, sizeof(int));
	if (x==0) return;
	int n;
	read(fds[i], &n, sizeof(int));
	char line[namelen+1];
	line[namelen]=0;
	read(fds[i], line, (namelen+1) * sizeof(char));
	if (check_username(line, n)) {
		printf("Check successful\n");
		strcpy(names[i], line);
		phase[i] = 2;
		ready++;
		writeint(fds[i], 1);
		load_usernames();
		int index = -1;
		for (int j = 0; j < num_users; j++) if (!strcmp(names[i], arr[j].username)) index = j;
		writeint(fds[i], arr[index].numgames);
		if (arr[index].numgames>0) write(fds[i], arr[index].history, arr[index].numgames * sizeof(struct past_game));
		phase2();
	} else {
		writeint(fds[i], 0);
		writeint(fds[i], 1);
	}
}
void phase2() {
	printf("Phase 2 names: \n");
	for (int i = 0; i < 4; i++) printf("[%s]\n", names[i]);
	for (int i = 0; i < 4; i++) if (phase[i]==2) {
		writeint(fds[i], 2);
		writeint(fds[i], ready);
		write(fds[i], names, 4*21*sizeof(char));
	}
}
void phase3(int i) {
	writeint(fds[i], 3);
	writeint(fds[i], i);
	// write seeker info
	write(fds[i], isseeker, 4*sizeof(int));
	// write map
	write(fds[i], map, height*width*sizeof(int));
	phase[i] = 4;
}
void phase4() {
	int currenttime = time(NULL) - starttime;
	int alivecount = 0;
	for (int i = 0; i < 4; i++) if (alive[i]) alivecount++;
	if (alivecount==1) timedied[seeker] = currenttime;
	if (currenttime > gametime || alivecount==1) {
		printf("%d %d\n", currenttime, gametime);
		for (int i = 0; i < 4; i++) phase[i]=5;
		return;
	}
	for (int i = 0; i < 4; i++) {
		writeint(fds[i], 4);
		write(fds[i], ipos, 4*2*sizeof(int));
		writeint(fds[i], currenttime);
	}
	for (int i = 0; i < 4; i++) {
		int dx, dy;
		read(fds[i], &dx, sizeof(int));
		read(fds[i], &dy, sizeof(int));
		if (alive[i]) {
			int nx = ipos[i][0] + dx;
			int ny = ipos[i][1] + dy;
			int change = 1;
			int inx = nx, iny = ny;
			if (nx<1 || nx>height-2 || ny<1 || ny>width-2) change = 0;
			if (map[inx][iny]==-2) change = 0;
			if (change) {
				ipos[i][0] = inx;
				ipos[i][1] = iny;
			}
		}
	}

	for (int i = 0; i < 4; i++) if (i != seeker && alive[i]) {
		if (abs(ipos[i][0]-ipos[seeker][0])+abs(ipos[i][1]-ipos[seeker][1]) <= 1) {
			alive[i]=0;
			ipos[i][0] = ipos[i][1] = -1;
			timedied[i] = time(NULL) - starttime;
		}
	}
}
void phase5() {
	for (int i = 0; i < 4; i++) {
		writeint(fds[i], 5);
		write(fds[i], timedied, 4*sizeof(int));
		close(fds[i]);
		fds[i] = 0;
	}
	starttm = *localtime(&starttime);
	load_usernames();
	int file = open("users.data", O_WRONLY | O_TRUNC, 0777);
	writeint(file, num_users);
	for (int i = 0; i < num_users; i++) {
		write(file, arr[i].username, 21*sizeof(char));
		int index = -1;
		for (int j = 0; j < 4; j++) if (!strcmp(names[j], arr[i].username)) index = j;
		if (index==-1) {
			writeint(file, arr[i].numgames);
			if (arr[i].numgames) write(file, arr[i].history, arr[i].numgames*sizeof(struct past_game));
		} else {
			writeint(file, arr[i].numgames+1);
			if (arr[i].numgames) write(file, arr[i].history, arr[i].numgames*sizeof(struct past_game));
			struct past_game add;
			add.date = starttm;
			add.role = isseeker[index];
			add.result = timedied[index];
			write(file, &add, sizeof(struct past_game));
		}
	}
}



// Login Stuff



void debug_game(struct past_game game) {
	struct tm tm = game.date;
	printf("%02d-%02d %02d:%02d:%02d   ", tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	if (game.role) {
		printf("SEEKER   ");
		if (game.result==-1) printf("LOST");
		else printf("WON IN %d SECONDS", game.result);
	} else {
		printf("HIDER   ");
		if (game.result==-1) printf("SURVIVED");
		else printf("DIED IN %d SECONDS", game.result);
	}
	printf("\n");
}

void load_usernames() {
	if (access("users.data", F_OK) != 0) {
		int file = open("users.data", O_WRONLY | O_CREAT, 0777);
		writeint(file, 0);
		close(file);
	}
	if (arr) {
		for (int i = 0; i < num_users; i++) if (arr[i].numgames) free(arr[i].history);
		free(arr);
		arr = 0;
	}
	int file = open("users.data", O_RDONLY);
	read(file, &num_users, sizeof(int));
	printf("%d users\n", num_users);
	arr = calloc(num_users, sizeof(int));
	for (int i = 0; i < num_users; i++) {
		read(file, arr[i].username, 21*sizeof(char));
		read(file, &arr[i].numgames, sizeof(int));
		printf("%s has played %d games\n", arr[i].username, arr[i].numgames);
		if (arr[i].numgames) {
			arr[i].history = calloc(arr[i].numgames, sizeof(struct past_game));
			read(file, arr[i].history, arr[i].numgames * sizeof(struct past_game));
		}
		for (int j = 0; j < arr[i].numgames; j++) {
			printf("Game %d: ", j);
			debug_game(arr[i].history[j]);
		}
	}
	close(file);
}
void add_username(char * line) {
	int file = open("users.data", O_WRONLY | O_APPEND);
	write(file, line, 21 * sizeof(char));
	writeint(file, 0);
	close(file);
	file = open("users.data", O_RDONLY);
	int k;
	read(file, &k, sizeof(int));
	close(file);
	file = open("users.data", O_WRONLY);
	lseek(file, 0, SEEK_SET);
	writeint(file, k + 1);
	close(file);
}
int user_exists(char * line) {
	if (strchr(line, '\n')) *strchr(line, '\n') = 0;
	load_usernames();
	int i = 0;
	for (i = 0; i < num_users; i ++) {
		if (strcmp(arr[i].username, line) == 0) return 1;
	}
	return 0;
}
int check_username(char * line, int n) {
	int val = user_exists(line);
	if (n==CREATE && val==0) add_username(line);
	return val != n;
}


void INThandler(int sig) {
	close(listener);
	for (int i = 0; i < 4; i++) if (fds[i]) close(fds[i]);
	exit(0);
}
