/**
 * @file game_aux.c
 * @copyright University of Bordeaux. All rights reserved, 2020.
 *
 **/

#include "game_aux.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_ext.h"
#include "game_prof.h"

/* ************************************************************************** */
// Regular text
#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"

// Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"

#define RESET "\x1B[0m"
/* ************************************************************************** */

void displayGroup() {
  printf(
      "\t     " BYEL
      " ========================================================== \n" RESET);
  printf("\t     " BGRN "|" BWHT
         "                     Tents by la Team                     " BGRN
         "|\n" RESET);
  printf("\t     " BYEL "|" BGRN
         "----------------------------------------------------------" BYEL
         "|\n" RESET);
  printf("\t     " BGRN "|" BBLU "       Alexandre Bousquet:" RESET "" RED
         " https://github.com/aleb33      " BGRN "|\n" RESET);
  printf("\t     " BYEL "|" BGRN
         "----------------------------------------------------------" BYEL
         "|\n" RESET);
  printf("\t     " BMAG "|" BLU "   Anass Bousseaden:" RESET "" RED
         " https://github.com/AnassBousseaden   " BMAG "|\n" RESET);
  printf("\t     " BYEL "|" BGRN
         "----------------------------------------------------------" BYEL
         "|\n" RESET);
  printf("\t     " BGRN "|" BLU "     Baptiste Bédouret:" RESET "" RED
         " https://github.com/Baptiste-is    " BGRN "|\n" RESET);
  printf("\t     " BYEL "|" BGRN
         "----------------------------------------------------------" BYEL
         "|\n" RESET);
  printf("\t     " BGRN "|" BLU "        Corentin Banier:" RESET "" RED
         " https://github.com/cbanier       " BGRN "|\n" RESET);
  printf(
      "\t     " BYEL
      " ========================================================== \n" RESET);
  printf("\n");
}

void how_to_play(void) {
  printf("\t     " BYEL
         "╔═════════════════════════════════════════════════════════════════"
         "╗\n" RESET);
  printf("\t     " BYEL "║" BWHT " \t\t\t    Voici comment jouer !" BYEL
         "                      ║\n" RESET);
  printf("\t     " BYEL "║" BMAG " ► " RESET "" CYN "Choisissez " BRED
         "t/g/e " RESET "" CYN
         "pour placer une TENT, une GRASS ou un EMPTY " BYEL
         " ║\n"
         "\t     " BYEL "║" RESET "" CYN
         "   en ajoutant une coordonée i et j pour jouer." BYEL
         "                  ║ \n");
  printf("\t     " BYEL "║" BMAG " ► " RESET "" CYN "Par exemple : " BGRN
         "t 0 2" RESET "" CYN " -> Place une tente aux coordonées 0 2  " BYEL
         "   ║\n");
  printf("\t     " BYEL "║" BMAG " ► " BRED "r " RESET "" CYN
         "pour relancer le jeu " BYEL
         "                                       ║\n");
  printf("\t     " BYEL "║" BMAG " ► " BRED "h " RESET "" CYN
         "pour afficher de l'aide" BYEL
         "                                     ║\n");
  printf("\t     " BYEL "║" BMAG " ► " BRED "s " RESET "" CYN
         "pour résoudre le jeu" BYEL
         "                                        ║\n");
  printf("\t     " BYEL "║" BMAG " ► " BRED "d " RESET "" CYN
         "pour sauvegarder le jeu" BYEL
         "                                     ║\n");
  printf("\t     " BYEL "║" BMAG " ► " BRED "q " RESET "" CYN
         "pour quitter le jeu" BYEL
         "                                         ║\n");
  printf("\t     " BYEL "║" BMAG " ► " BRED "z " RESET "" CYN
         "pour revenir en arrière " BYEL
         "                                    ║\n");
  printf("\t     " BYEL "║" BMAG " ► " BRED "y " RESET "" CYN
         "pour revenir en avant " BYEL
         "                                      ║\n");
  printf("\t     " BYEL
         "╚═════════════════════════════════════════════════════════════════╝"
         "\n\n" RESET);
}

/* ************************************************************************** */

void displaySpace(cgame g) {
  if (game_nb_cols(g) < 10)
    printf("\t\t   ");
  else
    printf("\t\t    ");
}

/* ************************************************************************** */

void displayTopBar(cgame g) {
  printf("\t\t     ");
  for (uint j = 1; j < game_nb_cols(g); j = j + 2) {
    if (j < 10)
      printf("%d ", j);
    else
      printf("%d", j);
  }
  printf("\n");
  printf("\t\t    ");
  for (uint j = 0; j < game_nb_cols(g); j = j + 2) {
    if (j < 10)
      printf("%d ", j);
    else
      printf("%d", j);
  }
  printf("\n");
}

/* ************************************************************************** */

void game_print(cgame g) {
  assert(g);
  displayGroup();
  if (game_nb_cols(g) >= 10)
    displayTopBar(g);
  else {
    printf("\t\t   ");
    for (uint j = 0; j < game_nb_cols(g); j++) printf("%d", j);
    printf("\n");
  }
  displaySpace(g);
  for (uint j = 0; j < game_nb_cols(g); j++) printf("-");
  printf("\n");
  for (uint i = 0; i < game_nb_rows(g); i++) {
    if (game_nb_rows(g) < 10)
      printf("\t\t|%d|", i);
    else if (i < 10)
      printf("\t\t|%d |", i);
    else
      printf("\t\t|%d|", i);
    for (uint j = 0; j < game_nb_cols(g); j++) {
      square s = game_get_square(g, i, j);
      char c = _square2str(s);
      printf("%c", c);
    }
    printf("|%d|\n", game_get_expected_nb_tents_row(g, i));
  }
  displaySpace(g);
  for (uint j = 0; j < game_nb_cols(g); j++) printf("-");
  printf("\n");
  displaySpace(g);
  for (uint j = 0; j < game_nb_cols(g); j++)
    printf("%d", game_get_expected_nb_tents_col(g, j));
  printf("\n");
}

/* ************************************************************************** */

game game_default(void) {
  square squares[] = {
      EMPTY, EMPTY, EMPTY, EMPTY, TREE,  TREE,  EMPTY, EMPTY, /* row 0 */
      TREE,  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE,  /* row 1 */
      EMPTY, EMPTY, EMPTY, EMPTY, TREE,  EMPTY, EMPTY, EMPTY, /* row 2 */
      TREE,  EMPTY, EMPTY, EMPTY, EMPTY, TREE,  EMPTY, EMPTY, /* row 3 */
      EMPTY, TREE,  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, /* row 4 */
      TREE,  EMPTY, EMPTY, EMPTY, TREE,  EMPTY, TREE,  EMPTY, /* row 5 */
      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, /* row 6 */
      TREE,  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, /* row 7 */
  };
  uint nb_tents_row[] = {3, 0, 4, 0, 4, 0, 1, 0};
  uint nb_tents_col[] = {4, 0, 1, 2, 1, 1, 2, 1};
  return game_new(squares, nb_tents_row, nb_tents_col);
}

/* ************************************************************************** */

game game_default_solution(void) {
  game g = game_default();
  game_play_move(g, 0, 0, TENT);
  game_play_move(g, 0, 3, TENT);
  game_play_move(g, 0, 6, TENT);
  game_play_move(g, 2, 0, TENT);
  game_play_move(g, 2, 3, TENT);
  game_play_move(g, 2, 5, TENT);
  game_play_move(g, 2, 7, TENT);
  game_play_move(g, 4, 0, TENT);
  game_play_move(g, 4, 2, TENT);
  game_play_move(g, 4, 4, TENT);
  game_play_move(g, 4, 6, TENT);
  game_play_move(g, 6, 0, TENT);
  for (uint i = 0; i < DEFAULT_SIZE; i++) game_fill_grass_row(g, i);
  return g;
}

/* ************************************************************************** */
