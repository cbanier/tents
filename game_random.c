/**
 * @file game_random.c
 * @brief Generate a random game, print it and optionally save it.
 *
 **/

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_prof.h"
#include "game_tools.h"
#include "queue.h"

/* command usage */
void usage(int argc, char *argv[]) {
  printf(
      "Usage %s <nb_rows> <nb_cols> <wrapping> <diagadj> <nb_trees> "
      "<with_solution> [<filename>] \n",
      argv[0]);
  printf("Example: %s 4 4 0 0 2 1 random.tnt\n", argv[0]);
  exit(EXIT_FAILURE);
}

/* main routine */
int main(int argc, char *argv[]) {
  if (argc < 7 || argc > 8) usage(argc, argv);

  /* parse args */
  int nb_rows = atoi(argv[1]);
  int nb_cols = atoi(argv[2]);
  bool wrapping = atoi(argv[3]);
  bool diagadj = atoi(argv[4]);
  int nb_trees = atoi(argv[5]);
  bool with_solution = atoi(argv[6]);
  game g =
      game_random(nb_rows, nb_cols, wrapping, diagadj, nb_trees, with_solution);
  game_print(g);
  return EXIT_SUCCESS;
}
