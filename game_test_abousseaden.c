#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_prof.h"
#include "game_tools.h"

void usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

int test_game_check_move_1() {
  /**
     01234567
     --------
  0 |x      ?| 1
  1 |        | 0
  2 |        | 0
  3 |   x    | 0
  4 |        | 0
  5 |        | 0
  6 |        | 0
  7 |        | 0
     --------
     00000001
  **/
  // initialize the above game with wrapping and diagadj
  game g = game_new_empty_ext(8, 8, true, true);
  game_set_expected_nb_tents_col(g, 7, 1);
  game_set_expected_nb_tents_row(g, 0, 1);
  game_set_square(g, 0, 0, TREE);
  game_set_square(g, 3, 3, TREE);
  // check_move on square (0,7) (wrapping)
  if (game_check_move(g, 0, 7, TENT) != REGULAR) {
    game_delete(g);
    return EXIT_FAILURE;
  }
  /**
       01234567
       --------
    0 |x*      | 1
    1 |  ?x    | 2
    2 |        | 0
    3 |   x    | 0
    4 |        | 0
    5 |        | 0
    6 |        | 0
    7 |        | 0
       --------
       02200001
  **/
  // intialize the above game
  game_set_square(g, 1, 3, TREE);
  game_play_move(g, 0, 1, TENT);
  game_set_expected_nb_tents_col(g, 1, 2);
  game_set_expected_nb_tents_row(g, 1, 2);
  game_set_expected_nb_tents_col(g, 2, 2);
  // check_move on square (1,2) (diagadj)
  if (game_check_move(g, 1, 2, TENT) != REGULAR) {
    game_delete(g);
    return EXIT_FAILURE;
  }
  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_new_ext() {
  // initialize a (7x54) game with "random" squares
  int nb_cols = 54;
  int nb_rows = 7;
  square squares[nb_cols * nb_rows];
  uint nb_tents_row[] = {0, 0, 0, 0, 0, 0, 0};
  uint nb_tents_col[nb_cols];
  for (int i = 0; i < nb_cols * nb_rows; i++) {
    squares[i] = (uint)i % 3;
    nb_tents_col[i % nb_cols] = 0;
  }
  game g = game_new_ext(nb_rows, nb_cols, squares, nb_tents_row, nb_tents_col,
                        true, false);
  // assert the expected nb_tents_row/squares/
  for (int i = 0; i < nb_rows; i++) {
    if (nb_tents_row[i] != game_get_expected_nb_tents_row(g, i)) {
      game_delete(g);
      return EXIT_FAILURE;
    }
    for (int j = 0; j < nb_cols; j++) {
      if (game_get_square(g, i, j) != squares[i * (nb_cols) + j]) {
        game_delete(g);
        return EXIT_FAILURE;
      }
    }
  }
  game_delete(g);
  return EXIT_SUCCESS;
}

int text_game_new_empty_ext() {
  // intialize a 10x37 game
  game g = game_new_empty_ext(10, 37, true, false);
  game_play_move(g, 9, 5, GRASS);
  // assert the square
  if (game_get_square(g, 9, 5) != GRASS) {
    game_delete(g);
    return EXIT_FAILURE;
  }
  // assert diagadj/wrapping/nb_rows/nb_cols
  if (game_is_diagadj(g) != false || game_is_wrapping(g) != true ||
      game_nb_rows(g) != 10 || game_nb_cols(g) != 37) {
    game_delete(g);
    return EXIT_FAILURE;
  }
  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_redo() {
  int i, j;
  i = 5;
  j = 3;
  // initialize a game and plays 3 moves on the same square
  game g = game_new_empty();
  game_play_move(g, i, j, GRASS);
  game_play_move(g, i, j, TENT);
  game_play_move(g, i, j, EMPTY);
  // 2 undo and 2 redo
  game_undo(g);
  game_undo(g);
  game_redo(g);
  game_redo(g);
  // check the expected result of the operation
  if (game_get_square(g, i, j) != EMPTY) {
    game_delete(g);
    return EXIT_FAILURE;
  }

  game_play_move(g, i, j, GRASS);
  game_undo(g);
  game_play_move(g, i, j, TENT);
  game_redo(g);
  if (game_get_square(g, i, j) != TENT) {
    game_delete(g);
    return EXIT_FAILURE;
  }
  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_undo() {
  int i, j;
  i = 5;
  j = 4;
  game g = game_new_empty();
  game_play_move(g, i, j, GRASS);
  game_play_move(g, i, j, TENT);
  game_play_move(g, i, j, EMPTY);
  game_undo(g);
  if (game_get_square(g, i, j) != TENT) {
    game_delete(g);
    return EXIT_FAILURE;
  }
  game_undo(g);
  if (game_get_square(g, i, j) != GRASS) {
    game_delete(g);
    return EXIT_FAILURE;
  }
  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_equal(void) {
  // test 1 on a square (GRASS)
  int i, j;
  i = 15;
  j = 30;
  game idem = game_new_empty_ext(60, 60, false, false);
  game g = game_new_empty_ext(60, 60, false, false);
  game_play_move(g, i + 1, j, GRASS);
  assert(!game_equal(g, idem));
  game_delete(g);
  game_delete(idem);
  // test 2 on diagadj/wrapping
  game g1 = game_new_empty_ext(3, 3, true, true);
  game g2 = game_new_empty_ext(3, 3, false, false);
  assert(!game_equal(g1, g2));
  game_delete(g1);
  game_delete(g2);
  return EXIT_SUCCESS;
}

int test_game_restart(void) {
  // initialize a "random" game
  game g = game_new_empty_ext(8, 8, false, true);
  game_play_move(g, 0, 0, GRASS);
  // test if restart resets the game_history
  game_undo(g);
  game_restart(g);
  game copy = game_new_empty_ext(game_nb_rows(g), game_nb_cols(g), false, true);
  game_redo(g);
  if (!game_equal(g, copy)) {
    game_delete(copy);
    game_delete(g);
    return EXIT_FAILURE;
  }
  game_delete(copy);
  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_set_expected_nb_tents_row(void) {
  int nb_rows, nb_cols;
  nb_rows = 7;
  nb_cols = 88;
  int i = 5;
  int nb_tents = 7;
  game g = game_new_empty_ext(nb_rows, nb_cols, false, false);
  game_set_expected_nb_tents_row(g, i, nb_tents);
  if (game_get_expected_nb_tents_row(g, i) != nb_tents) {
    game_delete(g);
    return EXIT_FAILURE;
  }
  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_set_expected_nb_tents_col(void) {
  int nb_rows, nb_cols;
  nb_rows = 7;
  nb_cols = 88;
  int j = 5;
  int nb_tents = 7;
  game g = game_new_empty_ext(nb_rows, nb_cols, false, false);
  game_set_expected_nb_tents_col(g, j, nb_tents);
  if (game_get_expected_nb_tents_col(g, j) != nb_tents) {
    game_delete(g);
    return EXIT_FAILURE;
  }
  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_get_expected_nb_tents_row(void) {
  if (test_game_set_expected_nb_tents_row() != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }
  // initialize a game
  int nb_rows, nb_cols;
  nb_rows = 9;
  nb_cols = 17;
  square default_square[nb_cols * nb_rows];
  for (uint i = 0; i < nb_cols * nb_rows; i++) {
    default_square[i] = EMPTY;
  }
  uint nb_tents_row[] = {3, 7, 4, 0, 4, 0, 1, 0, 7};
  uint nb_tents_col[nb_cols];
  for (int j = 0; j < nb_cols; j++) nb_tents_col[j] = 0;

  game game_def = game_new_ext(nb_rows, nb_cols, default_square, nb_tents_row,
                               nb_tents_col, true, true);

  game_set_square(game_def, 0, 0, TENT);
  // assert nb_tents_row for every row
  for (int i = 0; i < nb_rows; i++) {
    if (game_get_expected_nb_tents_row(game_def, i) != nb_tents_row[i]) {
      game_delete(game_def);
      return EXIT_FAILURE;
    }
  }
  game_delete(game_def);
  // test (2) on game_default
  game g = game_default_solution();
  int cpt = 0;
  // check the tent number for every row
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    cpt = 0;
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_square(g, i, j) == TENT) {
        cpt++;
      }
    }
    if (game_get_expected_nb_tents_row(g, i) != cpt) {
      game_delete(g);
      return EXIT_FAILURE;
    }
  }
  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_get_expected_nb_tents_col(void) {
  if (test_game_set_expected_nb_tents_col() != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }
  // test 1
  // intialize a game
  square default_square[DEFAULT_SIZE * DEFAULT_SIZE];
  for (uint i = 0; i < DEFAULT_SIZE * DEFAULT_SIZE; i++) {
    default_square[i] = EMPTY;
  }
  uint nb_tents_row[DEFAULT_SIZE] = {3, 0, 4, 7, 4, 0, 1, 0};
  uint nb_tents_col[DEFAULT_SIZE] = {4, 7, 1, 2, 1, 1, 2, 1};
  game game_def = game_new(default_square, nb_tents_row, nb_tents_col);
  game_set_square(game_def, 0, 0, TENT);
  // assert get_expected_nb_tents_col for every col
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    if (game_get_expected_nb_tents_col(game_def, i) != nb_tents_col[i]) {
      game_delete(game_def);
      return EXIT_FAILURE;
    }
  }
  game_delete(game_def);
  // test (2) on game_default
  game g = game_default_solution();
  int cpt;
  // check the tent number for every col
  for (int j = 0; j < DEFAULT_SIZE; j++) {
    cpt = 0;
    for (int i = 0; i < DEFAULT_SIZE; i++) {
      if (game_get_square(g, i, j) == TENT) {
        cpt++;
      }
    }
    if (game_get_expected_nb_tents_col(g, j) != cpt) {
      game_delete(g);
      return EXIT_FAILURE;
    }
  }
  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_get_expected_nb_tents_all(void) {
  // initialize a game
  game game_def = game_default();
  int sum_row = 0;
  int sum_col = 0;
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    sum_col += game_get_expected_nb_tents_col(game_def, i);
    sum_row += game_get_expected_nb_tents_row(game_def, i);
  }
  game g = game_default_solution();
  int result = game_get_expected_nb_tents_all(game_def);
  if (result != sum_col || result != sum_row ||
      result != game_get_expected_nb_tents_all(g)) {
    game_delete(g);
    game_delete(game_def);
    return EXIT_FAILURE;
  }
  game_delete(game_def);
  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_default_solution(void) {
  // create a default game and its solution
  game game_start = game_default();
  game game_solution = game_default_solution();
  // assert the games
  if (game_start == NULL || game_solution == NULL) {
    game_delete(game_solution);
    game_delete(game_start);
    return EXIT_FAILURE;
  }
  // checks nb_tents_row/nb_tents_col/nb_tents_all
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    if (game_get_expected_nb_tents_row(game_solution, i) !=
            game_get_current_nb_tents_row(game_solution, i) ||
        game_get_expected_nb_tents_col(game_solution, i) !=
            game_get_current_nb_tents_col(game_solution, i)) {
      game_delete(game_solution);
      game_delete(game_start);
      return EXIT_FAILURE;
    }
    if (game_get_current_nb_tents_all(game_solution) !=
        game_get_expected_nb_tents_all(game_solution)) {
      game_delete(game_solution);
      game_delete(game_start);
      return EXIT_FAILURE;
    }
  }
  // plays the solution into game_start
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      square s = game_get_square(game_solution, i, j);
      if (s != TREE) {
        if (game_check_move(game_start, i, j, s) == ILLEGAL) {
          game_delete(game_solution);
          game_delete(game_start);
          return EXIT_FAILURE;
        }
        game_play_move(game_start, i, j, s);
      }
    }
  }
  // if game_start is not over or the games are not equal there is a problem
  if (!game_equal(game_solution, game_start) || !game_is_over(game_start)) {
    game_delete(game_solution);
    game_delete(game_start);
    return EXIT_FAILURE;
  }
  game_delete(game_solution);
  game_delete(game_start);
  return EXIT_SUCCESS;
}

int test_game_load() {
  game test = game_load("../data/game_4x4w.tnt");
  game_print(test);
  game_delete(test);
  return EXIT_SUCCESS;
}
int test_game_solve() {
  game test = game_load("../data/random_20x20.tnt");
  game g = game_load("../data/game_2x4.tnt");
  uint nb_sol = game_nb_solutions(test);
  game_solve(test);
  assert(game_is_over(test) == true);
  assert(nb_sol == 4);
  game_delete(g);
  game_delete(test);
  printf("success");
  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    usage(argc, argv);
  }
  if (strcmp("game_set_expected_nb_tents_row", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_set_expected_nb_tents_row();
  }
  if (strcmp("game_set_expected_nb_tents_col", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_set_expected_nb_tents_col();
  }
  if (strcmp("game_get_expected_nb_tents_row", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_get_expected_nb_tents_row();
  }
  if (strcmp("game_get_expected_nb_tents_col", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_get_expected_nb_tents_col();
  }
  if (strcmp("game_get_expected_nb_tents_all", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_get_expected_nb_tents_all();
  }
  if (strcmp("game_default_solution", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_default_solution();
  }
  if (strcmp("game_restart", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_restart();
  }
  if (strcmp("game_equal", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_equal();
  }
  if (strcmp("game_undo", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_undo();
  }
  if (strcmp("game_redo", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_redo();
  }
  if (strcmp("game_new_ext", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_new_ext();
  }
  if (strcmp("game_new_empty_ext", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return text_game_new_empty_ext();
  }
  if (strcmp("game_check_move_1", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_check_move_1();
  }
  if (strcmp("game_load", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_load();
  }
  if (strcmp("game_solve", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_solve();
  }
  return EXIT_SUCCESS;
}
