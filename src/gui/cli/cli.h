#ifndef C7_BRICKGAME_V1_0_1_CLI_H
#define C7_BRICKGAME_V1_0_1_CLI_H

#include <ncurses.h>

#include "../../brick_game/tetris/tetris.h"

#define FPS 60

void initColours();
void printBoard(WINDOW *w, GameInfo_t *gi);
void printPiece(WINDOW *w, Piece_t next);
void printScore(WINDOW *w, GameInfo_t *gi);

#endif  // C7_BRICKGAME_V1_0_1_CLI_H
