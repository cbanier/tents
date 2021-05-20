#include "game.h"
#include "game_aux.c"
#include "game_ext.h"
#include "game_prof.h"
#include "game_tools.h"

#include <unistd.h>

void displayHelpMessage(void) {
  printf("action: help\n");
  printf("- press 't <i> <j>' to put a tent at square (i, j)\n");
  printf("- press 'g <i> <j>' to put grass at square (i, j)\n");
  printf("- press 'e <i> <j>' to erase square (i, j)\n");
  printf("- press 's' to save the actual game\n");
  printf("- press 'z' to undo the last move\n");
  printf("- press 'y' to redo the undo's moves\n");
  printf("- press 'r' to restart\n");
  printf("- press 'q' to quit\n");
}

bool square_move(char cmd) {
  if (cmd == 't') {
    return true;
  } else if (cmd == 'e') {
    return true;
  } else if (cmd == 'g') {
    return true;
  }
  return false;
}

square setSquareMove(char cmd) {
  if (cmd == 't') {
    return TENT;
  } else if (cmd == 'g') {
    return GRASS;
  }
  return EMPTY;
}

int game_loop(game g) {
  bool gameIsWon = false;
  square squareToAdd;
  int check_move_status;
  game_print(g);
  while (gameIsWon == false) {
    char cmd;
    int i, j;
    scanf(" %c", &cmd);
    if (cmd == 'h' || cmd == 'r' || cmd == 'q' || cmd == 'y' || cmd == 'z' ||
        cmd == 's' || cmd == 'd') {
      if (cmd == 'h') {
        how_to_play();
      } else if (cmd == 'r') {
        printf("action: restart\n");
        game_restart(g);
        game_print(g);
      } else if (cmd == 'q') {
        printf("action: quit\nWhat a shame, you gave up!");
        exit(0);
      } else if (cmd == 'z') {
        game_undo(g);
        printf("action: undo\n");
        game_print(g);
      } else if (cmd == 'y') {
        game_redo(g);
        printf("action: redo\n");
        game_print(g);
      } else if (cmd == 'd') {
        game_save(g, "../data/current.tnt");
        printf("action: save the current game in 'data/current.tnt'\n");
        game_print(g);
      } else if (cmd == 's') {
        game_solve(g);
        printf("action: solve the current game\n");
        game_print(g);
        sleep(3);
        exit(0);
      } else if (cmd != 't' && cmd != 'g' && cmd != 'e') {
        printf("[Command not found]\nPress 'h' to display the help message.\n");
        game_print(g);
      }
    }
    scanf(" %d %d", &i, &j);

    if (square_move(cmd)) {
      squareToAdd = setSquareMove(cmd);
      check_move_status = game_check_move(g, i, j, squareToAdd);
      if (check_move_status == REGULAR) {
        game_play_move(g, i, j, squareToAdd);
        printf("action: play move '%c' into square (%d, %d)\n", cmd, i, j);
        game_print(g);
        if (game_is_over(g)) {
          printf("Congratulations.\n");
          gameIsWon = true;
        }
      } else if (check_move_status == LOSING) {
        game_play_move(g, i, j, squareToAdd);
        printf("action: play move '%c' into square (%d, %d)\n", cmd, i, j);
        printf("Warning: losing move on square (%d, %d)\n", i, j);
        game_print(g);
      } else {
        printf("action: play move '%c' into square (%d, %d)\n", cmd, i, j);
        printf("Warning: illegal move on square (%d, %d)\n", i, j);
        game_print(g);
      }
    }
    cmd = '\0';
  }

  if (gameIsWon == true) {
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}

void usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s <../data/game_to_load.tnt> \n", argv[0]);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  int status = 1;
  if (argc >= 3) {
    usage(argc, argv);
  } else if (argc == 1) {
    int *t = game_args();
    game g =
        game_random(randInt(10, 14), randInt(10, 14), false, false, t[0], t[1]);
    game_restart(g);
    status = game_loop(g);
  } else {
    status = game_loop(game_load(argv[1]));
  }
  return status;
}