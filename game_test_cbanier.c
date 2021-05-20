#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_prof.h"
#include "game_tools.h"

#include "time.h"

void usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

/* ********** TESTS ********** */
int test_game_default(void) {
  game game_def = game_default();
  uint nb_tents_row[DEFAULT_SIZE] = {3, 0, 4, 0, 4, 0, 1, 0};
  uint nb_tents_col[DEFAULT_SIZE] = {4, 0, 1, 2, 1, 1, 2, 1};
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    if (nb_tents_col[i] != game_get_expected_nb_tents_col(game_def, i) ||
        nb_tents_row[i] != game_get_expected_nb_tents_row(game_def, i)) {
      game_delete(game_def);
      return EXIT_FAILURE;
    }
  }
  if (game_get_square(game_def, 0, 4) == TREE &&
      game_get_square(game_def, 0, 5) == TREE &&
      game_get_square(game_def, 1, 0) == TREE &&
      game_get_square(game_def, 1, 7) == TREE &&
      game_get_square(game_def, 2, 4) == TREE &&
      game_get_square(game_def, 3, 0) == TREE &&
      game_get_square(game_def, 3, 5) == TREE &&
      game_get_square(game_def, 4, 1) == TREE &&
      game_get_square(game_def, 5, 0) == TREE &&
      game_get_square(game_def, 5, 4) == TREE &&
      game_get_square(game_def, 5, 6) == TREE &&
      game_get_square(game_def, 7, 0) == TREE) {
    // try to put some tent or grass
    game_set_square(game_def, 0, 0, TENT);
    game_set_square(game_def, 0, 1, EMPTY);
    game_set_square(game_def, 1, 1, GRASS);
    assert(game_get_square(game_def, 0, 0) == TENT);
    assert(game_get_square(game_def, 0, 1) == EMPTY);
    assert(game_get_square(game_def, 1, 1) == GRASS);
    game_delete(game_def);
    return EXIT_SUCCESS;
  } else {
    game_delete(game_def);
    return EXIT_FAILURE;
  }
}

int test_game_new(void) {
  if (test_game_default() == EXIT_SUCCESS) {
    game game_def = game_default();
    square default_square[64];
    for (uint i = 0; i < 64; i++) {
      default_square[i] = EMPTY;
    }
    uint nb_tents_row[DEFAULT_SIZE] = {3, 0, 4, 0, 4, 0, 1, 0};
    uint nb_tents_col[DEFAULT_SIZE] = {4, 0, 1, 2, 1, 1, 2, 1};
    game new = game_new(default_square, nb_tents_row, nb_tents_col);
    assert(new != NULL);
    game_set_square(new, 0, 4, TREE);
    game_set_square(new, 0, 5, TREE);
    game_set_square(new, 1, 0, TREE);
    game_set_square(new, 1, 7, TREE);
    game_set_square(new, 2, 4, TREE);
    game_set_square(new, 3, 0, TREE);
    game_set_square(new, 3, 5, TREE);
    game_set_square(new, 4, 1, TREE);
    game_set_square(new, 5, 0, TREE);
    game_set_square(new, 5, 4, TREE);
    game_set_square(new, 5, 6, TREE);
    game_set_square(new, 7, 0, TREE);

    game_set_square(game_def, 0, 0, TENT);
    game_set_square(new, 0, 0, TENT);
    game_set_square(game_def, 0, 1, EMPTY);
    game_set_square(new, 0, 1, EMPTY);
    game_set_square(game_def, 1, 1, GRASS);
    game_set_square(new, 1, 1, GRASS);

    for (uint i = 0; i < DEFAULT_SIZE; i++) {
      assert(game_get_current_nb_tents_col(game_def, i) ==
             game_get_current_nb_tents_col(new, i));
      assert(game_get_current_nb_tents_row(game_def, i) ==
             game_get_current_nb_tents_row(new, i));
      assert(game_get_expected_nb_tents_col(game_def, i) ==
             game_get_expected_nb_tents_col(new, i));
      assert(game_get_expected_nb_tents_row(game_def, i) ==
             game_get_expected_nb_tents_row(new, i));
      for (uint j = 0; j < DEFAULT_SIZE; j++) {
        assert(game_get_square(game_def, i, j) == game_get_square(new, i, j));
      }
    }
    game_delete(game_def);
    game_delete(new);
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}

int test_game_new_empty(void) {
  game gameE = game_new_empty();
  assert(gameE != NULL);
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    assert(game_get_current_nb_tents_col(gameE, i) == 0);
    assert(game_get_current_nb_tents_row(gameE, i) == 0);
    assert(game_get_expected_nb_tents_col(gameE, i) == 0);
    assert(game_get_expected_nb_tents_row(gameE, i) == 0);
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      assert(game_get_square(gameE, i, j) == EMPTY);
    }
  }
  assert(game_get_current_nb_tents_all(gameE) == 0);
  assert(game_get_expected_nb_tents_all(gameE) == 0);
  game_delete(gameE);
  return EXIT_SUCCESS;
}

int test_game_check_move_2x2(void) {
  // Test pour une game 2x2
  game g1 = game_new_empty_ext(2, 2, false, false);
  game_set_expected_nb_tents_row(g1, 0, 1);
  game_set_expected_nb_tents_row(g1, 1, 0);
  game_set_expected_nb_tents_col(g1, 0, 0);
  game_set_expected_nb_tents_col(g1, 1, 1);
  game_set_square(g1, 0, 0, TREE);
  assert(game_check_move(g1, 0, 1, TENT) == REGULAR);
  game_delete(g1);
  // Test pour une game2x?
  game g2 = game_new_empty_ext(2, 6, true, true);
  game_set_expected_nb_tents_row(g2, 0, 1);
  game_set_expected_nb_tents_row(g2, 1, 2);
  game_set_expected_nb_tents_col(g2, 0, 0);
  game_set_expected_nb_tents_col(g2, 1, 1);
  game_set_expected_nb_tents_col(g2, 2, 1);
  game_set_expected_nb_tents_col(g2, 3, 0);
  game_set_expected_nb_tents_col(g2, 4, 0);
  game_set_expected_nb_tents_col(g2, 5, 1);
  game_set_square(g2, 0, 0, TREE);
  game_set_square(g2, 0, 2, TREE);
  game_set_square(g2, 1, 4, TREE);
  assert(game_check_move(g2, 0, 1, TENT) == REGULAR);
  game_play_move(g2, 0, 1, TENT);
  game_play_move(g2, 1, 5, TENT);
  assert(game_check_move(g2, 1, 2, TENT) == REGULAR);
  game_delete(g2);
  // Test pour une game ?x2
  game g3 = game_new_empty_ext(4, 2, true, false);
  game_set_expected_nb_tents_row(g3, 0, 0);
  game_set_expected_nb_tents_row(g3, 1, 0);
  game_set_expected_nb_tents_row(g3, 2, 1);
  game_set_expected_nb_tents_row(g3, 3, 0);
  game_set_expected_nb_tents_col(g3, 0, 0);
  game_set_expected_nb_tents_col(g3, 1, 1);
  game_set_square(g3, 1, 1, TREE);
  game_fill_grass_col(g3, 0);
  assert(game_check_move(g3, 2, 1, TENT) == REGULAR);
  assert(game_check_move(g3, 1, 1, TENT) == ILLEGAL);
  game_delete(g3);

  return EXIT_SUCCESS;
}

int test_game_check_move(void) {
  // Test pour sur game_default
  game g = game_default();
  assert(game_check_move(g, 0, 4, TENT) == ILLEGAL);
  game_play_move(g, 0, 0, TENT);
  game_play_move(g, 2, 0, TENT);
  game_play_move(g, 4, 0, TENT);
  assert(game_check_move(g, 6, 0, TENT) == REGULAR);
  assert(game_check_move(g, 6, 0, EMPTY) == REGULAR);
  game_delete(g);

  game g_bis = game_default_solution();
  assert(game_check_move(g_bis, 7, 7, TENT) == LOSING);
  assert(game_check_move(g_bis, 4, 2, GRASS) == LOSING);

  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      // test to add a TREE
      assert(game_check_move(g_bis, i, j, TREE) == ILLEGAL);
      // test to add SOMETHING on a TREE
      if (game_get_square(g_bis, i, j) == TREE) {
        assert(game_check_move(g_bis, i, j, EMPTY) == ILLEGAL);
        assert(game_check_move(g_bis, i, j, TENT) == ILLEGAL);
        assert(game_check_move(g_bis, i, j, GRASS) == ILLEGAL);
      }
    }
  }
  game_delete(g_bis);

  // Test on game with WRAPPING = DIAGADJ = TRUE
  game g2 = game_new_empty_ext(4, 8, true, true);
  game_set_square(g2, 0, 2, TREE);
  game_set_square(g2, 0, 6, TREE);
  game_set_square(g2, 1, 5, TREE);
  game_set_square(g2, 2, 2, TREE);
  game_set_square(g2, 2, 4, TREE);
  game_set_square(g2, 3, 1, TREE);
  game_set_square(g2, 3, 3, TREE);
  game_set_square(g2, 3, 5, TREE);
  game_set_square(g2, 3, 7, TREE);

  game_set_expected_nb_tents_row(g2, 0, 0);
  game_set_expected_nb_tents_row(g2, 1, 2);
  game_set_expected_nb_tents_row(g2, 2, 3);
  game_set_expected_nb_tents_row(g2, 3, 4);

  game_set_expected_nb_tents_col(g2, 0, 1);
  game_set_expected_nb_tents_col(g2, 1, 1);
  game_set_expected_nb_tents_col(g2, 2, 2);
  game_set_expected_nb_tents_col(g2, 3, 1);
  game_set_expected_nb_tents_col(g2, 4, 1);
  game_set_expected_nb_tents_col(g2, 5, 1);
  game_set_expected_nb_tents_col(g2, 6, 2);
  game_set_expected_nb_tents_col(g2, 7, 1);

  game_play_move(g2, 3, 0, TENT);
  game_play_move(g2, 3, 2, TENT);
  game_play_move(g2, 3, 4, TENT);
  assert(game_check_move(g2, 3, 6, GRASS) == LOSING);
  game_play_move(g2, 3, 6, TENT);
  game_play_move(g2, 2, 1, TENT);
  game_play_move(g2, 2, 3, TENT);
  game_play_move(g2, 2, 5, TENT);
  game_play_move(g2, 1, 2, TENT);
  game_play_move(g2, 1, 6, TENT);
  assert(game_check_move(g2, 0, 3, TENT) == LOSING);
  game_delete(g2);

  // Test on 3x3 game with WRAPPING = TRUE & DIAGADJ = FALSE
  square grid[9] = {TREE, TENT, GRASS, GRASS, TREE, GRASS, EMPTY, GRASS, TREE};
  uint nb_tents_col[3] = {1, 1, 1};
  uint nb_tents_row[3] = {1, 1, 1};
  game g3 = game_new_ext(3, 3, grid, nb_tents_row, nb_tents_col, true, false);

  assert(game_check_move(g3, 2, 2, EMPTY) == ILLEGAL);
  game_play_move(g3, 2, 0, TENT);
  assert(game_check_move(g3, 2, 0, GRASS) == LOSING);
  game_play_move(g3, 1, 2, EMPTY);
  assert(game_check_move(g3, 1, 2, GRASS) == LOSING);
  assert(game_check_move(g3, 1, 2, TENT) == LOSING);
  game_delete(g3);

  // Test on 4x5 game with WRAPPING = FALSE & DIAGADJ = TRUE
  square grid2[20] = {EMPTY, GRASS, TREE,  TENT, EMPTY, EMPTY, EMPTY,
                      EMPTY, TREE,  TREE,  TREE, GRASS, TREE,  EMPTY,
                      TENT,  TENT,  EMPTY, TENT, EMPTY, GRASS};
  uint nb_tents_row2[4] = {1, 1, 1, 2};
  uint nb_tents_col2[5] = {1, 0, 2, 1, 1};
  game g4 =
      game_new_ext(4, 5, grid2, nb_tents_row2, nb_tents_col2, false, true);
  assert(game_check_move(g4, 1, 2, TENT) == REGULAR);

  game_fill_grass_row(g4, 3);
  game_fill_grass_row(g4, 2);
  game_fill_grass_row(g4, 1);
  game_fill_grass_row(g4, 0);
  game_play_move(g4, 2, 4, EMPTY);

  assert(game_check_move(g4, 2, 1, TENT) == LOSING);
  assert(game_check_move(g4, 2, 1, GRASS) == LOSING);
  assert(game_check_move(g4, 1, 0, TENT) == LOSING);
  assert(game_check_move(g4, 2, 4, GRASS) == LOSING);
  game_delete(g4);
  return EXIT_SUCCESS;
}

int test_game_play_move(void) {
  // TEST ON EMPTY GAME
  game new = game_new_empty();
  game_play_move(new, 1, 1, TENT);
  assert(game_get_square(new, 1, 1) == TENT);
  game_play_move(new, 2, 2, GRASS);
  assert(game_get_square(new, 2, 2) == GRASS);
  game_play_move(new, 4, 4, EMPTY);
  assert(game_get_square(new, 4, 4) == EMPTY);
  game_delete(new);

  // TEST ON GAME DEF
  game g = game_default_solution();
  game_play_move(g, 0, 0, TENT);
  assert(game_get_square(g, 0, 0) == TENT);
  game_play_move(g, 0, 1, EMPTY);
  assert(game_get_square(g, 0, 1) == EMPTY);
  game_play_move(g, 0, 2, GRASS);
  assert(game_get_square(g, 0, 2) == GRASS);
  game_play_move(g, 0, 3, TENT);
  assert(game_get_square(g, 0, 3) == TENT);
  game_restart(g);

  square s;
  game_play_move(g, 0, 0, GRASS);
  assert(game_get_square(g, 0, 0) == GRASS);
  game_play_move(g, 0, 0, TENT);
  assert(game_get_square(g, 0, 0) == TENT);
  game_play_move(g, 0, 0, EMPTY);
  assert(game_get_square(g, 0, 0) == EMPTY);
  for (uint cpt = 1; cpt < 4; cpt++) {
    for (uint i = 0; i < DEFAULT_SIZE; i++) {
      for (uint j = 0; j < DEFAULT_SIZE; j++) {
        if (game_get_square(g, i, j) != TREE) {
          // on choisit au "hasard" un square, on fait en sorte de ne pas jouer
          // un TREE bien sûr
          srand(time(NULL));
          s = rand() % 4;
          while (s == TREE) {
            s = rand() % 4;
          }
          game_play_move(g, i, j, s);
          if (game_get_square(g, i, j) != s) return EXIT_FAILURE;
        }
      }
    }
  }
  game_delete(g);

  game g2 = game_new_empty();
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      srand(time(NULL));
      s = rand() % 4;
      while (s == TREE) {
        s = rand() % 4;
      }
      game_play_move(g2, i, j, s);
      if (game_get_square(g2, i, j) != s) return EXIT_FAILURE;
    }
  }
  game_delete(g2);
  return EXIT_SUCCESS;
}

// -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
// --- FONCTIONS AUXILIAIRES POUR TEST_GAME_FILL_GRASS ---
// Permet de connaître combien de cases sont remplis par la fonction
// fill_grass_row/col
uint get_nb_empty_tot(game g) {
  uint nb_empty = 0;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == EMPTY) {
        nb_empty++;
      }
    }
  }
  return nb_empty;
}
// Vérifie si on remplit correctement les lignes
int check_filling_grass_row(game g) {
  for (uint i = 0; i < game_nb_rows(g); i++) {
    game_fill_grass_row(g, i);
    for (uint j = 0; j < game_nb_cols(g); j++) {
      assert(game_get_square(g, i, j) != EMPTY);
    }
  }
  return EXIT_SUCCESS;
}
// Vérifie si on remplit correctement les colonnes
int check_filling_grass_col(game g) {
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      game_fill_grass_col(g, j);
      assert(game_get_square(g, i, j) != EMPTY);
    }
  }
  return EXIT_SUCCESS;
}
// Permet de undo le nombre de fois nécessaire pour reset les
// game_fill_grass_row/col
void do_undo(game g, uint total) {
  for (uint a = 0; a < total; a++) {
    game_undo(g);
  }
}
int test_game_fill_grass_row(void) {
  int status;
  // Test on game_default
  game g = game_default();
  game_play_move(g, 0, 0, TENT);
  game_play_move(g, 2, 0, TENT);
  game_play_move(g, 4, 0, GRASS);
  game_play_move(g, 6, 0, EMPTY);
  uint nb_empty = get_nb_empty_tot(g);
  game g_bis = game_copy(g);
  status = check_filling_grass_row(g);
  do_undo(g, nb_empty);
  assert(game_equal(g, g_bis) == true);
  game_delete(g);
  game_delete(g_bis);

  // Test on game with WRAPPING = DIAGADJ = TRUE
  game g2 = game_new_empty_ext(4, 8, true, true);
  game_play_move(g2, 0, 0, TENT);
  game_play_move(g2, 0, 1, GRASS);
  game_play_move(g2, 1, 0, EMPTY);
  nb_empty = get_nb_empty_tot(g2);
  game g2_bis = game_copy(g2);
  status = check_filling_grass_row(g2);
  do_undo(g2, nb_empty);
  assert(game_equal(g2, g2_bis) == true);
  game_delete(g2);
  game_delete(g2_bis);

  // Test on 3x3 game with WRAPPING = TRUE & DIAGADJ = FALSE
  square grid[9] = {TREE, TENT, EMPTY, EMPTY, TREE, GRASS, EMPTY, EMPTY, TREE};
  uint nb_tents_col[3] = {1, 1, 1};
  uint nb_tents_row[3] = {1, 1, 1};
  game g3 = game_new_ext(3, 3, grid, nb_tents_row, nb_tents_col, true, false);
  nb_empty = get_nb_empty_tot(g3);
  game g3_bis = game_copy(g3);
  status = check_filling_grass_row(g3);
  do_undo(g3, nb_empty);
  assert(game_equal(g3, g3_bis) == true);
  game_delete(g3);
  game_delete(g3_bis);

  // Test on 4x5 game with WRAPPING = FALSE & DIAGADJ = TRUE
  square grid2[20] = {EMPTY, GRASS, TREE,  TENT,  EMPTY, EMPTY, EMPTY,
                      EMPTY, TREE,  TREE,  TREE,  TREE,  TREE,  EMPTY,
                      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TENT};
  uint nb_tents_row2[4] = {1, 2, 3, 0};
  uint nb_tents_col2[5] = {1, 1, 2, 1, 1};
  game g4 =
      game_new_ext(4, 5, grid2, nb_tents_row2, nb_tents_col2, false, true);
  nb_empty = get_nb_empty_tot(g4);
  game g4_bis = game_copy(g4);
  status = check_filling_grass_row(g4);
  do_undo(g4, nb_empty);
  assert(game_equal(g4, g4_bis) == true);
  game_delete(g4);
  game_delete(g4_bis);

  return status;
}

int test_game_fill_grass_col(void) {
  int status;
  // Test on game_default
  game g = game_default();
  game_play_move(g, 0, 0, TENT);
  game_play_move(g, 2, 0, TENT);
  game_play_move(g, 4, 0, GRASS);
  game_play_move(g, 6, 0, EMPTY);
  uint nb_empty = get_nb_empty_tot(g);
  game g_bis = game_copy(g);
  status = check_filling_grass_col(g);
  do_undo(g, nb_empty);
  assert(game_equal(g, g_bis) == true);
  game_delete(g);
  game_delete(g_bis);

  // Test on game with WRAPPING = DIAGADJ = TRUE
  game g2 = game_new_empty_ext(4, 8, true, true);
  game_play_move(g2, 0, 0, TENT);
  game_play_move(g2, 0, 1, GRASS);
  game_play_move(g2, 1, 0, EMPTY);
  nb_empty = get_nb_empty_tot(g2);
  game g2_bis = game_copy(g2);
  status = check_filling_grass_col(g2);
  do_undo(g2, nb_empty);
  assert(game_equal(g2, g2_bis) == true);
  game_delete(g2);
  game_delete(g2_bis);

  // Test on 3x3 game with WRAPPING = TRUE & DIAGADJ = FALSE
  square grid[9] = {TREE, TENT, EMPTY, EMPTY, TREE, GRASS, EMPTY, EMPTY, TREE};
  uint nb_tents_col[3] = {1, 1, 1};
  uint nb_tents_row[3] = {1, 1, 1};
  game g3 = game_new_ext(3, 3, grid, nb_tents_row, nb_tents_col, true, false);
  nb_empty = get_nb_empty_tot(g3);
  game g3_bis = game_copy(g3);
  status = check_filling_grass_col(g3);
  do_undo(g3, nb_empty);
  assert(game_equal(g3, g3_bis) == true);
  game_delete(g3);
  game_delete(g3_bis);

  // Test on 4x5 game with WRAPPING = FALSE & DIAGADJ = TRUE
  square grid2[20] = {EMPTY, GRASS, TREE,  TENT,  EMPTY, EMPTY, EMPTY,
                      EMPTY, TREE,  TREE,  TREE,  TREE,  TREE,  EMPTY,
                      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TENT};
  uint nb_tents_row2[4] = {1, 2, 3, 0};
  uint nb_tents_col2[5] = {1, 1, 2, 1, 1};
  game g4 =
      game_new_ext(4, 5, grid2, nb_tents_row2, nb_tents_col2, false, true);
  nb_empty = get_nb_empty_tot(g4);
  game g4_bis = game_copy(g4);
  status = check_filling_grass_col(g4);
  do_undo(g4, nb_empty);
  assert(game_equal(g4, g4_bis) == true);
  game_delete(g4);
  game_delete(g4_bis);

  return status;
}

// FONCTIONS AUXILIAIRES POUR TEST_GAME_EQUAL
// Vérifie les chants de la structures initialisé au début de la partie
int check_struct_parameters(game g, game g_bis) {
  assert(game_nb_rows(g) == game_nb_rows(g_bis));
  assert(game_nb_cols(g) == game_nb_cols(g_bis));
  assert(game_is_diagadj(g) == game_is_diagadj(g_bis));
  assert(game_is_wrapping(g) == game_is_wrapping(g_bis));
  return EXIT_SUCCESS;
}
// Vérifie le nombre de tentes sur toute la grille
int check_nb_tents(game g, game g_bis) {
  assert(game_get_current_nb_tents_all(g) ==
         game_get_current_nb_tents_all(g_bis));
  assert(game_get_expected_nb_tents_all(g) ==
         game_get_expected_nb_tents_all(g_bis));
  for (uint i = 0; i < game_nb_rows(g); i++) {
    assert(game_get_current_nb_tents_row(g, i) ==
           game_get_current_nb_tents_row(g_bis, i));
    assert(game_get_expected_nb_tents_row(g, i) ==
           game_get_expected_nb_tents_row(g_bis, i));
    for (uint j = 0; j < game_nb_cols(g); j++) {
      assert(game_get_current_nb_tents_col(g, j) ==
             game_get_current_nb_tents_col(g_bis, j));
      assert(game_get_expected_nb_tents_col(g, j) ==
             game_get_expected_nb_tents_col(g_bis, j));
    }
  }
  return EXIT_SUCCESS;
}
// Vérifie la position de chaque éléments
int check_all_position(game g, game g_bis) {
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      assert(game_get_square(g, i, j) == game_get_square(g_bis, i, j));
    }
  }
  return EXIT_SUCCESS;
}
// Test game_equal
int test_game_equal(void) {
  int status;
  // Test on game_default
  game g = game_default();
  game g_bis = game_default();
  game_play_move(g, 0, 0, TENT);
  game_play_move(g, 2, 0, TENT);
  game_play_move(g, 4, 0, GRASS);
  game_play_move(g, 6, 0, EMPTY);
  // on enlève les 4 coups joués
  do_undo(g, 4);
  assert(game_equal(g, g_bis) == true);
  game_fill_grass_col(g, 4);
  game_fill_grass_col(g_bis, 4);
  game_fill_grass_row(g, 6);
  game_fill_grass_row(g_bis, 6);
  game_play_move(g, 7, 7, TENT);
  game_play_move(g_bis, 7, 7, TENT);

  assert(game_equal(g, g_bis) == true);
  assert(get_nb_empty_tot(g) == get_nb_empty_tot(g_bis));
  status = check_struct_parameters(g, g_bis);
  status = check_nb_tents(g, g_bis);
  status = check_all_position(g, g_bis);
  game_delete(g);
  game_delete(g_bis);

  // Test on game with WRAPPING = DIAGADJ = TRUE
  game g2 = game_new_empty_ext(4, 8, true, true);
  game g2_bis = game_new_empty_ext(4, 8, true, true);
  game_play_move(g2, 0, 0, TENT);
  game_play_move(g2, 0, 1, GRASS);
  game_play_move(g2, 1, 0, EMPTY);
  do_undo(g2, 3);
  assert(game_equal(g2, g2_bis) == true);
  game_fill_grass_col(g2, 2);
  game_fill_grass_col(g2_bis, 2);
  game_fill_grass_row(g2, 3);
  game_fill_grass_row(g2_bis, 3);
  game_play_move(g2, 3, 7, TENT);
  game_play_move(g2_bis, 3, 7, TENT);

  assert(game_equal(g2, g2_bis) == true);
  assert(get_nb_empty_tot(g2) == get_nb_empty_tot(g2_bis));
  status = check_struct_parameters(g2, g2_bis);
  status = check_nb_tents(g2, g2_bis);
  status = check_all_position(g2, g2_bis);
  game_delete(g2);
  game_delete(g2_bis);

  // Test on 3x3 game with WRAPPING = TRUE & DIAGADJ = FALSE
  square grid[9] = {TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE};
  uint nb_tents_col[3] = {1, 1, 1};
  uint nb_tents_row[3] = {1, 1, 1};
  game g3 = game_new_ext(3, 3, grid, nb_tents_row, nb_tents_col, true, false);
  game g3_bis =
      game_new_ext(3, 3, grid, nb_tents_row, nb_tents_col, true, false);
  game_play_move(g3, 0, 1, TENT);
  game_play_move(g3, 0, 2, GRASS);
  game_play_move(g3, 2, 1, TENT);
  do_undo(g3, 3);
  assert(game_equal(g3, g3_bis) == true);
  game_fill_grass_col(g3, 1);
  game_fill_grass_col(g3_bis, 1);
  game_fill_grass_row(g3, 2);
  game_fill_grass_row(g3_bis, 2);
  game_play_move(g3, 1, 2, TENT);
  game_play_move(g3_bis, 1, 2, TENT);

  assert(game_equal(g3, g3_bis) == true);
  assert(get_nb_empty_tot(g3) == get_nb_empty_tot(g3_bis));
  status = check_struct_parameters(g3, g3_bis);
  status = check_nb_tents(g3, g3_bis);
  status = check_all_position(g3, g3_bis);
  game_delete(g3);
  game_delete(g3_bis);

  // Test on 4x5 game with WRAPPING = FALSE & DIAGADJ = TRUE
  square grid2[20] = {EMPTY, EMPTY, TREE,  EMPTY, EMPTY, EMPTY, EMPTY,
                      EMPTY, TREE,  TREE,  TREE,  TREE,  TREE,  EMPTY,
                      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TENT};
  uint nb_tents_row2[4] = {1, 2, 3, 0};
  uint nb_tents_col2[5] = {1, 1, 2, 1, 1};
  game g4 =
      game_new_ext(4, 5, grid2, nb_tents_row2, nb_tents_col2, false, true);
  game g4_bis =
      game_new_ext(4, 5, grid2, nb_tents_row2, nb_tents_col2, false, true);
  game_play_move(g4, 0, 1, GRASS);
  game_play_move(g4, 0, 3, TENT);
  game_play_move(g4, 3, 4, GRASS);
  do_undo(g4, 3);
  assert(game_equal(g4, g4_bis) == true);
  game_fill_grass_col(g4, 1);
  game_fill_grass_col(g4_bis, 1);
  game_fill_grass_row(g4, 2);
  game_fill_grass_row(g4_bis, 2);
  game_play_move(g4, 3, 0, TENT);
  game_play_move(g4_bis, 3, 0, TENT);

  assert(game_equal(g4, g4_bis) == true);
  assert(get_nb_empty_tot(g4) == get_nb_empty_tot(g4_bis));
  status = check_struct_parameters(g4, g4_bis);
  status = check_nb_tents(g4, g4_bis);
  status = check_all_position(g4, g4_bis);
  game_delete(g4);
  game_delete(g4_bis);

  return status;
}

/* ********** MAIN ROUTINE ********** */
int main(int argc, char *argv[]) {
  if (argc == 1) {
    usage(argc, argv);
  }
  /* INITIALISATION */
  int ok = EXIT_FAILURE;
  // start test
  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
  if (strcmp("game_new", argv[1]) == 0) {
    ok = test_game_new();
  }
  if (strcmp("game_new_empty", argv[1]) == 0) {
    ok = test_game_new_empty();
  }
  if (strcmp("game_check_move_2x2", argv[1]) == 0) {
    ok = test_game_check_move_2x2();
  }
  if (strcmp("game_check_move", argv[1]) == 0) {
    ok = test_game_check_move();
  }
  if (strcmp("game_play_move", argv[1]) == 0) {
    ok = test_game_play_move();
  }
  if (strcmp("game_fill_grass_row", argv[1]) == 0) {
    ok = test_game_fill_grass_row();
  }
  if (strcmp("game_fill_grass_col", argv[1]) == 0) {
    ok = test_game_fill_grass_col();
  }
  if (strcmp("game_equal", argv[1]) == 0) {
    ok = test_game_equal();
  }
  if (strcmp("game_default", argv[1]) == 0) {
    ok = test_game_default();
  }
  if (ok == EXIT_SUCCESS) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}