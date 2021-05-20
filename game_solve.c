#include "game.h"
#include "game_aux.c"
#include "game_ext.h"
#include "game_prof.h"
#include "game_tools.h"

#include <getopt.h>

/* ************************************************************************** */
/*                             GAME SOLVE MAIN                                */
/* ************************************************************************** */

void usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s <option> <input> [<output>] \n", argv[0]);
  fprintf(stderr, "-s: solver \n");
  fprintf(stderr, "-c: find solutions number\n");
  exit(EXIT_FAILURE);
}

/* ************************************************************************** */

int main(int argc, char *argv[]) {
  if (argc >= 5) {
    usage(argc, argv);
  }
  /* parse args */
  game game_to_solve = game_load(argv[2]);
  int c;
  bool game_sol;
  uint nb_sol;
  /* run solver */
  while ((c = getopt(argc, argv, "sc:")) != -1) {
    if (c == 's') goto solve;
    if (c == 'c') goto number_of_solutions;
  }

  return EXIT_FAILURE;

solve:
  printf("Game to solve :\n");
  game_print(game_to_solve);
  if (argv[3] != NULL) {
    game game_to_solve_cp = game_copy(game_to_solve);
    game_sol = game_solve(game_to_solve_cp);
    game_save(game_to_solve_cp, argv[3]);
    printf("\nSolution of the <%s> game is saved in <%s> output file.\n\n",
           argv[2], argv[3]);
    game_delete(game_to_solve_cp);
    game_delete(game_to_solve);
  } else {
    game_sol = game_solve(game_to_solve);
    if (game_sol) printf("\nHere is a solution of the <%s> game.\n", argv[2]);
    game_print(game_to_solve);
    game_delete(game_to_solve);
  }
  return !game_sol;

number_of_solutions:
  nb_sol = game_nb_solutions(game_to_solve);
  if (nb_sol < 0) return EXIT_FAILURE;
  if (argv[3] != NULL) {
    FILE *file = fopen(argv[3], "w");
    if (file == NULL) {
      fprintf(stderr, "MAUVAIT NOM FICHIER : %s", argv[3]);
      exit(EXIT_FAILURE);
      fclose(file);
    }
    fprintf(file, "%d \n", nb_sol);
    fclose(file);
  } else {
    if (nb_sol == 0)
      printf("The game: <%s> has no solutions.\n", argv[2]);
    else if (nb_sol == 1)
      printf("The game: <%s> has only %d solution.\n", argv[2], nb_sol);
    else
      printf("The game: <%s> has %d solutions.\n", argv[2], nb_sol);
    game_delete(game_to_solve);
  }
  return EXIT_SUCCESS;
}