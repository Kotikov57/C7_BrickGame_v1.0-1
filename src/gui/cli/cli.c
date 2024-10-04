#include "cli.h"

void initColours() {
  start_color();
  init_pair(IBlock, COLOR_RED, COLOR_BLACK);
  init_pair(JBlock, COLOR_WHITE, COLOR_BLACK);
  init_pair(LBlock, COLOR_YELLOW, COLOR_BLACK);
  init_pair(OBlock, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(SBlock, COLOR_GREEN, COLOR_BLACK);
  init_pair(TBlock, COLOR_BLUE, COLOR_BLACK);
  init_pair(ZBlock, COLOR_CYAN, COLOR_BLACK);
}

void printBoard(WINDOW *w, GameInfo_t *gi) {
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    wmove(w, i + 1, 1);
    for (int j = 0; j < BOARD_WIDTH; j++) {
      if (gi->field[i][j] != Empty) {
        waddch(w, ' ' | A_REVERSE | COLOR_PAIR(gi->field[i][j]));
        waddch(w, ' ' | A_REVERSE | COLOR_PAIR(gi->field[i][j]));
      } else {
        waddch(w, ' ');
        waddch(w, ' ');
      }
    }
  }

  if (gi->pause == 1) {
    wmove(w, BOARD_HEIGHT / 2, BOARD_WIDTH / 2);
    wprintw(w, "PAUSED...\n");
    wmove(w, BOARD_HEIGHT / 2 + 1, 1);
    wprintw(w, "Press 's' to start\n");
    wmove(w, BOARD_HEIGHT / 2 + 2, 1);
    wprintw(w, "Press 'q' to quit\n");
  } else if (gi->pause == 3) {
    wmove(w, BOARD_HEIGHT / 2, BOARD_WIDTH / 2);
    wprintw(w, "GAME OVER\n");
    wmove(w, BOARD_HEIGHT / 2 + 1, 1);
    wprintw(w, "Press 's' to start\n");
    wmove(w, BOARD_HEIGHT / 2 + 2, 1);
    wprintw(w, "Press 'q' to start\n");
  }
  box(w, 0, 0);
  wnoutrefresh(w);
}

void printPiece(WINDOW *w, Piece_t next) {
  wclear(w);
  box(w, 0, 0);
  for (int i = 0; i < 4; i++) {
    Location_t c = getPieceLocation(next.type, next.orientation, i);
    int x = c.x;
    int y = c.y;
    wmove(w, y + 1, x * 2 + 1);
    waddch(w, ' ' | A_REVERSE | COLOR_PAIR(next.type));
    waddch(w, ' ' | A_REVERSE | COLOR_PAIR(next.type));
  }
  wnoutrefresh(w);
}

void printScore(WINDOW *w, GameInfo_t *gi) {
  wclear(w);
  wprintw(w, "Score:\n%d\n", gi->score);
  wprintw(w, "High Score:\n%d\n", gi->high_score);
  wprintw(w, "Level:\n%d\n", gi->level);
  wprintw(w, "Speed:\n%d\n", gi->speed);
  wnoutrefresh(w);
}