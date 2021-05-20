#include "game_tools.h"

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
#include "queue.h"

/* ************************************************************************** */
/*                                 GAME LOAD                                  */
/* ************************************************************************** */

square square_to_char(char s) {
  switch (s) {
    case '*':
      return TENT;
    case 'x':
      return TREE;
    case '-':
      return GRASS;
  }
  return EMPTY;
}

/* ************************************************************************** */

game game_load(char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "MAUVAIT NOM FICHIER : %s", filename);
    return NULL;
  }
  char c1, c2;
  uint nb_cols_char;
  uint nb_rows_char;
  int diagadj_char = false;
  int wrapping_char = false;

  fscanf(file, "%u %u %d %d", &nb_rows_char, &nb_cols_char, &wrapping_char,
         &diagadj_char);
  game game_loaded = game_new_empty_ext(nb_rows_char, nb_cols_char,
                                        wrapping_char, diagadj_char);
  uint tmp;
  char s;
  for (uint i = 0; i < nb_rows_char; i++) {
    fscanf(file, "%u", &tmp);
    game_set_expected_nb_tents_row(game_loaded, i, tmp);
  }
  for (uint j = 0; j < nb_cols_char; j++) {
    fscanf(file, "%u", &tmp);
    game_set_expected_nb_tents_col(game_loaded, j, tmp);
  }
  fscanf(file, "%c%c", &c1, &c2);
  for (uint i = 0; i < nb_rows_char; i++) {
    for (uint j = 0; j < nb_cols_char; j++) {
      fscanf(file, "%c", &s);
      square square_char = square_to_char(s);
      game_set_square(game_loaded, i, j, square_char);
    }
    // scanner le retour a la ligne ('\n')
    fscanf(file, "%c", &c1);
  }
  fclose(file);
  return game_loaded;
}

/* ************************************************************************** */
/*                                 GAME SAVE                                  */
/* ************************************************************************** */

char char_to_square(square s) {
  switch (s) {
    case 0:
      return ' ';
    case 1:
      return 'x';
    case 2:
      return '*';
    case 3:
      break;
  }
  return '-';
}

/* ************************************************************************** */

void game_save(cgame g, char *filename) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "MAUVAIT NOM FICHIER : %s", filename);
    exit(EXIT_FAILURE);
    fclose(file);
  }
  fprintf(file, "%u %u %u %u\n", game_nb_rows(g), game_nb_cols(g),
          game_is_wrapping(g), game_is_diagadj(g));
  for (uint i = 0; i < game_nb_rows(g); i++) {
    fprintf(file, "%u ", game_get_expected_nb_tents_row(g, i));
  }
  fputs("\n", file);
  for (uint j = 0; j < game_nb_cols(g); j++) {
    fprintf(file, "%u ", game_get_expected_nb_tents_col(g, j));
  }
  fputs("\n", file);
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      char char_square = char_to_square(game_get_square(g, i, j));
      fprintf(file, "%c", char_square);
    }
    fputs("\n", file);
  }
  fclose(file);
}

/* ************************************************************************** */
/*                                 GAME SOLVE                                 */
/* ************************************************************************** */

int GREENER(game g) {
  int taille = 0;
  int check_move_result;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      check_move_result = game_check_move(g, i, j, TENT);
      if (check_move_result == LOSING) {
        game_set_square(g, i, j, GRASS);
      }
      if (game_get_square(g, i, j) == EMPTY) {
        (taille)++;
      }
    }
  }
  // game_print(g);
  return taille;
}

/* ************************************************************************** */

// tableau de taille DISONS 10 pour tester
bool greener(game g, point *tableau_candidate_position, int *taille,
             int indice) {
  int check_move_result_tent, check_move_result_grass;
  point p;
  for (uint i = indice + 1; i < (*taille); i++) {
    p = tableau_candidate_position[i];
    check_move_result_tent = game_check_move(g, p.i, p.j, TENT);
    check_move_result_grass = game_check_move(g, p.i, p.j, GRASS);
    if (check_move_result_tent == LOSING && check_move_result_grass == LOSING) {
      return true;
    }
    if (check_move_result_tent == LOSING) {
      game_set_square(g, p.i, p.j, GRASS);
      continue;
    }
    if (check_move_result_grass == LOSING) {
      game_set_square(g, p.i, p.j, TENT);
      continue;
    }
  }
  return false;
}

/* ************************************************************************** */

void tab_indice_creator(game g, point *tableau_candidate_position,
                        point *tableau_tree_position) {
  int cpt1 = 0;
  int cpt2 = 0;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == EMPTY) {
        tableau_candidate_position[cpt1].i = i;
        tableau_candidate_position[cpt1].j = j;
        cpt1++;
      }
      if (game_get_square(g, i, j) == TREE) {
        tableau_tree_position[cpt2].i = i;
        tableau_tree_position[cpt2].j = j;
        cpt2++;
      }
    }
  }
}
/* ************************************************************************** */
// fonction qui a chaque tent qui a deux arbre voisin, regarde si au moins un
// arbres adjacent a plus de 2 tents/EMPTY orthogonalement sinon le perfect
// matching n'est pas respecté .

bool is_trees_perfect_match(game g, point p, bool wrapping) {
  point UP_SQUARE;
  bool up = false;
  point RIGHT_SQUARE;
  bool right = false;
  point DOWN_SQUARE;
  bool down = false;
  point LEFT_SQUARE;
  bool left = false;
  if (wrapping) {
    uint nb_rows = game_nb_rows(g);
    uint nb_cols = game_nb_cols(g);
    UP_SQUARE = (point){(p.i - 1 + nb_rows) % nb_rows, p.j};
    DOWN_SQUARE = (point){(p.i + 1) % nb_rows, p.j};
    RIGHT_SQUARE = (point){p.i, (p.j + 1) % nb_cols};
    LEFT_SQUARE = (point){p.i, (p.j - 1 + nb_cols) % nb_cols};
    up = true;
    right = true;
    left = true;
    down = true;
  }
  if (!wrapping) {
    if (_inside_neigh(g, p.i, p.j, UP)) {
      UP_SQUARE = (point){p.i - 1, p.j};
      up = !up;
    }
    if (_inside_neigh(g, p.i, p.j, DOWN)) {
      DOWN_SQUARE = (point){p.i + 1, p.j};
      down = !down;
    }
    if (_inside_neigh(g, p.i, p.j, RIGHT)) {
      RIGHT_SQUARE = (point){p.i, p.j + 1};
      right = !right;
    }
    if (_inside_neigh(g, p.i, p.j, LEFT)) {
      LEFT_SQUARE = (point){p.i, p.j - 1};
      left = !left;
    }
  }

  if (right && game_get_square(g, RIGHT_SQUARE.i, RIGHT_SQUARE.j) != TREE) {
    right = false;
  }
  if (left && game_get_square(g, LEFT_SQUARE.i, LEFT_SQUARE.j) != TREE) {
    left = false;
  }
  if (up && game_get_square(g, UP_SQUARE.i, UP_SQUARE.j) != TREE) {
    up = false;
  }
  if (down && game_get_square(g, DOWN_SQUARE.i, DOWN_SQUARE.j) != TREE) {
    down = false;
  }

  if (up) {
    if (_neigh_count(g, UP_SQUARE.i, UP_SQUARE.j, TENT, false) +
            _neigh_count(g, UP_SQUARE.i, UP_SQUARE.j, EMPTY, false) >
        1) {
      return true;
    }
  }
  if (left) {
    if (_neigh_count(g, LEFT_SQUARE.i, LEFT_SQUARE.j, TENT, false) +
            _neigh_count(g, LEFT_SQUARE.i, LEFT_SQUARE.j, EMPTY, false) >
        1) {
      return true;
    }
  }
  if (right) {
    if (_neigh_count(g, RIGHT_SQUARE.i, RIGHT_SQUARE.j, TENT, false) +
            _neigh_count(g, RIGHT_SQUARE.i, RIGHT_SQUARE.j, EMPTY, false) >
        1) {
      return true;
    }
  }
  if (down) {
    if (_neigh_count(g, DOWN_SQUARE.i, DOWN_SQUARE.j, TENT, false) +
            _neigh_count(g, DOWN_SQUARE.i, DOWN_SQUARE.j, EMPTY, false) >
        1) {
      return true;
    }
  }
  return false;
}

/* ************************************************************************** */

// Cette fonction remplie les case vide evidente et renvoir true si la game est
// cette fonction semble ne demontrer aucun avantage en pratique ...

bool fill_alone_trees(game g, point p) {
  if (!(_neigh_count(g, p.i, p.j, EMPTY, false) == 1 &&
        _neigh_count(g, p.i, p.j, TENT, false) == 0)) {
    return false;
  }

  point UP_SQUARE;
  bool up = false;
  point RIGHT_SQUARE;
  bool right = false;
  point DOWN_SQUARE;
  bool down = false;
  point LEFT_SQUARE;
  bool left = false;
  //(0,13)
  if (_inside_neigh(g, p.i, p.j, UP)) {
    UP_SQUARE = (point){p.i - 1, p.j};
    up = !up;
  }
  if (_inside_neigh(g, p.i, p.j, DOWN)) {
    DOWN_SQUARE = (point){p.i + 1, p.j};
    down = !down;
  }
  if (_inside_neigh(g, p.i, p.j, RIGHT)) {
    RIGHT_SQUARE = (point){p.i, p.j + 1};
    right = !right;
  }
  if (_inside_neigh(g, p.i, p.j, LEFT)) {
    LEFT_SQUARE = (point){p.i, p.j - 1};
    left = !left;
  }

  if (up) {
    if (game_get_square(g, UP_SQUARE.i, UP_SQUARE.j) == EMPTY) {
      if (game_check_move(g, UP_SQUARE.i, UP_SQUARE.j, TENT) == ILLEGAL) {
        return true;
      }
      game_set_square(g, UP_SQUARE.i, UP_SQUARE.j, TENT);
      return false;
    }
  }
  if (down) {
    if (game_get_square(g, DOWN_SQUARE.i, DOWN_SQUARE.j) == EMPTY) {
      if (game_check_move(g, DOWN_SQUARE.i, DOWN_SQUARE.j, TENT) == ILLEGAL) {
        return true;
      }
      game_set_square(g, DOWN_SQUARE.i, DOWN_SQUARE.j, TENT);
      return false;
    }
  }
  if (left) {
    if (game_get_square(g, LEFT_SQUARE.i, LEFT_SQUARE.j) == EMPTY) {
      if (game_check_move(g, LEFT_SQUARE.i, LEFT_SQUARE.j, TENT) == ILLEGAL) {
        return true;
      }
      game_set_square(g, LEFT_SQUARE.i, LEFT_SQUARE.j, TENT);
      return false;
    }
  }
  if (right) {
    if (game_get_square(g, RIGHT_SQUARE.i, RIGHT_SQUARE.j) == EMPTY) {
      if (game_check_move(g, RIGHT_SQUARE.i, RIGHT_SQUARE.j, TENT) == ILLEGAL) {
        return true;
      }
      game_set_square(g, RIGHT_SQUARE.i, RIGHT_SQUARE.j, TENT);
      return false;
    }
  }
  return false;
}

/* ************************************************************************** */

bool is_the_game_doomed(game g, point *tableau_tree_position,
                        point *tableau_candidate_position, int *nb_tents,
                        int *taille, int indice) {
  point p;
  bool result_tent, result_empty;
  int nb_trees_with_matching = 0;
  for (int i = 0; i < *nb_tents; i++) {
    p = tableau_tree_position[i];
    /*if (fill_alone_trees(g, p)) {
      return false;
    }*/

    result_empty = _neigh(g, p.i, p.j, EMPTY, false);
    result_tent = _neigh(g, p.i, p.j, TENT, false);
    if (result_tent) {
      nb_trees_with_matching++;
    }
    if (!result_tent && !result_empty) {
      return true;
    }
    if (!result_tent) {
      break;
    }
  }
  for (int i = 0; i < *taille; i++) {
    p = tableau_candidate_position[i];
    if (game_get_square(g, p.i, p.j) == TENT) {
      if (_neigh_count(g, p.i, p.j, TREE, false) > 1 &&
          !is_trees_perfect_match(g, p, game_is_wrapping(g))) {
        return true;
      }
    }
  }
  return false;
}

void full_reset_squares(game g, point *tableau_candidate_position, int indice,
                        int *taille) {
  point p;
  for (uint i = indice; i < (*taille); i++) {
    p = tableau_candidate_position[i];
    game_set_square(g, p.i, p.j, EMPTY);
  }
}

/* ************************************************************************** */

/* ************************************************************************** */

bool game_brute_force(game g, point *p, int indice_point,
                      point *tableau_candidate_position,
                      point *tableau_tree_position, int *taille,
                      int *nb_tents) {
  // printf("%d \n", indice_point);
  if (indice_point == ((*taille) - 1)) {
    game_set_square(g, p->i, p->j, TENT);
    if (game_is_over(g)) {
      return true;
    }
    game_set_square(g, p->i, p->j, GRASS);
    if (game_is_over(g)) {
      return true;
    }
    game_set_square(g, p->i, p->j, EMPTY);
    return false;
  }
  int game_check_tent = game_check_move(g, p->i, p->j, TENT);
  int game_check_grass = game_check_move(g, p->i, p->j, GRASS);
  if (game_check_tent == LOSING && game_check_grass == LOSING) {
    return false;
  }

  if (game_check_tent == LOSING) {
    game_set_square(g, p->i, p->j, GRASS);
    if (game_brute_force(g, &tableau_candidate_position[indice_point + 1],
                         indice_point + 1, tableau_candidate_position,
                         tableau_tree_position, taille, nb_tents)) {
      return true;
    }
    game_set_square(g, p->i, p->j, EMPTY);
    return false;
  }
  // game_check_tent != LOOSING HERE
  if (game_check_grass == LOSING) {
    game_set_square(g, p->i, p->j, TENT);

    if (game_brute_force(g, &tableau_candidate_position[indice_point + 1],
                         indice_point + 1, tableau_candidate_position,
                         tableau_tree_position, taille, nb_tents)) {
      return true;
    }
    game_set_square(g, p->i, p->j, EMPTY);
    return false;
  }
  // HERE game_check_tent != LOOSING && game_check_grass != LOOSING
  // On peut rien dire et on doit tester les deux cas

  if (is_the_game_doomed(g, tableau_tree_position, tableau_candidate_position,
                         nb_tents, taille, indice_point)) {
    return false;
  }

  // if (*cpt % 100000) game_print(g);

  // full_reset_squares(g, tableau_candidate_position, indice_point, taille);

  game_set_square(g, p->i, p->j, GRASS);
  if (game_brute_force(g, &tableau_candidate_position[indice_point + 1],
                       indice_point + 1, tableau_candidate_position,
                       tableau_tree_position, taille, nb_tents)) {
    return true;
  }
  game_set_square(g, p->i, p->j, TENT);
  if (game_brute_force(g, &tableau_candidate_position[indice_point + 1],
                       indice_point + 1, tableau_candidate_position,
                       tableau_tree_position, taille, nb_tents)) {
    return true;
  }
  // full_reset_squares(g, tableau_candidate_position, indice_point, taille);
  game_set_square(g, p->i, p->j, EMPTY);
  return false;
}

/* ************************************************************************** */
void swap(point *xp, point *yp) {
  point temp = *xp;
  *xp = *yp;
  *yp = temp;
}
/* ************************************************************************** */

void bubbleSort(point *tableau_candidat, int taille, game g) {
  for (int k = 0; k < taille - 1; k++) {
    for (int j = 0; j < taille - 1 - k; j++) {
      uint pj_1 = tableau_candidat[j + 1].i;
      uint pj = tableau_candidat[j].i;
      if (game_get_expected_nb_tents_row(g, pj) !=
              game_get_expected_nb_tents_row(g, pj_1) ||
          true) {
        swap(&tableau_candidat[j], &tableau_candidat[j + 1]);
      }
    }
  }
}

/* ************************************************************************** */
bool game_solve(game g) {
  // printf("Game to solve :\n\n");
  int taille = GREENER(g);
  // game_print(g);
  int nb_tents = game_get_expected_nb_tents_all(g);
  // printf("\ntaille :%d \n\nnombre de TENT : %d  \n\n", taille, nb_tents);
  point tableau_candidate_position[taille];
  point tableau_tree_position[nb_tents];

  tab_indice_creator(g, tableau_candidate_position, tableau_tree_position);

  bubbleSort(tableau_candidate_position, taille, g);
  bubbleSort(tableau_tree_position, nb_tents, g);

  bool result = game_brute_force(g, &tableau_candidate_position[0], 0,
                                 tableau_candidate_position,
                                 tableau_tree_position, &taille, &nb_tents);
  // printf("nombre d'appel rec :%llu \n\n", cpt);
  if (!result) game_restart(g);
  // game_print(g);
  return result;
}

/* ************************************************************************** */

void game_brute_force_cpt(game g, point p, int indice_point,
                          point *tableau_candidate_position, int *taille,
                          uint *cpt) {
  if (indice_point == ((*taille) - 1)) {
    game_set_square(g, p.i, p.j, TENT);
    if (game_is_over(g)) {
      ((*cpt)++);
      game_set_square(g, p.i, p.j, EMPTY);
      return;
    }
    game_set_square(g, p.i, p.j, GRASS);
    if (game_is_over(g)) {
      ((*cpt)++);
      game_set_square(g, p.i, p.j, EMPTY);

      return;
    }
    game_set_square(g, p.i, p.j, EMPTY);
    return;
  }
  int game_check_tent = game_check_move(g, p.i, p.j, TENT);
  int game_check_grass = game_check_move(g, p.i, p.j, GRASS);
  if (game_check_tent == LOSING && game_check_grass == LOSING) {
    return;
  }
  if (game_check_tent == LOSING) {
    game_set_square(g, p.i, p.j, GRASS);
    game_brute_force_cpt(g, tableau_candidate_position[indice_point + 1],
                         indice_point + 1, tableau_candidate_position, taille,
                         cpt);
    game_set_square(g, p.i, p.j, EMPTY);
    return;
  }
  // game_check_tent != LOOSING HERE
  if (game_check_grass == LOSING) {
    game_set_square(g, p.i, p.j, TENT);
    game_brute_force_cpt(g, tableau_candidate_position[indice_point + 1],
                         indice_point + 1, tableau_candidate_position, taille,
                         cpt);
    game_set_square(g, p.i, p.j, EMPTY);
    return;
  }
  // HERE game_check_tent != LOOSING && game_check_grass != LOOSING
  // On peut rien dire et on doit tester les deux cas
  game_set_square(g, p.i, p.j, TENT);
  game_brute_force_cpt(g, tableau_candidate_position[indice_point + 1],
                       indice_point + 1, tableau_candidate_position, taille,
                       cpt);

  game_set_square(g, p.i, p.j, GRASS);

  game_brute_force_cpt(g, tableau_candidate_position[indice_point + 1],
                       indice_point + 1, tableau_candidate_position, taille,
                       cpt);

  game_set_square(g, p.i, p.j, EMPTY);
  return;
}

/* ************************************************************************** */

uint game_nb_solutions(game g) {
  uint cpt = 0;
  int taille = GREENER(g);
  // printf("taille :%d", taille);
  point tableau_candidate_position[taille];
  point tableau_tree_position[taille];
  tab_indice_creator(g, tableau_candidate_position, tableau_tree_position);
  game_brute_force_cpt(g, tableau_candidate_position[0], 0,
                       tableau_candidate_position, &taille, &cpt);
  game_restart(g);
  return cpt;
}

/* ************************************************************************** */
/*                               GAME RANDOM                                  */
/* ************************************************************************** */

game game_random(uint nb_rows, uint nb_cols, bool wrapping, bool diagadj,
                 uint nb_trees, bool with_solution) {
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping, diagadj);
  int num_tree = 0;
  while (num_tree < nb_trees) {
    uint i;
    uint j;
    uint itree, jtree;
    int direction, di, dj;
    bool pseudo_valid = false;
    do {
      pseudo_valid = true;
      i = rand() % nb_rows;
      j = rand() % nb_cols;
      direction = rand() % 4;
      int dx[] = {0, 0, 1, -1};
      int dy[] = {1, -1, 0, 0};
      di = dx[direction];
      dj = dy[direction];
      itree = i + di;
      jtree = j + dj;
      if (wrapping) {
        itree = (itree + nb_rows) % nb_rows;
        jtree = (jtree + nb_cols) % nb_cols;
      }
      pseudo_valid = pseudo_valid && (itree >= 0 && itree < nb_rows);
      pseudo_valid = pseudo_valid && (jtree >= 0 && jtree < nb_cols);
      pseudo_valid = pseudo_valid && game_get_square(g, i, j) == EMPTY;
      pseudo_valid = pseudo_valid && game_get_square(g, itree, jtree) == EMPTY;
    } while (!pseudo_valid);
    game_set_square(g, i, j, TENT);
    game_set_expected_nb_tents_row(g, i,
                                   game_get_expected_nb_tents_row(g, i) + 1);
    game_set_expected_nb_tents_col(g, j,
                                   game_get_expected_nb_tents_col(g, j) + 1);
    game_set_square(g, itree, jtree, TREE);
    if (game_is_over(g)) {
      num_tree++;
    } else {
      game_set_square(g, i, j, EMPTY);
      game_set_expected_nb_tents_row(g, i,
                                     game_get_expected_nb_tents_row(g, i) - 1);
      game_set_expected_nb_tents_col(g, j,
                                     game_get_expected_nb_tents_col(g, j) - 1);
      game_set_square(g, itree, jtree, EMPTY);
    }
  }
  if (!with_solution) game_restart(g);
  return g;
}

/* ************************************************************************** */
/*                                GAME TOOLS                                  */
/* ************************************************************************** */

// Get a random number from rmin to rmax
int randInt(int rmin, int rmax) { return rand() % (rmax - rmin + 1) + rmin; }

// Set the random number for the game
int *setArgs(void) {
  srand((unsigned int)time(NULL));
  int *p = malloc(2 * sizeof(int));
  assert(p != NULL);
  p[0] = randInt(12, 20);
  p[1] = randInt(1, 2);
  return p;
}

// Set the game arguments in a tab
int *game_args(void) {
  int *args = setArgs();
  int *tab = malloc(2 * sizeof(int));
  assert(tab != NULL);
  tab[0] = args[0];
  tab[1] = args[1];
  free(args);
  return tab;
}
/* **************************************************************** */