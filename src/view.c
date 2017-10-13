#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include <stdio.h>

#include "view.h"
#include "geo.h"

const char* const _VIEW_MODE_NAMES[] = {
  "elevation", 
  "smooth-elevation",
  "humidity",
  "slope",
  "life",
  "city",
  "distance"
};

static inline int max3(int x, int y, int z) {
  return x > y ? (x > z ? x : z) : (y > z ? y : z);
}
static inline int min3(int x, int y, int z) {
  return x < y ? (x < z ? x : z) : (y < z ? y : z);
}

void _VIEW_InitialiseSurface(View *view) {
  if (view->draw_surface) {
    SDL_FreeSurface(view->draw_surface);
    view->draw_surface = NULL;
  }
  view->draw_surface = SDL_CreateRGBSurface(
      0,
      (MAP_SIZE + 1) * view->tile_size,
      MAP_SIZE * view->tile_size,
      32,
      0xFF000000,
      0x00FF0000,
      0x0000FF00,
      0x000000FF
  );
}

uint32_t _VIEW_GetElevationColour(int d) {
  uint32_t colour = 0;
  if (d < -1000) {
    colour = 0x000070FF;
  } else if (d < -500) {
    colour = 0x000088FF;
  } else if (d < 0) {
    colour = 0x0000A0FF;
  } else if (d < 50) {
    colour = 0x009000FF;
  } else if (d < 250) {
    colour = 0x089800FF;
    colour = 0x009000FF;
  } else if (d < 500) {
    colour = 0x10A008FF;
  } else if (d < 750) {
    colour = 0x18B010FF;
  } else if (d < 1000) {
    colour = 0x20C018FF;
  } else if (d < 1250) {
    colour = 0x30D018FF;
  } else if (d < 1500) {
    colour = 0x40D818FF;
  } else if (d < 1750) {
    colour = 0x50E020FF;
  } else if (d < 2000) {
    colour = 0x68C828FF;
  } else if (d < 2250) {
    colour = 0x80B028FF;
  } else if (d < 2500) {
    colour = 0x989828FF;
  } else if (d < 2750) {
    colour = 0xB08030FF;
  } else if (d < 3000) {
    colour = 0xC07030FF;
  } else {
    colour = 0xF0F0F0FF;
  }
  return colour;
}

uint32_t _VIEW_InterpolateTrig(Trig *trig, int size, int left, int base) {
  int et = trig->vertices[VERT_TIP]->elevation;
  int el = trig->vertices[VERT_LEFT]->elevation;
  int er = trig->vertices[VERT_RIGHT]->elevation;
  size += 1;
  double l1 = (double)(base) / (double)(size);
  double l2 = (double)((-size) * (left + 1.0) + (size / 2) * (base + 1.0))
    / (double)((-size) * size);
  double l3 = 1.0 - l1 - l2;
  return _VIEW_GetElevationColour(el * l3 + er * l2 + et * l1);
}

uint32_t _VIEW_RenderTrigPixel(Trig *trig, int size, int left, int base) {
  int et = trig->vertices[VERT_TIP]->elevation;
  int el = trig->vertices[VERT_LEFT]->elevation;
  int er = trig->vertices[VERT_RIGHT]->elevation;
  int sea = 0;
  if (et < 0) {
    ++sea;
  }
  if (el < 0) {
    ++sea;
  }
  if (er < 0) {
    ++sea;
  }
  if (base >= (size / 2)) {
    return _VIEW_GetElevationColour(et);
  } else if (base + left * 2 < size) {
    return _VIEW_GetElevationColour(el);
  } else if (base + (size - left - 1) * 2 < size) {
    return _VIEW_GetElevationColour(er);
  } else {
    switch (sea) {
      case 0: return _VIEW_GetElevationColour((et + el + er) / 3);
      case 1: return _VIEW_GetElevationColour(max3(et, el, er));
      case 2: return _VIEW_GetElevationColour(min3(et, el, er));
      case 3: return _VIEW_GetElevationColour((et + el + er) / 3);
    }
  }
  return -1;
}

void _VIEW_DrawCities(View *view) {
  SDL_Rect r;
  SDL_Renderer *sr = SDL_CreateSoftwareRenderer(view->draw_surface);
  SDL_SetRenderDrawColor(
      sr,
      0xF0,
      0,
      0,
      0xFF
  );
  for (int i = 0; i < (MAP_SIZE - 1) * 2 * (MAP_SIZE - 1); ++i) {
    Trig *trig = &geo->trigs[i];
    if (trig->city > 200) {
      r.w = 6;
      r.h = 6;
      r.y = view->tile_size * (trig->y + 1) - 3;
      r.x = (view->tile_size / 2) * (trig->x + 2) - 3;
      SDL_RenderFillRect(sr, &r);
    }
    if (trig->city > 800) {
      r.w = 10;
      r.h = 10;
      r.y = view->tile_size * (trig->y + 1) - 5;
      r.x = (view->tile_size / 2) * (trig->x + 2) - 5;
      SDL_RenderFillRect(sr, &r);
    }
    if (trig->city > 1600) {
      r.w = 14;
      r.h = 14;
      r.y = view->tile_size * (trig->y + 1) - 7;
      r.x = (view->tile_size / 2) * (trig->x + 2) - 7;
      SDL_RenderDrawRect(sr, &r);
    }
  }
  SDL_DestroyRenderer(sr);
}

View *GEO_NewView(SDL_Renderer *renderer) {
  View *view = malloc(sizeof(View));
  view->mode = GEO_VIEW_MODE_ELEVATION;
  view->renderer = renderer;
  view->move_x = 0;
  view->move_y = 0;
  view->centre_x = 0;
  view->centre_y = 0;
  view->tile_size = 4;
  view->map_texture = NULL;
  view->draw_surface = NULL;
  GEO_UpdateView(view);
  return view;
}

void GEO_DestroyView(View *view) {
  if (view->draw_surface) {
    SDL_FreeSurface(view->draw_surface);
  }
  free(view);
}

void GEO_DrawView(View *view) {
  int side = MAP_SIZE * view->tile_size;
  SDL_Rect source;
  SDL_Rect dest;
  SDL_Rect selection;
  selection.h = view->tile_size;
  selection.w = view->tile_size;
  selection.y = 300 - view->tile_size / 2;
  selection.x = 400 - view->tile_size / 2;
  if (view->centre_y & 1) {
    selection.x += view->tile_size / 2;
  }
  source.h = MAP_SIZE * view->tile_size;
  source.w = MAP_SIZE * view->tile_size + view->tile_size / 2;
  source.y = 0;
  source.x = 0;
  dest.h = side;
  dest.w = side + view->tile_size / 2;
  dest.y = 300 - view->tile_size / 2 - view->tile_size * view->centre_y;
  dest.x = 400 - view->tile_size / 2 - view->tile_size * view->centre_x;
  SDL_RenderCopy(
      view->renderer,
      view->map_texture,
      &source,
      &dest
  );

  SDL_SetRenderDrawColor(
      view->renderer,
      0xFF,
      0,
      0,
      0xFF
  );
  SDL_RenderDrawRect(
      view->renderer,
      &selection
  );
}

void GEO_UpdateViewSlope(View *view) {
  SDL_Rect r;
  r.w = view->tile_size;
  r.h = view->tile_size;
  for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
    Tile *tile = &geo->tiles[i];
    double slope = (double)tile->slope / 4000.0;
    if (slope > 1.0) {
      slope = 1.0;
    }
    slope = log10(10.0 + 89.9 * slope) - 1.0;
    slope *= 4000.0;
    uint32_t colour;
    unsigned char d = 0xFF * ((double)((int)slope / 400) / 10.0);
    d = 0xFF - d;
    if (tile->elevation >= 0) {
      colour = 0xF0 << 24 | d << 16 | d << 8 | 0xFF;
    } else {
      colour = d << 24 | d << 16 | 0xF0 << 8 | 0xFF;
    }
    r.y = view->tile_size * tile->y;
    r.x = view->tile_size * tile->x;
    if (tile->y & 1) {
      r.x += view->tile_size / 2;
    }
    SDL_FillRect(view->draw_surface, &r, colour);
  }
}

void GEO_UpdateViewHumidity(View *view) {
  SDL_Rect r;
  r.w = view->tile_size;
  r.h = view->tile_size;
  for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
    Tile *tile = &geo->tiles[i];
    uint32_t colour = 0xD0D0D0FF;
    if (tile->elevation >= 0) {
      unsigned char d =
        0xFF *
        ((double)(tile->humidity / 200) / 16);
      d = 0xFF - d;
      colour = d << 24 | d << 16 | 0xF0 << 8 | 0xFF;
    }
    r.y = view->tile_size * tile->y;
    r.x = view->tile_size * tile->x;
    if (tile->y & 1) {
      r.x += view->tile_size / 2;
    }
    SDL_FillRect(view->draw_surface, &r, colour);
  }
}

void GEO_UpdateViewLife(View *view) {
  SDL_Rect r;
  r.w = view->tile_size;
  r.h = view->tile_size;
  for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
    Tile *tile = &geo->tiles[i];
    uint32_t colour = 0xD0D0D0FF;
    if (tile->elevation >= -150) {
      int d = (0xFF * tile->life) / 20;
      d = 0xFF - d;
      colour = d << 24 | 0xF0 << 16 | d << 8 | 0xFF;
    }
    r.y = view->tile_size * tile->y;
    r.x = view->tile_size * tile->x;
    if (tile->y & 1) {
      r.x += view->tile_size / 2;
    }
    SDL_FillRect(view->draw_surface, &r, colour);
  }
}

void GEO_UpdateViewCity(View *view) {
  SDL_Rect r;
  r.w = view->tile_size;
  r.h = view->tile_size;
  for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
    Tile *tile = &geo->tiles[i];
    uint32_t colour = 0xD0D0D0FF;
    if (tile->elevation >= 0) {
      int d = (0xFF * tile->city) / 2400;
      d = 0xFF - d;
      colour = d << 24 | d << 16 | 0xF0 << 8 | 0xFF;
    }
    r.y = view->tile_size * tile->y;
    r.x = view->tile_size * tile->x;
    if (tile->y & 1) {
      r.x += view->tile_size / 2;
    }
    SDL_FillRect(view->draw_surface, &r, colour);
  }
}

void GEO_UpdateViewTrig(View *view) {
  SDL_Renderer *sr = SDL_CreateSoftwareRenderer(view->draw_surface);
  for (int y = 0; y <MAP_SIZE - 1; ++y) {
    for (int x = 0; x < 2 * (MAP_SIZE - 1); ++x) {
      int d = TRIG(y, x)->elevation;
      uint32_t colour = _VIEW_GetElevationColour(d);
      SDL_SetRenderDrawColor(
          sr,
          (colour & 0xFF000000) >> 24,
          (colour & 0x00FF0000) >> 16,
          (colour & 0x0000FF00) >> 8,
          0xFF
      );
      int dis = view->tile_size / 2;
      int s = view->tile_size;
      if (y & 1) {
        if (x & 1) {
          for (int i = 0; i < s; ++i) {
            for (int j = i / 2; j < s - i / 2; ++j) {
              colour = _VIEW_InterpolateTrig(
                  TRIG(y, x),
                  view->tile_size,
                  j,
                  i
              );
              SDL_SetRenderDrawColor(
                  sr,
                  (colour & 0xFF000000) >> 24,
                  (colour & 0x00FF0000) >> 16,
                  (colour & 0x0000FF00) >> 8,
                  0xFF
              );
              SDL_RenderDrawPoint(
                  sr,
                  (x / 2) * s + j + s / 2 + dis,
                  y * s + i + dis
              );
            }
          }
        } else {
          for (int i = 0; i < s; ++i) {
            for (int j = i / 2; j < s - i / 2; ++j) {
              colour = _VIEW_InterpolateTrig(
                  TRIG(y, x),
                  view->tile_size,
                  j,
                  i
              );
              SDL_SetRenderDrawColor(
                  sr,
                  (colour & 0xFF000000) >> 24,
                  (colour & 0x00FF0000) >> 16,
                  (colour & 0x0000FF00) >> 8,
                  0xFF
              );
              SDL_RenderDrawPoint(
                  sr,
                  (x / 2) * s + j + dis,
                  (y + 1) * s - i + dis
              );
            }
          }
        }
      } else {
        if (x & 1) {
          for (int i = 0; i < s; ++i) {
            for (int j = i / 2; j < s - i / 2; ++j) {
              colour = _VIEW_InterpolateTrig(
                  TRIG(y, x),
                  view->tile_size,
                  j,
                  i
              );
              SDL_SetRenderDrawColor(
                  sr,
                  (colour & 0xFF000000) >> 24,
                  (colour & 0x00FF0000) >> 16,
                  (colour & 0x0000FF00) >> 8,
                  0xFF
              );
              SDL_RenderDrawPoint(
                  sr,
                  (x / 2) * s + j + s / 2 + dis,
                  (y + 1) * s - i + dis
              );
            }
          }
        } else {
          for (int i = 0; i < s; ++i) {
            for (int j = i / 2; j < s - i / 2; ++j) {
              colour = _VIEW_InterpolateTrig(
                  TRIG(y, x),
                  view->tile_size,
                  j,
                  i
              );
              SDL_SetRenderDrawColor(
                  sr,
                  (colour & 0xFF000000) >> 24,
                  (colour & 0x00FF0000) >> 16,
                  (colour & 0x0000FF00) >> 8,
                  0xFF
              );
              SDL_RenderDrawPoint(
                  sr,
                  (x / 2) * s + j + dis,
                  y * s + i + dis
              );
            }
          }
        }
      }
    }
  }
  for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
    Tile *tile = &geo->tiles[i];
    if (tile->flow > RIVER_THRESHOLD) {
      int y1 = tile->y;
      int x1 = tile->x;
      int y2 = tile->adj[tile->down]->y;
      int x2 = tile->adj[tile->down]->x;
      y1 = y1 * (view->tile_size) + (view->tile_size / 2);
      x1 = x1 * (view->tile_size) + (view->tile_size / 2);
      y2 = y2 * (view->tile_size) + (view->tile_size / 2);
      x2 = x2 * (view->tile_size) + (view->tile_size / 2);
      if (tile->y & 1) {
        x1 += (view->tile_size / 2);
      }
      if (tile->adj[tile->down]->y & 1) {
        x2 += (view->tile_size / 2);
      }
      int d0 = tile->flow / 5000;
      int d1 = (tile->flow - 2500) / 5000;
      int t0;
      int t1;
      if (view->tile_size / 8 > d1) {
        t0 = d0;
        t1 = d1;
      } else {
        t0 = view->tile_size / 8;
        t1 = view->tile_size / 8;
      }
      SDL_SetRenderDrawColor(sr, 0, 0, 0xA0, 0xFF);
      for (int j = -t0; j <= t1; ++j) {
        for (int k = -t0; k <= t1; ++k) {
          SDL_RenderDrawLine(
              sr,
              x1 + j,
              y1 + k,
              x2 + j,
              y2 + k
          );
        }
      }
    }
  }
  SDL_DestroyRenderer(sr);
}

void GEO_UpdateViewElevation(View *view) {
  SDL_Rect r;
  r.w = view->tile_size;
  r.h = view->tile_size;
  SDL_Renderer *sr = SDL_CreateSoftwareRenderer(view->draw_surface);
  for (int y = 0; y < MAP_SIZE; ++y) {
    for (int x = 0; x < MAP_SIZE; ++x) {
      r.y = y * view->tile_size;
      r.x = x * view->tile_size;
      if (y & 1) {
        r.x += view->tile_size / 2;
      }
      int d = geo->tiles[MAP_SIZE * y + x].elevation;
      uint32_t colour = _VIEW_GetElevationColour(d);
      if (TILE_LAKE == geo->tiles[MAP_SIZE * y + x].type) {
        colour = 0x1030C0FF;
      }
      SDL_SetRenderDrawColor(
          sr,
          (colour & 0xFF000000) >> 24,
          (colour & 0x00FF0000) >> 16,
          (colour & 0x0000FF00) >> 8,
          0xFF
      );
      SDL_RenderFillRect(sr, &r);
    }
  }
  for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
    Tile *tile = &geo->tiles[i];
    if (tile->flow > RIVER_THRESHOLD) {
      int y1 = tile->y;
      int x1 = tile->x;
      int y2 = tile->adj[tile->down]->y;
      int x2 = tile->adj[tile->down]->x;
      y1 = y1 * (view->tile_size) + (view->tile_size / 2);
      x1 = x1 * (view->tile_size) + (view->tile_size / 2);
      y2 = y2 * (view->tile_size) + (view->tile_size / 2);
      x2 = x2 * (view->tile_size) + (view->tile_size / 2);
      if (tile->y & 1) {
        x1 += (view->tile_size / 2);
      }
      if (tile->adj[tile->down]->y & 1) {
        x2 += (view->tile_size / 2);
      }
      int d0 = tile->flow / 5000;
      int d1 = (tile->flow - 2500) / 5000;
      int t0;
      int t1;
      if (view->tile_size / 8 > d1) {
        t0 = d0;
        t1 = d1;
      } else {
        t0 = view->tile_size / 8;
        t1 = view->tile_size / 8;
      }
      SDL_SetRenderDrawColor(sr, 0, 0, 0xA0, 0xFF);
      for (int j = -t0; j <= t1; ++j) {
        for (int k = -t0; k <= t1; ++k) {
          SDL_RenderDrawLine(
              sr,
              x1 + j,
              y1 + k,
              x2 + j,
              y2 + k
          );
        }
      }
    }
  }
  SDL_DestroyRenderer(sr);
}

void GEO_UpdateViewDistance(View *view) {
  SDL_Rect r;
  r.w = view->tile_size;
  r.h = view->tile_size;
  SDL_Renderer *sr = SDL_CreateSoftwareRenderer(view->draw_surface);
  SDL_SetRenderDrawBlendMode(sr, SDL_BLENDMODE_BLEND);
  for (int y = 0; y < MAP_SIZE; ++y) {
    for (int x = 0; x < MAP_SIZE; ++x) {
      r.y = y * view->tile_size;
      r.x = x * view->tile_size;
      if (y & 1) {
        r.x += view->tile_size / 2;
      }
      int d = GEO_GetTileDistance(y, x) / 200;
      if (d & 1) {
        SDL_SetRenderDrawColor(
            sr,
            0xFF,
            0xFF,
            0xFF,
            0x60
        );
      } else {
        SDL_SetRenderDrawColor(
            sr,
            0,
            0,
            0,
            0x60
        );
      }
      if (d > 0) {
        SDL_RenderFillRect(sr, &r);
      }
    }
  }

  SDL_DestroyRenderer(sr);
}

void GEO_UpdateView(View *view) {
  if (view->map_texture) {
    SDL_DestroyTexture(view->map_texture);
    view->map_texture = NULL;
  }
  _VIEW_InitialiseSurface(view);
  switch (view->mode) {
    case GEO_VIEW_MODE_ELEVATION:
      GEO_UpdateViewElevation(view);
      _VIEW_DrawCities(view);
      break;
    case GEO_VIEW_MODE_TRIG:
      GEO_UpdateViewTrig(view);
      _VIEW_DrawCities(view);
      break;
    case GEO_VIEW_MODE_HUMIDITY:
      GEO_UpdateViewHumidity(view);
      break;
    case GEO_VIEW_MODE_SLOPE:
      GEO_UpdateViewSlope(view);
      break;
    case GEO_VIEW_MODE_LIFE:
      GEO_UpdateViewLife(view);
      break;
    case GEO_VIEW_MODE_CITY:
      GEO_UpdateViewCity(view);
      break;
    case GEO_VIEW_MODE_DISTANCE:
      GEO_UpdateViewElevation(view);
      GEO_UpdateViewDistance(view);
      _VIEW_DrawCities(view);
      break;
  }
  view->map_texture = SDL_CreateTextureFromSurface(
      view->renderer,
      view->draw_surface
  );
}

void GEO_SaveImage(View *view) {
  if (!view->draw_surface) {
    return;
  } else if (view->tile_size > 10) {
    printf("* Tile size is too large.\n");
    return;
  }
  char *filename = calloc(1, 255);
  char const *mode_name = _VIEW_MODE_NAMES[view->mode];
  snprintf(
      filename,
      255,
      "%s.bmp", mode_name
  );
  SDL_SaveBMP(view->draw_surface, filename);
  free(filename);
}

