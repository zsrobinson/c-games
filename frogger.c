#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#define BOARD_HEIGHT 12
#define BOARD_WIDTH 16

typedef struct object {
  int x;
  int y;
  int dx;
  int length;
} Object;

typedef struct player {
  int x;
  int y;
} Player;

int main() {
  int round_time = 10, lives = 7, score = 0; // global game variables

  Player *player = malloc(sizeof(Player));
  Object *cars = NULL, *logs = NULL;
  int cars_size = 0, logs_size = 0;
  player->y = 1; player->x = 7;
  
  /* init ncurses */
  initscr();
  noecho();
  nodelay(stdscr, 1);
  cbreak();
  keypad(stdscr, TRUE);
  clear();

  srand(time(NULL));

  /* print game */
  char board[BOARD_HEIGHT][BOARD_WIDTH];

  for (int y = 0; y <= BOARD_HEIGHT; y++) {
    for (int x = 0; x <= BOARD_WIDTH; x++) {
      board[y][x] = ' ';
    }
  }

  for (int y = 1; y <= 5; y++) {
    for (int x = 1; x <= 14; x++) {
      board[y][x] = '.';
    }
  }

  for (int y = 7; y <= 11; y++) {
    for (int x = 1; x <= 14; x++) {
      board[y][x] = '~';
    }
  }

  board[player->y][player->x] = '@';
  printw(" |   ---   ---   ---   ---   | \n");

  for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
      if (x == 0 || x == 14) {
        printw("%c|", board[y][x]);
      } else {
        printw("%c ", board[y][x]);
      }
    }

    printw("\n");
  }

  printw(" |---------------------------|\n");
  refresh();

  return 0;
}