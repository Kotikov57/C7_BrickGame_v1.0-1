#include "tetris.h"

GameInfo_t *getGameInfoInstance() {
  static GameInfo_t gameInfo;
  return &gameInfo;
}

int *getTicks() {
  static int ticks;
  return &ticks;
}

Piece_t *getCurrentPiece() {
  static Piece_t current_piece;
  return &current_piece;
}

Piece_t *getNextPiece() {
  static Piece_t next_piece;
  return &next_piece;
}

void gameInfoInit(GameInfo_t *gi) {
  gi->field = (int **)malloc(BOARD_HEIGHT * sizeof(int *));
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    gi->field[i] = (int *)malloc(BOARD_WIDTH * sizeof(int));
    for (int j = 0; j < BOARD_WIDTH; j++) gi->field[i][j] = Empty;
  }
  gi->next = (int **)malloc(BOARD_HEIGHT * sizeof(int *));
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    gi->next[i] = (int *)malloc(BOARD_WIDTH * sizeof(int));
    for (int j = 0; j < BOARD_WIDTH; j++) gi->next[i][j] = Empty;
  }
  gi->level = 1;
  gi->score = 0;
  readHighScore(gi);
  gi->speed = INIT_SPEED;
  gi->pause = 1;
}

void fieldDestroy(int **field) {
  if (field == NULL) return;
  for (int i = 0; i < BOARD_HEIGHT; i++) free(field[i]);
  free(field);
  field = NULL;
}

void copyField(int **src, int **dest) {
  for (int i = 0; i < BOARD_HEIGHT; i++)
    for (int j = 0; j < BOARD_WIDTH; j++) dest[i][j] = src[i][j];
}

void newPiece(Piece_t *pc) {
  pc->type = rand() % NUM_PIECES;
  pc->orientation = 0;
  pc->location.y = 0;
  pc->location.x = BOARD_WIDTH / 2 - 2;
}

void userInput(UserAction_t action, bool hold) {
  GameInfo_t *gi = getGameInfoInstance();
  int *ticks = getTicks();
  if (hold) {
  };
  switch ((gi->pause == 1 && action != Start && action != Terminate) ? Pause
                                                                     : action) {
    case Start:
      gi->pause = 0;
      break;
    case Pause:
      gi->pause = 1;
      break;
    case Terminate:
      gi->pause = 2;
      break;
    case Left:
      moveLeftRight(gi->next, getCurrentPiece(), -1);
      break;
    case Right:
      moveLeftRight(gi->next, getCurrentPiece(), 1);
      break;
    case Up:
      pieceRotate(gi->next, getCurrentPiece(), 1);
      break;
    case Down:
      // placeDown(gi->next, getCurrentPiece(), getNextPiece());
      (*ticks) += gi->speed / 2;
      break;
    case Action:
      pieceRotate(gi->next, getCurrentPiece(), 1);
      break;
  }
}

GameInfo_t updateCurrentState() {
  int *ticks = getTicks();
  GameInfo_t *gi = getGameInfoInstance();
  Piece_t *current_piece = getCurrentPiece();
  Piece_t *next_piece = getNextPiece();

  // memcpy(gi->next, gi->field, BOARD_HEIGHT * BOARD_WIDTH * sizeof(int));
  copyField(gi->next, gi->field);

  if (*ticks >= gi->speed && gi->pause == 0) {
    moveDown(gi->next, current_piece, next_piece);
    *ticks = 0;
  } else if (gi->pause == 0)
    (*ticks)++;
  addScore(gi, checkLines(gi->next, *current_piece));
  if (gi->score / 600 >= gi->level && gi->level < 10) {
    gi->level++;
    gi->speed -= SPEED_STEP;
  }

  writeHighScore(gi);
  if (isGameOver(gi->next, *current_piece)) gi->pause = 3;
  return *gi;
}

Location_t getPieceLocation(int type, int orientation, int i) {
  Location_t PIECES_T[NUM_PIECES][4][4] = {
      // I
      {{{1, 0}, {1, 1}, {1, 2}, {1, 3}},
       {{0, 2}, {1, 2}, {2, 2}, {3, 2}},
       {{3, 0}, {3, 1}, {3, 2}, {3, 3}},
       {{0, 1}, {1, 1}, {2, 1}, {3, 1}}},
      // J
      {{{0, 0}, {1, 0}, {1, 1}, {1, 2}},
       {{0, 1}, {0, 2}, {1, 1}, {2, 1}},
       {{1, 0}, {1, 1}, {1, 2}, {2, 2}},
       {{0, 1}, {1, 1}, {2, 0}, {2, 1}}},
      // L
      {{{0, 2}, {1, 0}, {1, 1}, {1, 2}},
       {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
       {{1, 0}, {1, 1}, {1, 2}, {2, 0}},
       {{0, 0}, {0, 1}, {1, 1}, {2, 1}}},
      // O
      {{{0, 1}, {0, 2}, {1, 1}, {1, 2}},
       {{0, 1}, {0, 2}, {1, 1}, {1, 2}},
       {{0, 1}, {0, 2}, {1, 1}, {1, 2}},
       {{0, 1}, {0, 2}, {1, 1}, {1, 2}}},
      // S
      {{{0, 1}, {0, 2}, {1, 0}, {1, 1}},
       {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
       {{1, 1}, {1, 2}, {2, 0}, {2, 1}},
       {{0, 0}, {1, 0}, {1, 1}, {2, 1}}},
      // T
      {{{0, 1}, {1, 0}, {1, 1}, {1, 2}},
       {{0, 1}, {1, 1}, {1, 2}, {2, 1}},
       {{1, 0}, {1, 1}, {1, 2}, {2, 1}},
       {{0, 1}, {1, 0}, {1, 1}, {2, 1}}},
      // Z
      {{{0, 0}, {0, 1}, {1, 1}, {1, 2}},
       {{0, 2}, {1, 1}, {1, 2}, {2, 1}},
       {{1, 0}, {1, 1}, {2, 1}, {2, 2}},
       {{0, 1}, {1, 0}, {1, 1}, {2, 0}}},
  };
  return PIECES_T[type][orientation][i];
}

void spawnPiece(int **field, Piece_t piece) {
  for (int i = 0; i < 4; i++) {
    Location_t c = getPieceLocation(piece.type, piece.orientation, i);
    field[piece.location.y + c.y][piece.location.x + c.x] = piece.type;
  }
}

void despawnPiece(int **field, Piece_t piece) {
  for (int i = 0; i < 4; i++) {
    Location_t c = getPieceLocation(piece.type, piece.orientation, i);
    field[piece.location.y + c.y][piece.location.x + c.x] = Empty;
  }
}

void moveDown(int **field, Piece_t *piece, Piece_t *next) {
  despawnPiece(field, *piece);
  piece->location.y++;
  if (!pieceFits(field, *piece)) {
    piece->location.y--;
    spawnPiece(field, *piece);
    *piece = *next;
    newPiece(next);
  }
  spawnPiece(field, *piece);
}

void moveLeftRight(int **field, Piece_t *piece, int dir) {
  despawnPiece(field, *piece);
  piece->location.x += dir;
  if (!pieceFits(field, *piece)) piece->location.x -= dir;
  spawnPiece(field, *piece);
}

// void placeDown(int **field, Piece_t *piece, Piece_t *next) {
//   despawnPiece(field, *piece);
//   while (pieceFits(field, *piece)) piece->location.y++;
//   piece->location.y--;
//   spawnPiece(field, *piece);
//   *piece = *next;
//   newPiece(next);
// }

void pieceRotate(int **field, Piece_t *piece, int dir) {
  despawnPiece(field, *piece);
  while (true) {
    piece->orientation = (piece->orientation + dir) % 4;
    if (pieceFits(field, *piece)) break;
    piece->location.x--;
    if (pieceFits(field, *piece)) break;
    piece->location.x += 2;
    if (pieceFits(field, *piece)) break;
    piece->location.x--;
  }
  spawnPiece(field, *piece);
}

bool pieceFits(int **field, Piece_t piece) {
  for (int i = 0; i < 4; i++) {
    Location_t c = getPieceLocation(piece.type, piece.orientation, i);
    int x = piece.location.x + c.x;
    int y = piece.location.y + c.y;
    if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT ||
        field[y][x] != Empty)
      return false;
  }
  return true;
}

bool lineCheck(int **field, int i) {
  for (int j = 0; j < BOARD_WIDTH; j++)
    if (field[i][j] == Empty) return false;
  return true;
}

void shiftLines(int **field, int i) {
  for (int j = i - 1; j >= 0; j--)
    for (int k = 0; k < BOARD_WIDTH; k++) {
      field[j + 1][k] = field[j][k];
      field[j][k] = Empty;
    }
}

int checkLines(int **field, Piece_t piece) {
  int lines = 0;
  despawnPiece(field, piece);
  for (int i = BOARD_HEIGHT - 1; i >= 0; i--)
    if (lineCheck(field, i)) {
      shiftLines(field, i);
      i++;
      lines++;
    }
  spawnPiece(field, piece);
  return lines;
}

void addScore(GameInfo_t *gi, int lines) {
  const int scores[] = {0, 100, 300, 700, 1500};

  gi->score += scores[lines];
  if (gi->score > gi->high_score) gi->high_score = gi->score;
}

bool isGameOver(int **field, Piece_t piece) {
  bool res = false;
  despawnPiece(field, piece);
  for (int i = 0; i <= 2; i++)
    for (int j = 0; j < BOARD_WIDTH; j++)
      if (field[i][j] != Empty) res = true;
  spawnPiece(field, piece);
  return res;
}

void readHighScore(GameInfo_t *gi) {
  FILE *fp = fopen(HIGH_SCORE_FILE, "r");
  if (fp == NULL) {
    gi->high_score = 0;
    return;
  }
  fscanf(fp, "%d", &gi->high_score);
  fclose(fp);
}

void writeHighScore(GameInfo_t *gi) {
  FILE *fp = fopen(HIGH_SCORE_FILE, "w");
  fprintf(fp, "%d", gi->high_score);
  fclose(fp);
}