#include "model.h"

#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "assert.h"
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_prof.h"
#include "game_tools.h"
#include "time.h"

#define FONT "Roboto-Regular.ttf"
#define FONTSIZE 26
#define TREE_ "tree.png"
#define TENT_ "tent.png"
#define GRASS_ "grass.png"
#define ZERO "0.png"
#define ONE "1.png"
#define TWO "2.png"
#define THREE "3.png"
#define FOUR "4.png"
#define FIVE "5.png"
#define SIX "6.png"
#define SEVEN "7.png"
#define EIGHT "8.png"
#define NINE "9.png"
#define HELP "help.png"
#define NEXT "next.png"

/* **************************************************************** */

struct Env_t {
  /* PUT YOUR VARIABLES HERE */
  SDL_Texture *tree;
  SDL_Texture *tent;
  SDL_Texture *grass;
  SDL_Texture *zero;
  SDL_Texture *one;
  SDL_Texture *two;
  SDL_Texture *three;
  SDL_Texture *four;
  SDL_Texture *five;
  SDL_Texture *six;
  SDL_Texture *seven;
  SDL_Texture *eight;
  SDL_Texture *nine;
  SDL_Texture *help;
  SDL_Texture *next;
  SDL_Texture *win_text;
  SDL_Texture *shame_text;
  SDL_Texture *warning_text;
  SDL_Texture *transparent_text;
  SDL_Texture *luck_text;
  SDL_Texture *reset_text;
  SDL_Texture *undo_text;
  SDL_Texture *redo_text;
  SDL_Texture *solve_text;
  game g;
  move m;
  int grid_width;
  int grid_height;
};

/* **************************************************************** */

Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));

  PRINT("Press q to quit. Enjoy this game!\n");

  /* parse the args */
  if (argc == 2)
    env->g = game_load(argv[1]);
  else {
    int *t = game_args();
    env->g = game_random(12, 12, false, false, t[0], t[1]);
    game_restart(env->g);
    free(t);
  }

  env->grid_width = game_nb_cols(env->g) * GRID_CELL_SIZE;
  env->grid_height = game_nb_rows(env->g) * GRID_CELL_SIZE;

  /* init text texture using Arial font */
  SDL_Color color = {34, 120, 15, 255};
  SDL_Color color2 = {110, 50, 50, 255};
  SDL_Color color3 = {150, 150, 0, 255};
  TTF_Font *font = TTF_OpenFont(FONT, FONTSIZE);
  TTF_Font *font2 = TTF_OpenFont(FONT, 14);
  if (!font) ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(font, TTF_STYLE_BOLD);
  TTF_SetFontStyle(font2, TTF_STYLE_BOLD);
  // TTF_STYLE_ITALIC | TTF_STYLE_NORMAL
  SDL_Surface *win_surf =
      TTF_RenderText_Blended(font, "Congrats man!",
                             color3);  // blended rendering for ultra nice text
  env->win_text = SDL_CreateTextureFromSurface(ren, win_surf);
  SDL_Surface *shame_surf =
      TTF_RenderText_Blended(font, "Shame on you!",
                             color);  // blended rendering for ultra nice text
  env->shame_text = SDL_CreateTextureFromSurface(ren, shame_surf);
  SDL_Surface *warning_surf =
      TTF_RenderText_Blended(font, "Losing move!",
                             color2);  // blended rendering for ultra nice text
  env->warning_text = SDL_CreateTextureFromSurface(ren, warning_surf);
  SDL_Surface *transparent_surf =
      TTF_RenderText_Blended(font, "",
                             color);  // blended rendering for ultra nice text
  env->transparent_text = SDL_CreateTextureFromSurface(ren, transparent_surf);
  SDL_Surface *luck_surf =
      TTF_RenderText_Blended(font, "Good luck!",
                             color);  // blended rendering for ultra nice text
  env->luck_text = SDL_CreateTextureFromSurface(ren, luck_surf);
  SDL_Surface *reset_surf =
      TTF_RenderText_Blended(font2, "Reset",
                             color);  // blended rendering for ultra nice text
  env->reset_text = SDL_CreateTextureFromSurface(ren, reset_surf);
  SDL_Surface *undo_surf =
      TTF_RenderText_Blended(font2, "Undo",
                             color);  // blended rendering for ultra nice text
  env->undo_text = SDL_CreateTextureFromSurface(ren, undo_surf);
  SDL_Surface *redo_surf =
      TTF_RenderText_Blended(font2, "Redo",
                             color);  // blended rendering for ultra nice text
  env->redo_text = SDL_CreateTextureFromSurface(ren, redo_surf);
  SDL_Surface *solve_surf =
      TTF_RenderText_Blended(font2, "Solve",
                             color);  // blended rendering for ultra nice text
  env->solve_text = SDL_CreateTextureFromSurface(ren, solve_surf);

  SDL_FreeSurface(win_surf);
  SDL_FreeSurface(shame_surf);
  SDL_FreeSurface(warning_surf);
  SDL_FreeSurface(transparent_surf);
  SDL_FreeSurface(luck_surf);
  SDL_FreeSurface(reset_surf);
  SDL_FreeSurface(undo_surf);
  SDL_FreeSurface(redo_surf);
  SDL_FreeSurface(solve_surf);
  TTF_CloseFont(font);
  TTF_CloseFont(font2);

  /* init tent texture from PNG image */
  env->tent = IMG_LoadTexture(ren, TENT_);
  if (!env->tent) ERROR("IMG_LoadTexture: %s\n", TENT_);

  /* init tree texture from PNG image */
  env->tree = IMG_LoadTexture(ren, TREE_);
  if (!env->tree) ERROR("IMG_LoadTexture: %s\n", TREE_);

  /* init grass texture from PNG image */
  env->grass = IMG_LoadTexture(ren, GRASS_);
  if (!env->grass) ERROR("IMG_LoadTexture: %s\n", GRASS_);

  env->zero = IMG_LoadTexture(ren, ZERO);
  if (!env->zero) ERROR("IMG_LoadTexture: %s\n", ZERO);

  env->one = IMG_LoadTexture(ren, ONE);
  if (!env->one) ERROR("IMG_LoadTexture: %s\n", ONE);

  env->two = IMG_LoadTexture(ren, TWO);
  if (!env->two) ERROR("IMG_LoadTexture: %s\n", TWO);

  env->three = IMG_LoadTexture(ren, THREE);
  if (!env->three) ERROR("IMG_LoadTexture: %s\n", THREE);

  env->four = IMG_LoadTexture(ren, FOUR);
  if (!env->four) ERROR("IMG_LoadTexture: %s\n", FOUR);

  env->five = IMG_LoadTexture(ren, FIVE);
  if (!env->five) ERROR("IMG_LoadTexture: %s\n", FIVE);

  env->six = IMG_LoadTexture(ren, SIX);
  if (!env->six) ERROR("IMG_LoadTexture: %s\n", SIX);

  env->seven = IMG_LoadTexture(ren, SEVEN);
  if (!env->seven) ERROR("IMG_LoadTexture: %s\n", SEVEN);

  env->eight = IMG_LoadTexture(ren, EIGHT);
  if (!env->eight) ERROR("IMG_LoadTexture: %s\n", EIGHT);

  env->nine = IMG_LoadTexture(ren, NINE);
  if (!env->nine) ERROR("IMG_LoadTexture: %s\n", NINE);

  env->help = IMG_LoadTexture(ren, HELP);
  if (!env->help) ERROR("IMG_LoadTexture: %s\n", HELP);

  env->next = IMG_LoadTexture(ren, NEXT);
  if (!env->next) ERROR("IMG_LoadTexture: %s\n", NEXT);

  return env;
}

/* **************************************************************** */
/* *                          RENDER                              * */
/* **************************************************************** */

void gridRender(SDL_Renderer *ren, Env *env, SDL_Rect rect, int w, int h) {
  int width_left_space = WIDTH_SPACE(MIN_WIDTH(game_nb_cols(env->g)),
                                     env->grid_width + GRID_CELL_SIZE);
  int height_top_space = HEIGHT_SPACE(MIN_HEIGHT(game_nb_rows(env->g)),
                                      env->grid_height + GRID_CELL_SIZE);
  // Draw grid lines.
  SDL_SetRenderDrawColor(ren, 44, 44, 44, 255);
  for (int i = 0; i <= env->grid_height + GRID_CELL_SIZE; i += GRID_CELL_SIZE) {
    for (int j = 0; j <= env->grid_width + GRID_CELL_SIZE;
         j += GRID_CELL_SIZE) {
      // draw cols
      SDL_RenderDrawLine(ren, j + width_left_space, height_top_space,
                         j + width_left_space,
                         env->grid_height + GRID_CELL_SIZE + height_top_space);
      // draw lines
      SDL_RenderDrawLine(ren, j + width_left_space, i + height_top_space,
                         env->grid_width + width_left_space,
                         i + height_top_space);
    }
  }
  // Color the bottom right cell in random color;
  srand((unsigned int)time(NULL));  // Initialize the random number generator
  rect.x = width_left_space + env->grid_width + 1;
  rect.y = height_top_space + env->grid_height + 1;
  rect.w = GRID_CELL_SIZE - 1;
  rect.h = GRID_CELL_SIZE - 1;
  SDL_SetRenderDrawColor(ren, randInt(0, 255), randInt(0, 255), randInt(0, 255),
                         255);
  SDL_RenderFillRect(ren, &rect);
}

/* **************************************************************** */

void setNbTentTexture(SDL_Renderer *ren, Env *env, SDL_Rect rect,
                      int nb_expected, int i, int j) {
  // set the correct nb to display on the win
  switch (nb_expected) {
    case 0:
      SDL_QueryTexture(env->zero, NULL, NULL, &rect.w, &rect.h);
      rect.x = i;
      rect.y = j;
      SDL_RenderCopy(ren, env->zero, NULL, &rect);
      break;

    case 1:
      SDL_QueryTexture(env->one, NULL, NULL, &rect.w, &rect.h);
      rect.x = i;
      rect.y = j;
      SDL_RenderCopy(ren, env->one, NULL, &rect);
      break;

    case 2:
      SDL_QueryTexture(env->two, NULL, NULL, &rect.w, &rect.h);
      rect.x = i;
      rect.y = j;
      SDL_RenderCopy(ren, env->two, NULL, &rect);
      break;

    case 3:
      SDL_QueryTexture(env->three, NULL, NULL, &rect.w, &rect.h);
      rect.x = i;
      rect.y = j;
      SDL_RenderCopy(ren, env->three, NULL, &rect);
      break;

    case 4:
      SDL_QueryTexture(env->four, NULL, NULL, &rect.w, &rect.h);
      rect.x = i;
      rect.y = j;
      SDL_RenderCopy(ren, env->four, NULL, &rect);
      break;

    case 5:
      SDL_QueryTexture(env->five, NULL, NULL, &rect.w, &rect.h);
      rect.x = i;
      rect.y = j;
      SDL_RenderCopy(ren, env->five, NULL, &rect);
      break;

    case 6:
      SDL_QueryTexture(env->six, NULL, NULL, &rect.w, &rect.h);
      rect.x = i;
      rect.y = j;
      SDL_RenderCopy(ren, env->six, NULL, &rect);
      break;

    case 7:
      SDL_QueryTexture(env->seven, NULL, NULL, &rect.w, &rect.h);
      rect.x = i;
      rect.y = j;
      SDL_RenderCopy(ren, env->seven, NULL, &rect);
      break;

    case 8:
      SDL_QueryTexture(env->eight, NULL, NULL, &rect.w, &rect.h);
      rect.x = i;
      rect.y = j;
      SDL_RenderCopy(ren, env->eight, NULL, &rect);
      break;

    case 9:
      SDL_QueryTexture(env->nine, NULL, NULL, &rect.w, &rect.h);
      rect.x = i;
      rect.y = j;
      SDL_RenderCopy(ren, env->nine, NULL, &rect);
      break;
  }
}

/* **************************************************************** */

void nbTentRender(SDL_Renderer *ren, Env *env, SDL_Rect rect, int w, int h) {
  // make the render for the game_nb_tents_expected of rows and columns
  int nb_expected;
  int x, y;
  for (int i = 0; i < game_nb_rows(env->g); i++) {
    nb_expected = game_get_expected_nb_tents_row(env->g, i);
    y = i * GRID_CELL_SIZE + HEIGHT_SPACE(MIN_HEIGHT(game_nb_rows(env->g)),
                                          env->grid_height + GRID_CELL_SIZE);
    x = WIDTH_SPACE(MIN_WIDTH(game_nb_cols(env->g)),
                    env->grid_width + GRID_CELL_SIZE) +
        env->grid_width;
    if (nb_expected <= game_get_current_nb_tents_row(env->g, i)) {
      rect.x = x + 1;
      rect.y = y + 1;
      rect.w = GRID_CELL_SIZE - 1;
      rect.h = GRID_CELL_SIZE - 1;
      if (nb_expected == game_get_current_nb_tents_row(env->g, i))
        SDL_SetRenderDrawColor(ren, 0, 146, 0, 255);
      else
        SDL_SetRenderDrawColor(ren, 178, 34, 34, 255);
      SDL_RenderFillRect(ren, &rect);
    }
    setNbTentTexture(ren, env, rect, nb_expected, x, y);
  }
  for (int j = 0; j < game_nb_cols(env->g); j++) {
    nb_expected = game_get_expected_nb_tents_col(env->g, j);
    y = env->grid_height + HEIGHT_SPACE(MIN_HEIGHT(game_nb_rows(env->g)),
                                        env->grid_height + GRID_CELL_SIZE);
    ;
    x = j * GRID_CELL_SIZE + WIDTH_SPACE(MIN_WIDTH(game_nb_cols(env->g)),
                                         env->grid_width + GRID_CELL_SIZE);
    if (nb_expected <= game_get_current_nb_tents_col(env->g, j)) {
      rect.x = x + 1;
      rect.y = y + 1;
      rect.w = GRID_CELL_SIZE - 1;
      rect.h = GRID_CELL_SIZE - 1;
      if (nb_expected == game_get_current_nb_tents_col(env->g, j))
        SDL_SetRenderDrawColor(ren, 0, 146, 0, 255);
      else
        SDL_SetRenderDrawColor(ren, 178, 34, 34, 255);
      SDL_RenderFillRect(ren, &rect);
    }
    setNbTentTexture(ren, env, rect, nb_expected, x, y);
  }
}

/* **************************************************************** */

void gameEngineRender(SDL_Renderer *ren, Env *env, SDL_Rect rect, int w,
                      int h) {
  // make the render from the interactive game
  square s;
  int x, y;
  for (int i = 0; i < game_nb_rows(env->g); i++) {
    for (int j = 0; j < game_nb_cols(env->g); j++) {
      s = game_get_square(env->g, i, j);
      y = i * GRID_CELL_SIZE + HEIGHT_SPACE(MIN_HEIGHT(game_nb_rows(env->g)),
                                            env->grid_height + GRID_CELL_SIZE);
      x = j * GRID_CELL_SIZE + WIDTH_SPACE(MIN_WIDTH(game_nb_cols(env->g)),
                                           env->grid_width + GRID_CELL_SIZE);
      switch (s) {
        case EMPTY:
          break;

        case TREE:
          SDL_QueryTexture(env->tree, NULL, NULL, &rect.w, &rect.h);
          rect.x = x;
          rect.y = y;
          SDL_RenderCopy(ren, env->tree, NULL, &rect);
          break;

        case TENT:
          SDL_QueryTexture(env->tent, NULL, NULL, &rect.w, &rect.h);
          rect.x = x;
          rect.y = y;
          SDL_RenderCopy(ren, env->tent, NULL, &rect);
          break;

        case GRASS:
          SDL_QueryTexture(env->grass, NULL, NULL, &rect.w, &rect.h);
          rect.x = x;
          rect.y = y;
          SDL_RenderCopy(ren, env->grass, NULL, &rect);
          break;
      }
    }
  }
}

/* **************************************************************** */

void drawTopRectangle(SDL_Renderer *ren, Env *env, SDL_Rect rect, int w) {
  int width_left_space = WIDTH_SPACE(MIN_WIDTH(game_nb_cols(env->g)),
                                     env->grid_width + GRID_CELL_SIZE);
  /* Message rectangle */
  rect.x = width_left_space + 1;
  rect.y = GRID_CELL_SIZE / 3;
  rect.w = env->grid_width - 1;
  rect.h = GRID_CELL_SIZE - 1;
  SDL_SetRenderDrawColor(ren, 225, 206, 154, 255);
  SDL_RenderFillRect(ren, &rect);

  /* render text texture */
  SDL_QueryTexture(env->luck_text, NULL, NULL, &rect.w, &rect.h);
  rect.x = width_left_space + 1 + GRID_CELL_SIZE / 3;
  rect.y = GRID_CELL_SIZE / 3 - 1;
  SDL_RenderCopy(ren, env->luck_text, NULL, &rect);

  /* Menu rectangle */
  rect.x = width_left_space + env->grid_width + 1;
  rect.y = GRID_CELL_SIZE / 3;
  rect.w = GRID_CELL_SIZE - 1;
  rect.h = GRID_CELL_SIZE - 1;
  SDL_RenderFillRect(ren, &rect);
  /* render menu texture */
  SDL_QueryTexture(env->next, NULL, NULL, &rect.w, &rect.h);
  /* +2 car l'image fait 26x26 pixel et la centre dans un rectangle de 30x30.*/
  rect.x = width_left_space + env->grid_width + 2;
  rect.y = GRID_CELL_SIZE / 3 + 2;
  SDL_RenderCopy(ren, env->next, NULL, &rect);

  // Draw the outline
  SDL_SetRenderDrawColor(ren, 44, 44, 44, 255);
  SDL_RenderDrawLine(ren, width_left_space, GRID_CELL_SIZE / 3 - 1,
                     width_left_space + env->grid_width + GRID_CELL_SIZE - 1,
                     GRID_CELL_SIZE / 3 - 1);

  SDL_RenderDrawLine(ren, width_left_space, 4 * GRID_CELL_SIZE / 3 - 1,
                     width_left_space + env->grid_width + GRID_CELL_SIZE - 1,
                     4 * GRID_CELL_SIZE / 3 - 1);

  SDL_RenderDrawLine(ren, width_left_space, GRID_CELL_SIZE / 3 - 1,
                     width_left_space, 4 * GRID_CELL_SIZE / 3 - 1);

  SDL_RenderDrawLine(ren, width_left_space + env->grid_width,
                     GRID_CELL_SIZE / 3 - 1, width_left_space + env->grid_width,
                     4 * GRID_CELL_SIZE / 3 - 1);

  SDL_RenderDrawLine(ren, width_left_space + env->grid_width + GRID_CELL_SIZE,
                     GRID_CELL_SIZE / 3 - 1,
                     width_left_space + env->grid_width + GRID_CELL_SIZE,
                     4 * GRID_CELL_SIZE / 3 - 1);
}

/* **************************************************************** */

void drawButtomRectangle(SDL_Renderer *ren, Env *env, SDL_Rect rect) {
  int width_left_space = WIDTH_SPACE(MIN_WIDTH(game_nb_cols(env->g)),
                                     env->grid_width + GRID_CELL_SIZE);
  int height_top_space = HEIGHT_SPACE(MIN_HEIGHT(game_nb_rows(env->g)),
                                      env->grid_height + GRID_CELL_SIZE);
  // draw the buttom rectangle
  rect.x = width_left_space + 1;
  rect.y = height_top_space + env->grid_height + GRID_CELL_SIZE + 1;
  rect.w = env->grid_width + GRID_CELL_SIZE - 1;
  rect.h = GRID_CELL_SIZE - 1;
  SDL_SetRenderDrawColor(ren, 225, 206, 154, 255);
  SDL_RenderFillRect(ren, &rect);
  // draw the outlines
  SDL_SetRenderDrawColor(ren, 44, 44, 44, 255);
  SDL_RenderDrawLine(ren, width_left_space,
                     height_top_space + env->grid_height + 2 * GRID_CELL_SIZE,
                     width_left_space + env->grid_width + GRID_CELL_SIZE,
                     height_top_space + env->grid_height + 2 * GRID_CELL_SIZE);

  SDL_RenderDrawLine(ren, width_left_space,
                     height_top_space + env->grid_height + GRID_CELL_SIZE,
                     width_left_space,
                     height_top_space + env->grid_height + 2 * GRID_CELL_SIZE);

  SDL_RenderDrawLine(ren, width_left_space + env->grid_width,
                     height_top_space + env->grid_height + GRID_CELL_SIZE,
                     width_left_space + env->grid_width,
                     height_top_space + env->grid_height + 2 * GRID_CELL_SIZE);

  SDL_RenderDrawLine(ren, width_left_space + env->grid_width + GRID_CELL_SIZE,
                     height_top_space + env->grid_height + GRID_CELL_SIZE,
                     width_left_space + env->grid_width + GRID_CELL_SIZE,
                     height_top_space + env->grid_height + 2 * GRID_CELL_SIZE);

  float rec_action_width = env->grid_width / 4;
  for (uint i = 0; i < 4; i++) {
    switch (i) {
      case 0:
        SDL_RenderDrawLine(
            ren, width_left_space + rec_action_width,
            height_top_space + env->grid_height + GRID_CELL_SIZE,
            width_left_space + rec_action_width,
            height_top_space + env->grid_height + 2 * GRID_CELL_SIZE);
        rect.x = width_left_space + rec_action_width / 6 + 1;
        rect.w = rec_action_width - rec_action_width / 3;
        rect.y = height_top_space + env->grid_height + GRID_CELL_SIZE + 1;
        rect.h = GRID_CELL_SIZE - 2;
        SDL_RenderCopyEx(ren, env->reset_text, NULL, &rect, 0, NULL,
                         SDL_FLIP_NONE);
        break;
      case 1:
        SDL_RenderDrawLine(
            ren, width_left_space + 2 * rec_action_width,
            height_top_space + env->grid_height + GRID_CELL_SIZE,
            width_left_space + 2 * rec_action_width,
            height_top_space + env->grid_height + 2 * GRID_CELL_SIZE);
        rect.x = width_left_space + rec_action_width + rec_action_width / 6 + 1;
        rect.w = rec_action_width - rec_action_width / 3;
        rect.y = height_top_space + env->grid_height + GRID_CELL_SIZE + 1;
        rect.h = GRID_CELL_SIZE - 2;
        SDL_RenderCopyEx(ren, env->undo_text, NULL, &rect, 0, NULL,
                         SDL_FLIP_NONE);
        break;
      case 2:
        SDL_RenderDrawLine(
            ren, width_left_space + 3 * rec_action_width,
            height_top_space + env->grid_height + GRID_CELL_SIZE,
            width_left_space + 3 * rec_action_width,
            height_top_space + env->grid_height + 2 * GRID_CELL_SIZE);
        rect.x =
            width_left_space + 2 * rec_action_width + rec_action_width / 6 + 1;
        rect.w = rec_action_width - rec_action_width / 3;
        rect.y = height_top_space + env->grid_height + GRID_CELL_SIZE + 1;
        rect.h = GRID_CELL_SIZE - 2;
        SDL_RenderCopyEx(ren, env->redo_text, NULL, &rect, 0, NULL,
                         SDL_FLIP_NONE);
        break;
      case 3:
        rect.x =
            width_left_space + 3 * rec_action_width + rec_action_width / 6 + 1;
        rect.w = rec_action_width - rec_action_width / 3;
        rect.y = height_top_space + env->grid_height + GRID_CELL_SIZE + 1;
        rect.h = GRID_CELL_SIZE - 2;
        SDL_RenderCopyEx(ren, env->solve_text, NULL, &rect, 0, NULL,
                         SDL_FLIP_NONE);
        break;
    }
  }
  /* render help texture */
  SDL_QueryTexture(env->help, NULL, NULL, &rect.w, &rect.h);
  /* +2 car l'image fait 26x26 pixel et la centre dans un rectangle de 30x30.*/
  rect.x = width_left_space + env->grid_width + 2;
  rect.y = height_top_space + env->grid_height + GRID_CELL_SIZE + 2;
  SDL_RenderCopy(ren, env->help, NULL, &rect);
}
/* **************************************************************** */

// This functions allowed to color losing move cell
void drawRedCell(SDL_Renderer *ren, Env *env, SDL_Rect rect, int i, int j,
                 int w) {
  SDL_SetRenderDrawColor(ren, 178, 34, 34, 255);
  rect.x = j * GRID_CELL_SIZE +
           WIDTH_SPACE(MIN_WIDTH(game_nb_cols(env->g)),
                       env->grid_width + GRID_CELL_SIZE) +
           1;
  rect.y = i * GRID_CELL_SIZE +
           HEIGHT_SPACE(MIN_HEIGHT(game_nb_rows(env->g)),
                        env->grid_height + GRID_CELL_SIZE) +
           1;
  rect.w = GRID_CELL_SIZE - 1;
  rect.h = GRID_CELL_SIZE - 1;
  SDL_RenderFillRect(ren, &rect);
  SDL_RenderPresent(ren);
  SDL_Delay(2 * DELAY);
}

/* **************************************************************** */

void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  /* get current window size */
  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  drawTopRectangle(ren, env, rect, w);
  gridRender(ren, env, rect, w, h);
  nbTentRender(ren, env, rect, w, h);
  gameEngineRender(ren, env, rect, w, h);
  drawButtomRectangle(ren, env, rect);
}

/* **************************************************************** */
/* *                         PROCESS                              * */
/* **************************************************************** */

bool gameEngineProcess(SDL_Point type, SDL_Window *win, SDL_Renderer *ren,
                       Env *env, SDL_Event *e, SDL_Rect rect) {
  int width_left_space = WIDTH_SPACE(MIN_WIDTH(game_nb_cols(env->g)),
                                     env->grid_width + GRID_CELL_SIZE);
  int height_top_space = HEIGHT_SPACE(MIN_HEIGHT(game_nb_rows(env->g)),
                                      env->grid_height + GRID_CELL_SIZE);
  int i, j, w, h;
  SDL_GetWindowSize(win, &w, &h);
  square prev_s;
  float rec_action_width = env->grid_width / 4;
  /* click on menu */
  if ((type.x <= width_left_space + env->grid_width + GRID_CELL_SIZE &&
       type.y <= 4 * GRID_CELL_SIZE / 3) &&
      (type.x >= width_left_space + env->grid_width &&
       type.y >= GRID_CELL_SIZE / 3)) {
    game_delete(env->g);
    int *t = game_args();
    env->g = game_random(12, 12, false, false, t[0], t[1]);
    game_restart(env->g);
    free(t);
  }
  /* click on restart*/
  else if ((type.x < width_left_space + rec_action_width &&
            type.y <=
                height_top_space + env->grid_height + 2 * GRID_CELL_SIZE) &&
           (type.x >= width_left_space &&
            type.y >= height_top_space + env->grid_height + GRID_CELL_SIZE)) {
    game_restart(env->g);
    env->luck_text = env->transparent_text;
  }
  /* click on undo*/
  else if ((type.x < width_left_space + 2 * rec_action_width &&
            type.y <=
                height_top_space + env->grid_height + 2 * GRID_CELL_SIZE) &&
           (type.x >= width_left_space + rec_action_width &&
            type.y >= height_top_space + env->grid_height + GRID_CELL_SIZE)) {
    game_undo(env->g);
    if (!_stack_is_empty(env->g->undo_stack)) {
      move m = _stack_pop_move(env->g->undo_stack);
      _stack_push_move(env->g->undo_stack, m);
      if (game_check_move(env->g, m.i, m.j, m.new) == REGULAR)
        env->luck_text = env->transparent_text;
      else if (game_check_move(env->g, m.i, m.j, m.new) == LOSING)
        env->luck_text = env->warning_text;
    } else
      env->luck_text = env->transparent_text;
  }
  /* click on redo*/
  else if ((type.x < width_left_space + 3 * rec_action_width &&
            type.y <=
                height_top_space + env->grid_height + 2 * GRID_CELL_SIZE) &&
           (type.x >= width_left_space + 2 * rec_action_width &&
            type.y >= height_top_space + env->grid_height + GRID_CELL_SIZE)) {
    if (!_stack_is_empty(env->g->redo_stack)) {
      move m = _stack_pop_move(env->g->redo_stack);
      _stack_push_move(env->g->redo_stack, m);
      if (game_check_move(env->g, m.i, m.j, m.new) == REGULAR)
        env->luck_text = env->transparent_text;
      else if (game_check_move(env->g, m.i, m.j, m.new) == LOSING)
        env->luck_text = env->warning_text;
    } else {
      env->luck_text = env->transparent_text;
    }
    game_redo(env->g);
  }
  /* click on solve*/
  else if ((type.x < width_left_space + 4 * rec_action_width &&
            type.y <=
                height_top_space + env->grid_height + 2 * GRID_CELL_SIZE) &&
           (type.x >= width_left_space + 3 * rec_action_width &&
            type.y >= height_top_space + env->grid_height + GRID_CELL_SIZE)) {
    game_restart(env->g);
    game_solve(env->g);
    env->luck_text = env->shame_text;
    return true;
  }
  /* click on help*/
  else if ((type.x <=
                width_left_space + 4 * rec_action_width + GRID_CELL_SIZE &&
            type.y <=
                height_top_space + env->grid_height + 2 * GRID_CELL_SIZE) &&
           (type.x >= width_left_space + 4 * rec_action_width &&
            type.y >= height_top_space + env->grid_height + GRID_CELL_SIZE)) {
    SDL_ShowSimpleMessageBox(
        SDL_MESSAGEBOX_INFORMATION, "Help commands",
        "•press 's' to solve the game\n•press 'f' to save the actual "
        "game\n•press 'z' to undo the last move\n•press 'y' to redo the "
        "undo's moves\n•press 'r' to restart\n•press 'q' to quit\n",
        win);
  }
  /* click on the grid */
  if ((type.x <= width_left_space + env->grid_width &&
       type.y <= height_top_space + env->grid_height) &&
      (type.x >= width_left_space && type.y >= height_top_space)) {
    j = (type.x - width_left_space) / GRID_CELL_SIZE;
    i = (type.y - height_top_space) / GRID_CELL_SIZE;

    prev_s = game_get_square(env->g, i, j);
    if (prev_s == EMPTY) {
      if (game_check_move(env->g, i, j, TENT) == REGULAR) {
        game_play_move(env->g, i, j, TENT);
        env->luck_text = env->transparent_text;
        if (game_is_over(env->g)) {
          env->luck_text = env->win_text;
          return true;
        }
      } else if (game_check_move(env->g, i, j, TENT) == LOSING) {
        env->luck_text = env->warning_text;
        drawRedCell(ren, env, rect, i, j, w);
        game_play_move(env->g, i, j, TENT);
      }
    } else if (prev_s == GRASS) {
      game_play_move(env->g, i, j, EMPTY);
      env->luck_text = env->transparent_text;

    } else if (prev_s == TENT) {
      if (game_check_move(env->g, i, j, GRASS) == REGULAR) {
        game_play_move(env->g, i, j, GRASS);
        env->luck_text = env->transparent_text;
        if (game_is_over(env->g)) {
          env->luck_text = env->win_text;
          return true;
        }
      } else if (game_check_move(env->g, i, j, GRASS) == LOSING) {
        env->luck_text = env->warning_text;
        drawRedCell(ren, env, rect, i, j, w);
        game_play_move(env->g, i, j, GRASS);
      }
    }
  }
  return false;
}

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  /* get current window size */
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  if (e->type == SDL_QUIT) {
    return true;
  }
  bool processStatut = false;
  /* Android events */
  /*
  #ifdef __ANDROID__
    else if (e->type == SDL_FINGERDOWN) {
      SDL_Point finger;
      finger.x = e->tfinger.x;
      finger.y = e->tfinger.y;
      processStatut = gameEngineProcess(finger, win, ren, env, e, rect);
    }
  // other events
  #else*/
  if (e->type == SDL_KEYDOWN) {
    switch (e->key.keysym.sym) {
      case SDLK_q:
        env->luck_text = env->shame_text;
        return true;
      case SDLK_s:
        game_restart(env->g);
        game_solve(env->g);
        env->luck_text = env->shame_text;
        return true;
      case SDLK_r:
        game_restart(env->g);
        env->luck_text = env->transparent_text;
        break;
      case SDLK_l:
        game_save(env->g, "../data/current_game_sdl.tnt");
        return true;
      case SDLK_z:
        game_undo(env->g);
        if (!_stack_is_empty(env->g->undo_stack)) {
          move m = _stack_pop_move(env->g->undo_stack);
          _stack_push_move(env->g->undo_stack, m);
          if (game_check_move(env->g, m.i, m.j, m.new) == REGULAR) {
            env->luck_text = env->transparent_text;
          }
          if (game_check_move(env->g, m.i, m.j, m.new) == LOSING) {
            env->luck_text = env->warning_text;
          }
        } else {
          env->luck_text = env->transparent_text;
        }
        break;
      case SDLK_y:
        if (!_stack_is_empty(env->g->redo_stack)) {
          move m = _stack_pop_move(env->g->redo_stack);
          _stack_push_move(env->g->redo_stack, m);
          if (game_check_move(env->g, m.i, m.j, m.new) == REGULAR) {
            env->luck_text = env->transparent_text;
          }
          if (game_check_move(env->g, m.i, m.j, m.new) == LOSING) {
            env->luck_text = env->warning_text;
          }
        } else {
          env->luck_text = env->transparent_text;
        }
        game_redo(env->g);
        break;
      case SDLK_h:
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_INFORMATION, "Help commands",
            "•press 's' to solve the game\n•press 'f' to save the actual "
            "game\n•press 'z' to undo the last move\n•press 'y' to redo the "
            "undo's moves\n•press 'r' to restart\n•press 'q' to quit\n",
            win);
        break;
    }
  } else if (e->type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse;
    mouse.x = e->button.x;
    mouse.y = e->button.y;
    processStatut = gameEngineProcess(mouse, win, ren, env, e, rect);
  } else if (e->type == SDL_FINGERDOWN) {
    SDL_Point finger;
    finger.x = e->tfinger.x;
    finger.y = e->tfinger.y;
    processStatut = gameEngineProcess(finger, win, ren, env, e, rect);
  }
  return processStatut;
}

/* **************************************************************** */

void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  /* PUT YOUR CODE HERE TO CLEAN MEMORY */
  game_delete(env->g);
  SDL_DestroyTexture(env->tree);
  SDL_DestroyTexture(env->tent);
  SDL_DestroyTexture(env->grass);
  SDL_DestroyTexture(env->zero);
  SDL_DestroyTexture(env->one);
  SDL_DestroyTexture(env->two);
  SDL_DestroyTexture(env->three);
  SDL_DestroyTexture(env->four);
  SDL_DestroyTexture(env->five);
  SDL_DestroyTexture(env->six);
  SDL_DestroyTexture(env->seven);
  SDL_DestroyTexture(env->eight);
  SDL_DestroyTexture(env->nine);
  SDL_DestroyTexture(env->shame_text);
  SDL_DestroyTexture(env->win_text);
  SDL_DestroyTexture(env->warning_text);
  SDL_DestroyTexture(env->luck_text);
  SDL_DestroyTexture(env->transparent_text);
  SDL_DestroyTexture(env->reset_text);
  SDL_DestroyTexture(env->undo_text);
  SDL_DestroyTexture(env->redo_text);
  SDL_DestroyTexture(env->solve_text);
  SDL_DestroyTexture(env->help);
  SDL_DestroyTexture(env->next);
  free(env);
}

/* **************************************************************** */
