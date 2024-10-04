#ifndef C7_BRICKGAME_V1_0_1_TETRIS_H
#define C7_BRICKGAME_V1_0_1_TETRIS_H

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define NUM_PIECES 7
#define INIT_SPEED 50
#define SPEED_STEP 5
#define HIGH_SCORE_FILE "high_score.txt"

#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  Start = 's',
  Pause = 'p',
  Terminate = 'q',
  Left = KEY_LEFT,
  Right = KEY_RIGHT,
  Up,
  Down = KEY_DOWN,
  Action = KEY_UP
} UserAction_t;

typedef enum {
  IBlock,
  JBlock,
  LBlock,
  OBlock,
  SBlock,
  TBlock,
  ZBlock,
  Empty,
  Locked
} PiecesEnum_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef struct {
  int y;
  int x;
} Location_t;

typedef struct {
  int type;
  int orientation;
  Location_t location;
} Piece_t;

GameInfo_t *getGameInfoInstance();
int *getTicks();
Piece_t *getCurrentPiece();
Piece_t *getNextPiece();

void gameInfoInit(GameInfo_t *gi);
void fieldDestroy(int **field);
void copyField(int **src, int **dest);
void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();
void newPiece(Piece_t *pc);
Location_t getPieceLocation(int type, int orientation, int i);
void spawnPiece(int **field, Piece_t piece);
void despawnPiece(int **field, Piece_t piece);
void moveDown(int **field, Piece_t *piece, Piece_t *next);
void moveLeftRight(int **field, Piece_t *piece, int dir);
void placeDown(int **field, Piece_t *piece, Piece_t *next);
void pieceRotate(int **field, Piece_t *piece, int dir);
bool pieceFits(int **field, Piece_t piece);
bool lineCheck(int **field, int i);
void shiftLines(int **field, int i);
int checkLines(int **field, Piece_t piece);
void addScore(GameInfo_t *gi, int lines);
bool isGameOver(int **field, Piece_t piece);
void readHighScore(GameInfo_t *gi);
void writeHighScore(GameInfo_t *gi);
#endif  // C7_BRICKGAME_V1_0_1_TETRIS_H
