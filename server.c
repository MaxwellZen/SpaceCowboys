#include "networking.h"

int listener;
int fds[4];
int phase[4];
char names[4][21];
int found;
int map[height][width];
float pos[4][2];
int ipos[4][2];
int seeker;
int isseeker[4];
int alive[4];
int timedied[4];
time_t starttime;

void process(int i);
void gamesetup();

void phase1(int i);
void phase2();
void phase3(int i);
void phase4();
void phase5();

struct user * arr;

int main() {

	signal(SIGINT, INThandler);
	srand(time(NULL));
	listener = server_setup();
	while (1) {
		for (int i = 0; i < 4; i++) {
			fds[i] = 0;
			phase[i] = 0;
			strcpy(names[i], "Waiting...");
		}
		fd_set read_fds;
		int max_descriptor = listener;
		found = 0;

		while (found<4) {
			FD_ZERO(&read_fds);
			FD_SET(listener, &read_fds);
			for (int i = 0; i < found; i++) FD_SET(fds[i], &read_fds);
			select(max_descriptor+1, &read_fds, NULL, NULL, NULL);
			for (int i = 0; i < found; i++) {
				if (FD_ISSET(fds[i], &read_fds)) {
					printf("Processing %d\n", i);
					process(i);
				}
			}
			if (FD_ISSET(listener, &read_fds)) {
				fds[found] = server_connect(listener);
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
			found = 0;
			for (int i = 0; i < 4; i++) {
				if (phase[i]==2) found++;
			}
			while (found<4) {
				FD_ZERO(&read_fds);
				for (int i = 0; i < 4; i++) if (phase[i]==1) FD_SET(fds[i], &read_fds);
				select(max_descriptor+1, &read_fds, NULL, NULL, NULL);
				for (int i = 0; i < 4; i++) if (phase[i]==1 && FD_ISSET(fds[i], &read_fds)) {
					process(i);
					if (phase[i]==2) found++;
					printf("%d usernames set\n", found);
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
	// create seeker info
	for (int i = 0; i < 4; i++) isseeker[i] = 0;
	// seeker = rand()%4;
	seeker = 0;
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
		ipos[i][0] = pos[i][0] = 3; ipos[i][1] = pos[i][1] = 3;
		if (i&1) ipos[i][0] = pos[i][0] = height - 4;
		if (i&2) ipos[i][1] = pos[i][1] = width - 4;
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
		writeint(fds[i], 1);
		strcpy(names[i], line);
		phase[i] = 2;
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
		writeint(fds[i], found);
		for (int j = 0; j < 4; j++) for (int k = 0; k < 21; k++) write(fds[i], &names[j][k], sizeof(char));
	}
}
void phase3(int i) {
	printf("sending phase 3 info to %d\n", i);
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
				pos[i][0] = nx;
				pos[i][1] = ny;
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
}



// Login Stuff
struct user{
	char username[20];
	int games;
};

long long filesize(char * name) {
	struct stat stats;
	stat(name, &stats);
	return stats.st_size;
}

void load_usernames() {
	int file = open("users.txt", O_RDONLY);
	int len = filesize("users.txt");
	char * data = malloc((len + 1) * sizeof(char));
	int result = read(file, data, sizeof(data));
	data[len] = 0;

	int i, j = 0;
	int lines = 0;
	for (i = 0; i < sizeof(data); i ++) {
		if (data[i] == '\n') lines += 1;
	}

	arr = calloc(lines / 2, sizeof(struct user));

	for (i = 0; i < lines / 2; i ++) {
		char * temp = strsep(&data, "\n");
		strcpy(arr[i].username, temp);
		temp = strsep(&data, "\n");
		sscanf(temp, "%d", &arr[i].games);
		// printf("%s %d\n", arr[i].username, arr[i].games);
	}
}

void add_username(char * line) {
	int file = open("users.txt", O_WRONLY | O_APPEND);
	write(file, line, strlen(line));
	write(file, "\n", 1);
	write(file, "0\n", 2);
	close(file);
}

int user_exists(char * line) {
	if (strchr(line, '\n')) *strchr(line, '\n') = 0;
	load_usernames();
	int i = 0;
	for (i = 0; i < sizeof(arr); i ++) {
		if (strcmp(arr[i].username, line) == 0) return 1;
	}
	return 0;
}

int check_username(char * line, int n) {
	int val = user_exists(line);
	printf("[%s], val: %d, n: %d\n", line, val, n);
	if (n==CREATE && val==0) add_username(line);
	return val != n;
	// // Login
	// if (n == 0) {
	// 	if (val == 1) printf("Login successful");
	// 	else printf("Username does not exist\n");
	// }
	// // Create Account
	// else {
	// 	if (val == 1) printf("Username already exists\n");
	// 	else add_username(line);
	// }
}



void INThandler(int sig) {
	close(listener);
	for (int i = 0; i < 4; i++) if (fds[i]) close(fds[i]);
	exit(0);
}
