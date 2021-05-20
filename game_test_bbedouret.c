#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
void usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

/* ********** TESTS ********** */

/**
 * @brief test if a game is duplicated. We check if g and g2 are equal.
 * @param g the game to copy
 * @param g2 the game which is copied
 * @return true if the game is duplicated
 * @pre @p g must be a valid pointer toward a game structure.
 **/

int test_game_copy(void) {
  int nb_cols = 8;
  int nb_rows = 8;
  // take a new empty game
  game g = game_new_empty_ext(nb_rows, nb_cols, true, true);
  if (g == NULL) {
    return EXIT_FAILURE;
  }
  game_set_square(g, 0, 7, TREE);
  game_set_square(g, 7, 0, TREE);
  game_play_move(g, 0, 0, TENT);
  game_play_move(g, 7, 7, TENT);
  game_undo(g);
  game_redo(g);
  game g2 = game_copy(g);
  if (!game_equal(g, g2)) {
    game_delete(g);
    game_delete(g2);
    return EXIT_FAILURE;
  }
  // check expected nb_tent_all
  if (game_get_expected_nb_tents_all(g2) != game_get_expected_nb_tents_all(g)) {
    game_delete(g);
    game_delete(g2);
    return EXIT_FAILURE;
  }
  // print the game
  game_print(g2);
  game_print(g);
  // check if the squares are the same between the two games
  if (game_get_square(g2, 0, 7) != TREE && game_get_square(g2, 7, 0) != TREE &&
      game_get_square(g2, 0, 0) != TENT && game_get_square(g2, 7, 7) != TENT) {
    game_delete(g);
    game_delete(g2);
    return EXIT_FAILURE;
  }
  game_delete(g);
  game_delete(g2);
  return EXIT_SUCCESS;
}

/**
 * @brief test if a game is printed correctly.
 * @param g the game to print
 * @return true if the game is printed
 * @pre @p g must be a valid pointer toward a game structure.
 **/

int test_game_print(void) {
  int nb_cols = 8;
  int nb_rows = 8;
  game g = game_new_empty_ext(nb_rows, nb_cols, true, true);
  if (g == NULL) {
    return EXIT_FAILURE;
  }
  game_print(g);
  game_delete(g);
  return EXIT_SUCCESS;
}

/**
 * @brief test if a game is equal. The function operates by taking 4 games
 *(g1a,g1b,g1c,g1d) which contains diagadj=true/false and wrapping=true/false
 *for each game. The function check if they are all equal.
 * @param g a game
 * @param idem a game
 * @param g1a g1b g1c g1d
 * @param g2a g2b g2c g2d
 * @param nb_rows the number of rows in g
 * @param nb_cols the number of cols in g
 * @return true if the game is printed
 * @pre @p g must be a valid pointer toward a game structure.
 **/
int test_game_equal(void) {
  int i, j;
  i = 15;
  j = 30;
  game idem = game_new_empty_ext(60, 60, false, false);
  game g = game_new_empty_ext(60, 60, false, false);

  // check if g and idem are equal
  game_play_move(g, i + 1, j, GRASS);
  assert(!game_equal(g, idem));
  game_play_move(idem, i + 1, j, GRASS);
  game_play_move(g, i, j, TENT);
  assert(!game_equal(g, idem));
  game_delete(g);
  game_delete(idem);

  /* IF WRAPPING=FALSE AND DIAGADJ=FALSE */

  // check if the two games are equal
  game g1a = game_new_empty_ext(20, 20, false, false);
  game g2a = game_copy(g1a);
  assert(game_equal(g1a, g2a) == true);
  game_play_move(g1a, 1, 1, TENT);
  game_play_move(g1a, 3, 5, GRASS);
  game_play_move(g2a, 1, 1, TENT);
  game_play_move(g2a, 3, 5, GRASS);
  if (g1a == NULL || g2a == NULL) {
    return EXIT_FAILURE;
  }
  int nb_tents1a = 0;
  int nb_tents2a = 0;
  uint nb_rowsa = 20;
  uint nb_colsa = 20;
  // iterate over the number of rows
  for (uint i = 0; i < nb_rowsa; i++) {
    // iterate over the number of cols
    for (uint j = 0; j < nb_colsa; j++) {
      square s1 = game_get_square(g1a, i, j);
      square s2 = game_get_square(g2a, i, j);
      // check if the square is equal to the other square
      if (s1 != s2) {
        game_delete(g2a);
        return EXIT_FAILURE;
      }
      if (s1 == TENT) {
        nb_tents1a++;
      }
      if (s2 == TENT) {
        nb_tents2a++;
      }
      // check if there is the same number of tents in the two games
      if (nb_tents1a++ != nb_tents2a++) {
        game_delete(g2a);
        return EXIT_FAILURE;
      }
    }
  }
  // assert boolean wrapping and diagadj
  assert(game_is_diagadj(g1a) == game_is_diagadj(g2a));
  assert(game_is_wrapping(g1a) == game_is_wrapping(g2a));
  game_delete(g2a);

  /* IF WRAPPING=TRUE AND DIAGADJ=FALSE */

  game g1b = game_new_empty_ext(20, 20, true, false);
  game g2b = game_copy(g1b);
  assert(game_equal(g1b, g2b) == true);
  game_play_move(g1b, 1, 1, TENT);
  game_play_move(g1b, 3, 5, GRASS);
  game_play_move(g2b, 1, 1, TENT);
  game_play_move(g2b, 3, 5, GRASS);
  if (g1b == NULL || g2b == NULL) {
    return EXIT_FAILURE;
  }
  int nb_tents1b = 0;
  int nb_tents2b = 0;
  uint nb_rowsb = 20;
  uint nb_colsb = 20;
  for (uint i = 0; i < nb_rowsb; i++) {
    for (uint j = 0; j < nb_colsb; j++) {
      square s1 = game_get_square(g1b, i, j);
      square s2 = game_get_square(g2b, i, j);
      if (s1 != s2) {
        game_delete(g2b);
        return EXIT_FAILURE;
      }
      if (s1 == TENT) {
        nb_tents1b++;
      }
      if (s2 == TENT) {
        nb_tents2b++;
      }
      if (nb_tents1b++ != nb_tents2b++) {
        game_delete(g2b);
        return EXIT_FAILURE;
      }
    }
  }
  assert(game_is_diagadj(g1b) == game_is_diagadj(g2b));
  assert(game_is_wrapping(g1b) == game_is_wrapping(g2b));
  game_delete(g2b);

  /* IF WRAPPING=TRUE AND DIAGADJ=TRUE */

  game g1c = game_new_empty_ext(20, 20, true, true);
  game g2c = game_copy(g1c);
  assert(game_equal(g1c, g2c) == true);
  game_play_move(g1c, 1, 1, TENT);
  game_play_move(g1c, 3, 5, GRASS);
  game_play_move(g2c, 1, 1, TENT);
  game_play_move(g2c, 3, 5, GRASS);
  if (g1c == NULL || g2c == NULL) {
    return EXIT_FAILURE;
  }
  int nb_tents1c = 0;
  int nb_tents2c = 0;
  uint nb_rowsc = 20;
  uint nb_colsc = 20;
  for (uint i = 0; i < nb_rowsc; i++) {
    for (uint j = 0; j < nb_colsc; j++) {
      square s1 = game_get_square(g1c, i, j);
      square s2 = game_get_square(g2c, i, j);
      if (s1 != s2) {
        game_delete(g2c);
        return EXIT_FAILURE;
      }
      if (s1 == TENT) {
        nb_tents1c++;
      }
      if (s2 == TENT) {
        nb_tents2c++;
      }
      if (nb_tents1c++ != nb_tents2c++) {
        game_delete(g2c);
        return EXIT_FAILURE;
      }
    }
  }
  assert(game_is_diagadj(g1c) == game_is_diagadj(g2c));
  assert(game_is_wrapping(g1c) == game_is_wrapping(g2c));
  game_delete(g2c);

  /* IF WRAPPING=FALSE AND DIAGADJ=TRUE */

  game g1d = game_new_empty_ext(20, 20, false, true);
  game g2d = game_copy(g1d);
  assert(game_equal(g1d, g2d) == true);
  game_play_move(g1d, 1, 1, TENT);
  game_play_move(g1d, 3, 5, GRASS);
  game_play_move(g2d, 1, 1, TENT);
  game_play_move(g2d, 3, 5, GRASS);
  if (g1d == NULL || g2d == NULL) {
    return EXIT_FAILURE;
  }
  int nb_tents1d = 0;
  int nb_tents2d = 0;
  uint nb_rowsd = 20;
  uint nb_cols = 20;
  for (uint i = 0; i < nb_rowsd; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      square s1 = game_get_square(g1d, i, j);
      square s2 = game_get_square(g2d, i, j);
      if (s1 != s2) {
        game_delete(g2d);
        return EXIT_FAILURE;
      }
      if (s1 == TENT) {
        nb_tents1d++;
      }
      if (s2 == TENT) {
        nb_tents2d++;
      }
      if (nb_tents1d++ != nb_tents2d++) {
        game_delete(g2d);
        return EXIT_FAILURE;
      }
    }
  }
  assert(game_is_diagadj(g1d) == game_is_diagadj(g2d));
  assert(game_is_wrapping(g1d) == game_is_wrapping(g2d));
  game_delete(g2d);

  // check if g1a,g1b,g1c,g1d are all equal, if not delete them
  assert(!game_equal(g1a, g1b));
  assert(!game_equal(g1a, g1c));
  assert(!game_equal(g1a, g1d));
  assert(!game_equal(g1b, g1c));
  assert(!game_equal(g1b, g1d));
  assert(!game_equal(g1c, g1d));

  game_delete(g1a);
  game_delete(g1b);
  game_delete(g1c);
  game_delete(g1d);

  return EXIT_SUCCESS;
}

/**
 * @brief test if a game gets the current number of tents in a given column. At
 *each loop, we increase the number of tent by one if the square in the grid is
 *a tent. Then, outside the loop, we check if the number of tents is equal to
 *the current number of tents in the game_default.
 * @param game_def the game default
 * @param j column index
 * @return true if the game has the current number of tent in a given column.
 * @pre @p j < nb_cols
 **/

int test_game_get_current_nb_tents_col(void) {
  game game_def = game_default();
  uint j = 0;
  if (game_def == NULL) {
    return EXIT_FAILURE;
  }
  // play a move in the game_default
  game_play_move(game_def, 0, 0, TENT);
  game_play_move(game_def, 2, 0, TENT);
  game_play_move(game_def, 4, 0, TENT);
  game_play_move(game_def, 6, 0, TENT);
  int nb_tents = 0;
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    if (game_get_square(game_def, i, j) == TENT) {
      nb_tents++;
    }
  }
  if (nb_tents != game_get_current_nb_tents_col(game_def, j)) {
    game_delete(game_def);
    return EXIT_FAILURE;
  }
  game_delete(game_def);
  return EXIT_SUCCESS;
}

/**
 * @brief test if a game gets the number of tents in the whole grid. At each
 *loop, we increase by one the current number of tents in a row (resp.column)
 *and then we check if the number of row (resp. column) is equal to the current
 *number of tent in the whole grid in a given row/column.
 * @param game_def the game default
 * @return true if the game has the current number of tents
 * @pre @p g must be a valid pointer toward a game structure.
 **/

int test_game_get_current_nb_tents_all(void) {
  game game_def = game_default();
  if (game_def == NULL) {
    return EXIT_FAILURE;
  }
  int sum_row = 0;
  int sum_col = 0;
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    sum_col += game_get_current_nb_tents_col(game_def, i);
    sum_row += game_get_current_nb_tents_row(game_def, i);
  }
  if (sum_row != game_get_current_nb_tents_all(game_def) ||
      sum_col != game_get_current_nb_tents_all(game_def)) {
    game_delete(game_def);
    return EXIT_FAILURE;
  }
  game_delete(game_def);
  return EXIT_SUCCESS;
}

/* ********** MAIN ROUTINE ********** */

int main(int argc, char *argv[]) {
  if (argc == 1) {
    usage(argc, argv);
  }

  /* INITIALISATION */

  // start test
  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
  if (strcmp("game_copy", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_copy();
  }
  if (strcmp("game_print", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_print();
  }
  if (strcmp("game_get_current_nb_tents_col", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_get_current_nb_tents_col();
  }
  if (strcmp("game_equal", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_equal();
  }
  if (strcmp("game_get_current_nb_tents_all", argv[1]) == 0) {
    fprintf(stderr, "Test \"%s\" \n", argv[1]);
    return test_game_get_current_nb_tents_all();
  }

  fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
  return EXIT_FAILURE;
}