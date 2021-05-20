// SDL2 Demo by aurelien.esnard@u-bordeaux.fr

#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include "model.h"
#include "time.h"

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_prof.h"
#include "game_tools.h"

/* **************************************************************** */

void usage(int argc, char* argv[]) {
  fprintf(stderr, "Usage: %s [input] \n", argv[0]);
  exit(EXIT_FAILURE);
}

/* **************************************************************** */

int main(int argc, char* argv[]) {
  /* initialize SDL2 and some extensions */
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
    ERROR("Error: SDL_Init VIDEO (%s)", SDL_GetError());
  if (IMG_Init(IMG_INIT_PNG & IMG_INIT_PNG) != IMG_INIT_PNG)
    ERROR("Error: IMG_Init PNG (%s)", SDL_GetError());
  if (TTF_Init() != 0) ERROR("Error: TTF_Init (%s)", SDL_GetError());

  /* load the game to play */
  game g;
  if (argc >= 3) usage(argc, argv);
  if (argc == 2)
    g = game_load(argv[1]);
  else {
    int* t = game_args();
    g = game_random(12, 12, false, false, t[0], t[1]);
    game_restart(g);
    free(t);
  }

  /* create window and renderer */
  SDL_Window* win =
      SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       MIN_WIDTH(game_nb_cols(g)), MIN_HEIGHT(game_nb_rows(g)),
                       SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (!win) ERROR("Error: SDL_CreateWindow (%s)", SDL_GetError());
  SDL_Renderer* ren = SDL_CreateRenderer(
      win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!ren) ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
  if (!ren) ERROR("Error: SDL_CreateRenderer (%s)", SDL_GetError());
  SDL_SetWindowMinimumSize(win, MIN_WIDTH(game_nb_cols(g)),
                           MIN_HEIGHT(game_nb_rows(g)));

  SDL_RenderSetLogicalSize(ren, MIN_WIDTH(game_nb_cols(g)),
                           MIN_HEIGHT(game_nb_rows(g)));
  /* initialize your environment */
  Env* env = init(win, ren, argc, argv);

  /* main render loop */
  SDL_Event e;
  bool quit = false;
  while (!quit) {
    /* manage events */
    while (SDL_PollEvent(&e)) {
      /* process your events */
      quit = process(win, ren, env, &e);
      if (quit) break;
    }

    /* background in gray */
    SDL_SetRenderDrawColor(ren, 200, 173, 127, 255);
    SDL_RenderClear(ren);

    /* render all what you want */
    render(win, ren, env);
    SDL_RenderPresent(ren);
    if (quit)
      SDL_Delay(2000);
    else
      SDL_Delay(DELAY);
  }

  /* clean your environment */
  clean(win, ren, env);

  game_delete(g);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();

  return EXIT_SUCCESS;
}