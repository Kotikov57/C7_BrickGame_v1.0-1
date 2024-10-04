#include <check.h>

#include "./brick_game/tetris/tetris.h"

START_TEST(gameInfoInit1) {
  GameInfo_t gi;
  gameInfoInit(&gi);
  ck_assert_int_eq(gi.level, 1);
  ck_assert_int_eq(gi.speed, INIT_SPEED);
  ck_assert_int_eq(gi.score, 0);
  ck_assert_int_eq(gi.pause, 1);
  ck_assert_ptr_nonnull(gi.field);
  ck_assert_ptr_nonnull(gi.next);
}
END_TEST

START_TEST(destroyField1) {
  int **field = (int **)malloc(BOARD_HEIGHT * sizeof(int *));
  for (int i = 0; i < BOARD_HEIGHT; i++)
    field[i] = (int *)malloc(BOARD_WIDTH * sizeof(int));

  fieldDestroy(field);
  ck_assert_ptr_null(field);
}

START_TEST(copyField1) {
  int **src = (int **)malloc(BOARD_HEIGHT * sizeof(int *));
  for (int i = 0; i < BOARD_HEIGHT; i++)
    src[i] = (int *)malloc(BOARD_WIDTH * sizeof(int));

  int **dest = (int **)malloc(BOARD_HEIGHT * sizeof(int *));
  for (int i = 0; i < BOARD_HEIGHT; i++)
    dest[i] = (int *)malloc(BOARD_WIDTH * sizeof(int));

  for (int i = 0; i < BOARD_HEIGHT; i++)
    for (int j = 0; j < BOARD_WIDTH; j++)
      src[i][j] = rand() % NUM_PIECES;

  copyField(src, dest);
  for (int i = 0; i < BOARD_HEIGHT; i++)
    for (int j = 0; j < BOARD_WIDTH; j++)
      ck_assert_int_eq(src[i][j], dest[i][j]);

  fieldDestroy(src);
  fieldDestroy(dest);
}

START_TEST(newPiece1) {
  Piece_t pc;
  newPiece(&pc);
  ck_assert_int_ge(pc.type, 0);
  ck_assert_int_lt(pc.type, NUM_PIECES);
  ck_assert_int_ge(pc.orientation, 0);
  ck_assert_int_lt(pc.orientation, 4);
  ck_assert_int_ge(pc.location.y, 0);
  ck_assert_int_lt(pc.location.y, BOARD_HEIGHT);
  ck_assert_int_ge(pc.location.x, 0);
  ck_assert_int_lt(pc.location.x, BOARD_WIDTH);
}

START_TEST(userInput1) {
  gameInfoInit(getGameInfoInstance());
  GameInfo_t gi = *getGameInfoInstance();
  Piece_t *current_piece = getCurrentPiece();
  Piece_t *next_piece = getNextPiece();
  newPiece(current_piece);
  newPiece(next_piece);
  spawnPiece(gi.field, *current_piece);
  userInput(Start, false);

  userInput(Left, false);
  ck_assert_int_eq(getCurrentPiece()->location.x, BOARD_WIDTH / 2 - 3);

  userInput(Right, false);
  ck_assert_int_eq(getCurrentPiece()->location.x, BOARD_WIDTH / 2 - 2);

  userInput(Up, false);
  ck_assert_int_eq(getCurrentPiece()->orientation, 1);

  userInput(Down, false);
  ck_assert_int_eq(*getTicks(), gi.speed / 2);

  userInput(Action, false);
  ck_assert_int_eq(getCurrentPiece()->orientation, 2);

  userInput(Pause, false);
  ck_assert_int_eq(getGameInfoInstance()->pause, 1);

  userInput(Terminate, false);
  ck_assert_int_eq(getGameInfoInstance()->pause, 2);
}

START_TEST(updateCurrentState1) {
  int *ticks = getTicks();
  GameInfo_t *gi = getGameInfoInstance();
  Piece_t *current_piece = getCurrentPiece();
  Piece_t *next_piece = getNextPiece();
  gameInfoInit(gi);
  newPiece(current_piece);
  newPiece(next_piece);
  spawnPiece(gi->field, *current_piece);
  gi->pause = 0;

  *ticks += gi->speed;
  updateCurrentState();
  ck_assert_int_eq(current_piece->location.y, 1);
  ck_assert_int_eq(*ticks, 0);

  updateCurrentState();
  ck_assert_int_eq(current_piece->location.y, 1);
  ck_assert_int_eq(*ticks, 1);

  gi->score += 600;
  updateCurrentState();
  ck_assert_int_eq(gi->level, 2);
  ck_assert_int_eq(gi->speed, INIT_SPEED - SPEED_STEP);
  ck_assert_int_eq(*ticks, 2);

  gi->next[0][0] = IBlock;
  updateCurrentState();
  ck_assert_int_eq(gi->pause, 3);
}

START_TEST(moveDown1) {
  int **field = (int **)malloc(BOARD_HEIGHT * sizeof(int *));
  for (int i = 0; i < BOARD_HEIGHT; i++)
    field[i] = (int *)malloc(BOARD_WIDTH * sizeof(int));

  Piece_t pc, next;
  newPiece(&pc);
  newPiece(&next);
  next.location.y = 2;

  spawnPiece(field, pc);
  spawnPiece(field, next);
  moveDown(field, &pc, &next);

  ck_assert_int_eq(pc.location.y, 2);

  despawnPiece(field, pc);
  despawnPiece(field, next);
  fieldDestroy(field);
}

START_TEST(moveLeftRight1) {
  int **field = (int **)malloc(BOARD_HEIGHT * sizeof(int *));
  for (int i = 0; i < BOARD_HEIGHT; i++)
    field[i] = (int *)malloc(BOARD_WIDTH * sizeof(int));

  Piece_t pc;
  newPiece(&pc);
  pc.type = IBlock;
  pc.location.y = 2;
  pc.location.x = 3;

  spawnPiece(field, pc);
  moveLeftRight(field, &pc, -1);
  ck_assert_int_eq(pc.location.x, 3);
}

START_TEST(pieceRotate1) {
  int **field = (int **)malloc(BOARD_HEIGHT * sizeof(int *));
  for (int i = 0; i < BOARD_HEIGHT; i++)
    field[i] = (int *)malloc(BOARD_WIDTH * sizeof(int));

  Piece_t pc;
  newPiece(&pc);
  pc.type = IBlock;
  pc.orientation = 1;
  pc.location.y = 3;
  pc.location.x = 4;

  spawnPiece(field, pc);
  pieceRotate(field, &pc, 1);
  ck_assert_int_eq(pc.location.x, 5);
}

START_TEST(lineCheck1) {
  int **field = (int **)malloc(BOARD_HEIGHT * sizeof(int *));
  for (int i = 0; i < BOARD_HEIGHT; i++)
    field[i] = (int *)malloc(BOARD_WIDTH * sizeof(int));

  for (int i = 0; i < BOARD_WIDTH; i++)
    field[0][i] = IBlock;
  ck_assert_int_eq(lineCheck(field, 0), 1);
}

START_TEST(checkLines1) {
  Piece_t pc;
  newPiece(&pc);
  int **field = (int **)malloc(BOARD_HEIGHT * sizeof(int *));
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    field[i] = (int *)malloc(BOARD_WIDTH * sizeof(int));
    for (int j = 0; j < BOARD_WIDTH; j++)
      field[i][j] = Empty;
  }

  for (int i = 0; i < BOARD_WIDTH; i++) {
    field[8][i] = IBlock;
    field[9][i] = IBlock;
  }
  int lines = checkLines(field, pc);
  ck_assert_int_eq(lines, 2);
}

int main(void) {
  Suite *s1 = suite_create("Core");
  TCase *tc1_1 = tcase_create("Core");
  SRunner *sr = srunner_create(s1);
  int nf;

  suite_add_tcase(s1, tc1_1);
  tcase_add_test(tc1_1, gameInfoInit1);
  // tcase_add_test(tc1_1, destroyField1);
  tcase_add_test(tc1_1, copyField1);
  tcase_add_test(tc1_1, newPiece1);
  tcase_add_test(tc1_1, userInput1);
  tcase_add_test(tc1_1, updateCurrentState1);
  tcase_add_test(tc1_1, moveDown1);
  tcase_add_test(tc1_1, moveLeftRight1);
  tcase_add_test(tc1_1, pieceRotate1);
  tcase_add_test(tc1_1, lineCheck1);
  tcase_add_test(tc1_1, checkLines1);

  srunner_run_all(sr, CK_ENV);
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);

  return nf == 0 ? 0 : 1;
}
