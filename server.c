#include "networking.h"

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
void phase2(int i);
void phase3(int i);
void phase4(int i);
void phase5(int i);

int main() {
	srand(time(NULL));
	int listener = server_setup();
	while (1) {
		for (int i = 0; i < 4; i++) {
			phase[i] = 0;
			names[i][0] = 0;
		}
		fd_set read_fds;
		int max_descriptor = listener;
		found = 0;

		while (1) {
			FD_ZERO(&read_fds);
			FD_SET(listener, &read_fds);
			for (int i = 0; i < found; i++) FD_SET(fds[i], &read_fds);
			select(max_descriptor+1, &read_fds, NULL, NULL, NULL);
			for (int i = 0; i < found; i++) {
				if (FD_ISSET(fds[i], &read_fds)) {
					int g = fork();
					if (!g) {
						process(i);
						exit(0);
					}
				}
			}
			if (FD_ISSET(listener, &read_fds)) {
				fds[found] = server_connect(listener);
				if (fds[found] > max_descriptor) max_descriptor = fds[found];
				phase[found] = 1;
				strcpy(names[found], "Setting username...");
				found++;
				printf("Found %d clients\n", found);
			}
			if (found==4) break;
		}

		int f = fork();
		if (f) {
			for (int i = 0; i < 4; i++) close(fds[i]);
		} else {
			gamesetup();
			while (1) {
				usleep(50000);
				for (int i = 0; i < 4; i++) process(i);
			}
			exit(0);
		}
	}
}

void process(int i) {
	if (phase[i]==0) return;
	else if (phase[i]==1) phase1(i);
	else if (phase[i]==2) phase2(i);
	else if (phase[i]==3) phase3(i);
	else if (phase[i]==4) phase4(i);
	else if (phase[i]==5) phase5(i);
}
void gamesetup() {
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
	for (int i = 0; i < 4; i++) {
		pos[i][0] = 3; pos[i][1] = 3;
		if (i&1) pos[i][0] = height - 4;
		if (i&2) pos[i][1] = width - 4;
		map[ipos[i][0]][ipos[i][1]] = 50 + (rand() % 30);
	}
}
void writeint(int fd, int x) {
	write(fd, &x, sizeof(int));
}


void phase1(int i) {

}
void phase2(int i) {
	writeint(fds[i], 2);
	writeint(fds[i], found);
	write(fds[i], names, sizeof(names));
}
void phase3(int i) {
	writeint(fds[i], 3);
	writeint(fds[i], i);
	// write seeker info
	write(fds[i], isseeker, 4*sizeof(int));
	// write map
	write(fds[i], map, sizeof(map));
}
void phase4(int i) {
	for (int i = 0; i < 4; i++) {
		int dx, dy;
		read(fds[i], &dx, sizeof(int));
		read(fds[i], &dy, sizeof(int));
		float nx, ny;
		if (isseeker[i]) {
			nx = pos[i][0] + 1.5*dx;
			ny = pos[i][1] + 1.5*dy;
		} else {
			nx = pos[i][0] + 1.0*dx;
			ny = pos[i][1] + 1.0*dy;
		}
		int change = 1;
		int inx = nx, iny = ny;
		if (nx<1 || nx>height-2 || ny<1 || ny>width-2) change = 0;
		if (map[inx][iny]==-2 || map[inx+1][iny]==-2 || map[inx][iny+1]==-2 || map[inx+1][iny+1]==-2) change = 0;
		if (change) {
			pos[i][0] = nx;
			pos[i][1] = ny;
			ipos[i][0] = inx;
			ipos[i][1] = iny;
		}
	}
	for (int i = 0; i < 4; i++) if (i != seeker && alive[i]) {
		if (hypot(pos[i][0]-pos[seeker][0], pos[i][1]-pos[seeker][1]) <= 1) {
			alive[i]=0;
			timedied[i] = time(NULL) - starttime;
		}
	}
	for (int i = 0; i < 4; i++) {
		writeint(fds[i], 4);
		if (alive[i]) {
			writeint(fds[i], ipos[i][0]);
			writeint(fds[i], ipos[i][1]);
		} else {
			writeint(fds[i], -1);
			writeint(fds[i], -1);
		}
	}
}
void phase5(int i) {

}
