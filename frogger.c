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

int valid_move(int c);
void print_game(Player *player);

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

  srand(time(NULL));

  clear();
  print_game(player);
  refresh();

  while (1) {
    int input = getch();
    if (input == 'q') break;
    if (valid_move(input)) {
      if (input == 'w' || input == KEY_UP) { // up
        player->y++;
      } else if (input == 'a' || input == KEY_LEFT) { // left
        player->x--;
      } else if (input == 's' || input == KEY_DOWN) { // down
        player->y--;
      } else if (input == 'd' || input == KEY_RIGHT) { // right
        player->x++;
      }
    } else {
      continue;
    }

    clear();
    print_game(player);
    refresh();
  }

  return 0;
}

void print_game(Player *player) {
  char board[BOARD_HEIGHT][BOARD_WIDTH];

  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
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
}

/** determines if a key press from `getch` is a valid game move */
int valid_move(int c) {
  return c == 'w' || c == 'a' || c == 's' || c == 'd'
    || c == KEY_UP || c == KEY_LEFT || c == KEY_DOWN || c == KEY_RIGHT;
}
