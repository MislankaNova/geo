#ifndef VIEW_H
#define VIEW_H

#include "SDL2/SDL.h"

#include "geo.h"

typedef enum GEO_VIEW_MODE {
  GEO_VIEW_MODE_ELEVATION,
  GEO_VIEW_MODE_TRIG,
  GEO_VIEW_MODE_HUMIDITY,
  GEO_VIEW_MODE_SLOPE,
  GEO_VIEW_MODE_LIFE,
  GEO_VIEW_MODE_CITY,
  GEO_VIEW_MODE_DISTANCE
} GEO_VIEW_MODE;

typedef struct View {
  GEO_VIEW_MODE mode;
  SDL_Renderer *renderer;
  SDL_Texture *map_texture;
  SDL_Texture *overlay_texture;
  SDL_Surface *draw_surface;
  int move_x;
  int move_y;
  int centre_x;
  int centre_y;
  int tile_size;
} View;

View *GEO_NewView(SDL_Renderer *renderer);
void GEO_DestroyView(View *view);

void GEO_DrawView(View *view);
void GEO_UpdateView(View *view);
void GEO_SaveImage(View *view, long int seed);

#endif
