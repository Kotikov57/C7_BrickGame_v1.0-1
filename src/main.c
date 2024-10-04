#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "./brick_game/tetris/tetris.h"
#include "./gui/cli/cli.h"

int main() {
  srand(time(NULL));
  WINDOW *board, *next, *score;
  bool running = true;
  Piece_t *current_piece = getCurrentPiece(), *next_piece = getNextPiece();

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, true);
  timeout(0);
  curs_set(0);
  initColours();

  board = newwin(BOARD_HEIGHT + 2, 2 * BOARD_WIDTH + 2, 0, 0);
  next = newwin(6, 10, 0, 2 * (BOARD_WIDTH + 1) + 1);
  score = newwin(8, 15, 7, 2 * (BOARD_WIDTH + 1) + 1);

  while (running) {
    gameInfoInit(getGameInfoInstance());
    GameInfo_t gi = *getGameInfoInstance();
    newPiece(current_piece);
    newPiece(next_piece);
    spawnPiece(gi.field, *current_piece);

    while (true) {
      printBoard(board, &gi);
      printPiece(next, *next_piece);
      printScore(score, &gi);

      doupdate();
      usleep(1000000 / FPS);
      gi = updateCurrentState();
      userInput(getch(), false);

      if (gi.pause == 2) {
        running = false;
        break;
      } else if (gi.pause == 3)
        break;
    }
  }

  fieldDestroy(getGameInfoInstance()->field);
  fieldDestroy(getGameInfoInstance()->next);
  wclear(stdscr);
  delwin(board);
  delwin(next);
  delwin(score);
  endwin();
  return 0;
}