#include <curses.h>

#define OBSTACLE '@'
#define BORDER '#'
#define FLOOR1 '_'
#define FLOOR2 '_'

char seeker[] = {0xF0, 0x9F, 0x9B, 0xB8, '\0'};
char hider[] = {0xF0, 0x9F, 0x90, 0x84, '\0'};
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define BRIGHT "\x1b[1m"
#define REV   "\x1b[7m"
#define DIM   "\x1b[2m"
#define RESET "\x1B[0m"

int main();
void get_username_mode();
void get_username();
void get_history();
void curses_setup();
void undo_curses_setup();
void phase2_display();
void game_display();
void INThandler(int sig);
