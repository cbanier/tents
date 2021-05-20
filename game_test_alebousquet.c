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
/* ********** USAGE ********** */

void usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

/* ********** TEST FUNCTIONS ********** */

bool test_game_delete(void);
bool test_game_set_square(void);
bool test_game_get_square();
bool test_game_is_over(void);
bool test_game_restart(void);

/* ********** TEST GAME ********** */

bool test_game_delete(void) {
  game g = game_new_empty_ext(39, 2, true, false);
  if (g == NULL) {
    game_delete(g);
    return false;
  }
  game_delete(g);
  return true;
}

bool test_game_get_current_nb_tents_row(void) {
  game g = game_new_empty();
  uint i = 0;
  if (g == NULL) {
    game_delete(g);
    return false;
  }
  // Jouer deux coups
  game_play_move(g, i, 0, TENT);
  game_play_move(g, i, game_nb_cols(g) - 1, TENT);

  // Vérifier le bon nombre de tentes sur la ligne
  int nb_tents = 0;
  for (int j = 0; j < game_nb_cols(g); j++) {
    if (game_get_square(g, i, j) == TENT) {
      nb_tents++;
    }
  }
  if (nb_tents != game_get_current_nb_tents_row(g, i)) {
    game_delete(g);
    return false;
  }
  game_delete(g);
  return true;
}

bool test_game_set_square(void) {
  game g = game_new_empty();
  // "Random" pour set un square
  uint i = rand() % game_nb_cols(g);
  uint j = rand() % game_nb_rows(g);
  square s = rand() % 4;
  if (g == NULL) {
    game_delete(g);
    return false;
  }
  // Appel de get_square pour vérifier que c'est le même square
  game_set_square(g, i, j, s);
  if (game_get_square(g, i, j) != s) {
    game_delete(g);
    return false;
  }
  game_delete(g);
  return true;
}

bool test_game_get_square(void) {
  game g = game_new_empty();
  uint i = 0;
  uint j = 0;
  if (g == NULL) {
    game_delete(g);
    return false;
  }
  // set une ligne avec que des tentes
  uint index = 0;
  for (int ind = 0; ind < game_nb_cols(g); ind++) {
    game_set_square(g, i, ind, TENT);
  }
  // VERIFICATION
  while (index < game_nb_cols(g)) {
    if (game_get_square(g, i, index) != TENT) {
      game_delete(g);
      return false;
    }
    index++;
  }
  // set une colonne avec que des tentes
  for (int ind = 0; ind < game_nb_rows(g); ind++) {
    game_set_square(g, ind, j, TENT);
  }
  // VERIFICATION
  while (index < game_nb_rows(g)) {
    if (game_get_square(g, index, j) != TENT) {
      game_delete(g);
      return false;
    }
    index++;
  }
  game_delete(g);
  return true;
}

bool test_game_is_over(void) {
  // cas wrapping
  square s[9] = {TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
  uint nb_tent_row[3] = {1, 0, 0};
  uint nb_tent_col[3] = {0, 0, 1};
  game g_wrapping =
      game_new_ext(3, 3, s, nb_tent_row, nb_tent_col, true, false);
  game_play_move(g_wrapping, 0, 2, TENT);
  game_play_move(g_wrapping, 0, 1, GRASS);
  assert(game_is_over(g_wrapping));
  game_delete(g_wrapping);

  // cas diagadj
  square s2[9] = {TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY};
  uint nb_tent_row2[3] = {1, 1, 0};
  uint nb_tent_col2[3] = {1, 1, 0};
  game g_diagadj =
      game_new_ext(3, 3, s2, nb_tent_row2, nb_tent_col2, false, true);
  game_play_move(g_diagadj, 0, 1, TENT);
  game_play_move(g_diagadj, 1, 0, TENT);
  game_play_move(g_diagadj, 0, 2, GRASS);
  assert(game_is_over(g_diagadj));
  game_delete(g_diagadj);

  // cas diagadj+wrapping
  square s3[9] = {EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY};
  uint nb_tent_row3[3] = {1, 0, 1};
  uint nb_tent_col3[3] = {1, 0, 1};
  game g_diagwrap =
      game_new_ext(3, 3, s3, nb_tent_row3, nb_tent_col3, true, true);
  game_play_move(g_diagwrap, 0, 0, TENT);
  game_play_move(g_diagwrap, 2, 2, TENT);
  game_play_move(g_diagwrap, 0, 1, GRASS);
  assert(game_is_over(g_diagwrap));
  game_delete(g_diagwrap);

  // Test avec game default;
  game g = game_default_solution();
  if (g == NULL) {
    game_delete(g);
    return false;
  }
  game_play_move(g, 0, 2, GRASS);

  if (game_get_square(g, 0, 0) == TENT && game_get_square(g, 0, 3) == TENT &&
      game_get_square(g, 0, 6) == TENT && game_get_square(g, 2, 0) == TENT &&
      game_get_square(g, 2, 0) == TENT && game_get_square(g, 2, 3) == TENT &&
      game_get_square(g, 2, 5) == TENT && game_get_square(g, 2, 7) == TENT &&
      game_get_square(g, 4, 0) == TENT && game_get_square(g, 4, 2) == TENT &&
      game_get_square(g, 4, 4) == TENT && game_get_square(g, 4, 6) == TENT &&
      game_get_square(g, 6, 0) == TENT) {
    for (uint i = 0; i < DEFAULT_SIZE; i++) {
      if (game_get_current_nb_tents_row(g, i) !=
          game_get_expected_nb_tents_row(g, i)) {
        game_delete(g);
        return false;
      }
    }
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_current_nb_tents_col(g, j) !=
          game_get_expected_nb_tents_col(g, j)) {
        game_delete(g);
        return false;
      }
    }
    assert(game_is_over(g));
    game_delete(g);
    return true;
  }
  game_delete(g);
  return false;
}

bool test_game_restart(void) {
  // initialize une game "random"
  game g = game_new_empty_ext(9, 9, false, true);
  game_play_move(g, 0, 0, GRASS);
  // test si game_restart reset the game_history
  game_undo(g);
  game_restart(g);
  game copy = game_new_empty_ext(game_nb_rows(g), game_nb_cols(g), false, true);
  game_redo(g);
  if (!game_equal(g, copy)) {
    game_delete(copy);
    game_delete(g);
    return false;
  }
  game_delete(copy);
  game_delete(g);
  return true;
}

bool test_game_save() {
  game g = game_default_solution();
  game_save(g, "../data/game4.tnt");
  game_delete(g);
  return true;
}

/* ********** MAIN ROUTINE ********** */

int main(int argc, char *argv[]) {
  if (argc == 1) usage(argc, argv);
  // start test
  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
  bool ok = false;
  srand(time(NULL));
  if (strcmp("game_delete", argv[1]) == 0) {
    ok = test_game_delete();
  } else if (strcmp("game_get_current_nb_tents_row", argv[1]) == 0) {
    ok = test_game_get_current_nb_tents_row();
  } else if (strcmp("game_set_square", argv[1]) == 0) {
    ok = test_game_set_square();
  } else if (strcmp("game_get_square", argv[1]) == 0) {
    ok = test_game_get_square();
  } else if (strcmp("game_is_over", argv[1]) == 0) {
    ok = test_game_is_over();
  } else if (strcmp("game_restart", argv[1]) == 0) {
    ok = test_game_restart();
  } else if (strcmp("game_save", argv[1]) == 0) {
    ok = test_game_save();
  }

  else {
    fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  if (ok) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}