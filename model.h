// SDL2 Model by aurelien.esnard@u-bordeaux.fr

#ifndef MODEL_H
#define MODEL_H

#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct Env_t Env;

/* **************************************************************** */

#ifdef __ANDROID__
#define PRINT(STR, ...)          \
  do {                           \
    SDL_Log(STR, ##__VA_ARGS__); \
  } while (0)
#define ERROR(STR, ...)          \
  do {                           \
    SDL_Log(STR, ##__VA_ARGS__); \
    exit(EXIT_FAILURE);          \
  } while (0)
#else
#define PRINT(STR, ...)         \
  do {                          \
    printf(STR, ##__VA_ARGS__); \
  } while (0)
#define ERROR(STR, ...)                  \
  do {                                   \
    fprintf(stderr, STR, ##__VA_ARGS__); \
    exit(EXIT_FAILURE);                  \
  } while (0)
#endif

/* **************************************************************** */

#define APP_NAME "Tents by A62A"
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define DELAY 100

#define GRID_CELL_SIZE 30
#define MIN_WIDTH(game_cols) ((game_cols + 2) * GRID_CELL_SIZE)
#define MIN_HEIGHT(game_rows) ((game_rows + 4) * GRID_CELL_SIZE)
#define WIDTH_SPACE(min_w, grid_width) ((min_w - (grid_width)) / 2)
#define HEIGHT_SPACE(min_h, grid_height) ((min_h - (grid_height/3)) / 2)

/* **************************************************************** */

Env* init(SDL_Window* win, SDL_Renderer* ren, int argc, char* argv[]);
void render(SDL_Window* win, SDL_Renderer* ren, Env* env);
void clean(SDL_Window* win, SDL_Renderer* ren, Env* env);
bool process(SDL_Window* win, SDL_Renderer* ren, Env* env, SDL_Event* e);

/* **************************************************************** */
#endif