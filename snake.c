#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#define WIDTH 40
#define HEIGHT 10

typedef struct segment {
  int x;
  int y;
  struct segment *next;
} Segment;

typedef struct apple {
  int x;
  int y;
} Apple;

void free_snake(Segment *seg);
void step_snake(Segment *seg);
void print_board(Segment *snake, Apple *apple);
Segment* add_segment(Segment* snake, char move);
int overlaps_snake(int x, int y, Segment *snake);
void set_rand_apple(Apple *apple, Segment *snake);
int valid_move(char c);

int main() {
  // init ncurses, set arguments
  WINDOW *w = initscr(); 
  noecho();
  nodelay(w, 1);
  cbreak();

  clear(); // clear the screen
  srand(time(NULL)); // seed the rng

  // create the first bit of snake
  Segment *snake = malloc(sizeof(Segment));
  snake->x = 3; snake->y = 3; snake->next = NULL;

  // init variables and get apple to start
  int score = 1;
  char move = 'd';
  Apple apple;
  set_rand_apple(&apple, snake);

  // game loop
  while (1) {
    clear(); // clear the screen

    // get the last character pressed
    char new_move = move;
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

    if (overlaps_snake(snake->x, snake->y, snake->next)) {
      return 0; // game over
    }
    
    if (apple.x == snake->x && apple.y == snake->y) {
      score++;
      set_rand_apple(&apple, snake);
    } else {
      step_snake(snake);
    }

    print_board(snake, &apple);
    printw("score: %d\n", score);
    refresh(); // push buffer to screen
    sleep(1); // wait a second
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
      printw("%c", board[y][x]);
    }
    printw("\n");
  }
}

Segment* add_segment(Segment* snake, char move) {
  Segment *new_seg = malloc(sizeof(Segment));
  new_seg->x = snake->x; new_seg->y = snake->y; new_seg->next = snake;

  if (move == 'w') { // up
    new_seg->y--;
  } else if (move == 'a') { // left
    new_seg->x--;
  } else if (move == 's') {
    new_seg->y++;
  } else if (move == 'd') {
    new_seg->x++;
  }

  return new_seg;
}

int overlaps_snake(int x, int y, Segment *snake) {
  if (x == snake->x && y == snake->y) {
    return 1;
  } else if (snake->next) {
    return overlaps_snake(x, y, snake->next);
  } else {
    return 0;
  }
}

void set_rand_apple(Apple *apple, Segment *snake) {
  apple->x = rand() % WIDTH;
  apple->y = rand() % HEIGHT;

  if (overlaps_snake(apple->x, apple->y, snake)) {
    set_rand_apple(apple, snake); // try again
  }
}

int valid_move(char c) {
  return c == 'w' || c == 'a' || c == 's' || c == 'd';
}