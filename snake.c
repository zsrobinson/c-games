#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#define WIDTH 20
#define HEIGHT 10
#define SPEED_INITIAL 500000 // half second
#define SPEED_RATE 0.95 // controls the snake getting faster

#define COLOR_BG -1
#define COLOR_SNAKE 1
#define COLOR_APPLE 2

typedef struct segment {
  int x;
  int y;
  struct segment *next;
} Segment;

typedef struct apple {
  int x;
  int y;
} Apple;

// function signatures
void free_snake(Segment *seg);
void step_snake(Segment *seg);
void print_board(Segment *snake, Apple *apple);
Segment* add_segment(Segment* snake, int move);
int overlaps_snake(int x, int y, Segment *snake);
void set_rand_apple(Apple *apple, Segment *snake);
int valid_move(int c);

int main() {
  // init ncurses and set arguments
  initscr(); 
  noecho();
  nodelay(stdscr, 1);
  cbreak();
  keypad(stdscr, TRUE);

  // init colors for ncurses
  use_default_colors();
  start_color();
  init_pair(COLOR_SNAKE, COLOR_GREEN, COLOR_BG);
  init_pair(COLOR_APPLE, COLOR_RED, COLOR_BG);

  clear(); // clear the screen
  srand(time(NULL)); // seed the rng

  // create the first bit of snake
  Segment *snake = malloc(sizeof(Segment));
  snake->x = 3; snake->y = 3; snake->next = NULL;

  // init variables and get apple to start
  int score = 1;
  int move = 'd';
  int speed = SPEED_INITIAL;
  Apple apple;
  set_rand_apple(&apple, snake);

  // game loop
  while (1) {
    clear(); // clear the screen

    // get the last character pressed
    int new_move = move;
    while (new_move != ERR) {
      new_move = getch();
      if (valid_move(new_move)) {
        move = new_move;
      } else if (new_move == 'q') {
        return 0;
      }
    }

    snake = add_segment(snake, move); // add to snake
    
    // check for overflow, game over
    if (snake->x < 0 || snake->x >= WIDTH) return 0;
    if (snake->y < 0 || snake->y >= HEIGHT) return 0;

    // check for apple collision
    if (apple.x == snake->x && apple.y == snake->y) {
      score++;
      speed *= SPEED_RATE;
      set_rand_apple(&apple, snake);
    } else {
      step_snake(snake); // otherwise add new segment
    }

    if (overlaps_snake(snake->x, snake->y, snake->next)) {
      return 0; // game over
    }

    print_board(snake, &apple);
    printw("score: %d, speed: %.2f\n", score, 1000000 / (float) speed);
    refresh(); // push buffer to screen

    usleep(speed);
  }
  
  endwin();
  return 0;
}

/** removes the last segment of the snake, returns size of snake */
void step_snake(Segment *seg) {
  if (!seg) return; // base case

  // pre-order traversal
  if (seg->next && !(seg->next->next)) {
    free_snake(seg->next);
    seg->next = NULL;
  }

  step_snake(seg->next); // recurse
}

/** frees the memory associated with a snake segment */
void free_snake(Segment *seg) {
  if (seg) free_snake(seg->next);
  free(seg);
}

/** prints the game board to `stdscr` (the standard output of ncurses) */
void print_board(Segment *snake, Apple *apple) {
  char board[HEIGHT][WIDTH];

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      board[y][x] = '.';
    }
  }
  
  if (apple) {
    board[apple->y][apple->x] = '@';
  }

  while (snake) {
    board[snake->y][snake->x] = 'O';
    snake = snake->next;
  }

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      if (board[y][x] == 'O') attron(COLOR_PAIR(COLOR_SNAKE));
      if (board[y][x] == '@') attron(COLOR_PAIR(COLOR_APPLE));

      printw("%c ", board[y][x]);

      if (board[y][x] == 'O') attroff(COLOR_PAIR(COLOR_SNAKE));
      if (board[y][x] == '@') attroff(COLOR_PAIR(COLOR_APPLE));
    }
    printw("\n");
  }
}

/** adds a new segment to the beginning of the snake given a game move */
Segment* add_segment(Segment* snake, int move) {
  Segment *new_seg = malloc(sizeof(Segment));
  new_seg->x = snake->x; new_seg->y = snake->y; new_seg->next = snake;

  if (move == 'w' || move == KEY_UP) { // up
    new_seg->y--;
  } else if (move == 'a' || move == KEY_LEFT) { // left
    new_seg->x--;
  } else if (move == 's' || move == KEY_DOWN) { // down
    new_seg->y++;
  } else if (move == 'd' || move == KEY_RIGHT) { // right
    new_seg->x++;
  }

  return new_seg;
}

/** determines if a set of cords belong to a segment of the snake */
int overlaps_snake(int x, int y, Segment *snake) {
  if (snake == NULL) {
    return 0;
  } else if (x == snake->x && y == snake->y) {
    return 1;
  } else if (snake->next) {
    return overlaps_snake(x, y, snake->next);
  } else {
    return 0;
  }
}

/** sets the cords of the apple struct to a random non-snake board cell */
void set_rand_apple(Apple *apple, Segment *snake) {
  apple->x = rand() % WIDTH;
  apple->y = rand() % HEIGHT;

  if (overlaps_snake(apple->x, apple->y, snake)) {
    set_rand_apple(apple, snake); // try again
  }
}

/** determines if a key press from `getch` is a valid game move */
int valid_move(int c) {
  return c == 'w' || c == 'a' || c == 's' || c == 'd'
    || c == KEY_UP || c == KEY_LEFT || c == KEY_DOWN || c == KEY_RIGHT;
}