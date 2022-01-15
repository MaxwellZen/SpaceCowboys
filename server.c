#include "networking.h"

int fds[4];
int phase[4];
char names[4][21];
int found;
const int height = 30, width = 120;
int map[height][width];
float pos[4][2];
int ipos[4][2];
int seeker;
int isseeker[4];

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
			// do game stuff
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
	// create map
	for (int i = 0; i < width; i++) map[0][i] = map[height-1][i] = -1;
	for (int i = 0; i < height; i++) map[i][0] = map[i][width-1] = -1;
	for (int i = 1; i < height-1; i++) {
		for (int j = 1; j < width-1; j++) {
			map[i][j] = 50 + (rand() % 30);
			if (rand()%9 == 0) map[i][j] = -2;
		}
	}
}


void phase1(int i) {

}
void phase2(int i) {
	int x = 2;
	write(fds[i], &x, sizeof(int));
	write(fds[i], &found, sizeof(int));
	write(fds[i], names, sizeof(names));
}
void phase3(int i) {
	// write phase
	int x = 3;
	write(fds[i], &x, sizeof(int));
	// write game index
	write(fds[i], &i, sizeof(int));
	// write seeker info
	write(fds[i], isseeker, 4*sizeof(int));
	// write height and width
	write(fds[i], &height, sizeof(int));
	write(fds[i], &width, sizeof(int));
	// write map
	write(fds[i], map, sizeof(map));
}
void phase4(int i) {

}
void phase5(int i) {

}
