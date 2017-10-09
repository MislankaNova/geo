#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include <stdio.h>

#include "geo.h"
#include "gen.h"
#include "noise.h"

Geo *geo;

void GEO_NewGeo(long int seed) {
  printf("- Creating new map\n");
  geo = malloc(sizeof(Geo));
  geo->seed = seed;
  printf("-- Allocating memory for tiles\n");
  geo->tiles = calloc(MAP_SIZE * MAP_SIZE, sizeof(Tile));
  for (int y = 0; y < MAP_SIZE; ++y) {
    for (int x = 0; x < MAP_SIZE; ++x) {
      Tile *tile = TILE(y, x);
      tile->id = MAP_SIZE * y + x;
      tile->y = y;
      tile->x = x;
      tile->elevation = 0;
      tile->humidity = 0;
      tile->flow = 0;
      tile->down = 0;
      int dx = y & 1 ? 1 : 0;
      tile->adj[W]  = TILE(y    , x - 1);
      tile->adj[NW] = TILE(y - 1, x - 1 + dx);
      tile->adj[NE] = TILE(y - 1, x + dx);
      tile->adj[E]  = TILE(y    , x + 1);
      tile->adj[SE] = TILE(y + 1, x     + dx);
      tile->adj[SW] = TILE(y + 1, x - 1 + dx);
    }
  }
  geo->tile_distances =
    calloc(MAP_SIZE * MAP_SIZE, sizeof(int));
  geo->trig_distances =
    calloc(2 * (MAP_SIZE - 1) * (MAP_SIZE - 1), sizeof(int));

  srand(seed);
  printf("-- Generating base elevation\n");
  GEO_GEN_InitialiseElevation();
  printf("-- Generating mountains\n");
  GEO_GEN_GenerateMountain();
  printf("-- Handling map edge\n");
  GEO_GEN_HandleEdge();
  GEO_GEN_AssignType();
  printf("-- Removing depression\n");
  GEO_GEN_RemoveDepression(-3000);
  GEO_GEN_AssignType();
  GEO_GEN_UpdateSlope();
  printf("-- Calculating Humidity\n");
  GEO_GEN_CalculateHumidity();
  printf("-- Generating rivers\n");
  for (int i = 0; i < 2; ++i) {
    printf("--- River pass %i\n", i + 1);
    GEO_GEN_RouteFlow();
    GEO_GEN_GenerateRivers(true);
    GEO_GEN_CalculateHumidity();
  }
  GEO_GEN_UpdateSlope();

  printf("-- Initialising map triangles\n");
  geo->trigs = calloc(2 * (MAP_SIZE - 1) * (MAP_SIZE - 1), sizeof(Trig));
  for (int y = 0; y < MAP_SIZE - 1; ++y) {
    for (int x = 0; x < 2 * (MAP_SIZE - 1); ++x) {
      Trig *trig = TRIG(y, x);
      trig->id = (MAP_SIZE - 1) * 2 * y + x;
      trig->y = y;
      trig->x = x;
      if (y & 1) {
        if (x & 1) {
          trig->vertices[VERT_TIP]   = TILE(y + 1, (x / 2) + 1);
          trig->vertices[VERT_LEFT]  = TILE(y    , (x / 2)    );
          trig->vertices[VERT_RIGHT] = TILE(y    , (x / 2) + 1);
        } else {
          trig->vertices[VERT_TIP]   = TILE(y    , (x / 2)    );
          trig->vertices[VERT_LEFT]  = TILE(y + 1, (x / 2)    );
          trig->vertices[VERT_RIGHT] = TILE(y + 1, (x / 2) + 1);
        }
      } else {
        if (x & 1) {
          trig->vertices[VERT_TIP]   = TILE(y    , (x / 2) + 1);
          trig->vertices[VERT_LEFT]  = TILE(y + 1, (x / 2)    );
          trig->vertices[VERT_RIGHT] = TILE(y + 1, (x / 2) + 1);
        } else {
          trig->vertices[VERT_TIP]   = TILE(y + 1, (x / 2)    );
          trig->vertices[VERT_LEFT]  = TILE(y    , (x / 2)    );
          trig->vertices[VERT_RIGHT] = TILE(y    , (x / 2) + 1);
        }
      }
      trig->adj[W]  = TRIG(x - 1, y);
      trig->adj[NW] = TRIG(x - 1, y - 1);
      trig->adj[N]  = TRIG(x    , y - 1);
      trig->adj[NE] = TRIG(x + 1, y - 1);
      trig->adj[E]  = TRIG(x + 1, y);
      trig->adj[SE] = TRIG(x + 1, y + 1);
      trig->adj[S]  = TRIG(x    , y + 1);
      trig->adj[SW] = TRIG(x - 1, y + 1);
      trig->elevation = (
          trig->vertices[VERT_TIP]->elevation  +
          trig->vertices[VERT_LEFT]->elevation +
          trig->vertices[VERT_RIGHT]->elevation) / 3;
      trig->slope = (
          trig->vertices[VERT_TIP]->slope  +
          trig->vertices[VERT_LEFT]->slope +
          trig->vertices[VERT_RIGHT]->slope) / 3;
    }
  }

  printf("-- Calculating life rating\n");
  GEO_GEN_CalculateLife();
  printf("-- Calculating city rating\n");
  GEO_GEN_CalculateCity();
  printf("-- Placing cities\n");
  for (int i = MAX_CITY_COUNT; i > 0; --i) {
    printf("--- Placing city %i\n", i + 1);
    GEO_GEN_PlaceCity();
  }

  printf("- Creating new map - DONE\n");
}

void GEO_DestroyGeo() {
  free(geo->tiles);
  free(geo->trigs);
  free(geo->tile_distances);
  free(geo->trig_distances);
  free(geo);
  geo = NULL;
}

Tile *GEO_GetTile(int y, int x) {
  if (y < 0 || x < 0 || y >= MAP_SIZE || x >= MAP_SIZE) {
    return NULL;
  }
  return &geo->tiles[MAP_SIZE * y + x];
}

Trig *GEO_GetTrig(int y, int x) {
  if (y < 0 || x < 0 || y >= MAP_SIZE - 1 || x >= (MAP_SIZE - 1) * 2) {
    return NULL;
  }
  return &geo->trigs[(MAP_SIZE - 1) * 2 * y + x];
}

int GEO_GetTileDistance(int y, int x) {
  if (y < 0 || x < 0 || y >= MAP_SIZE || x >= MAP_SIZE) {
    return 0x0FFFFFFF;
  }
  return geo->tile_distances[MAP_SIZE * y + x];
}

void GEO_SetTileDistance(int y, int x, int d) {
  if (y < 0 || x < 0 || y >= MAP_SIZE || x >= MAP_SIZE) {
    return;
  }
  geo->tile_distances[MAP_SIZE * y + x] = d;
}

int GEO_GetTrigDistance(int y, int x) {
  if (y < 0 || x < 0 || y >= MAP_SIZE - 1 || x >= (MAP_SIZE - 1) * 2) {
    return -1;
  }
  return geo->trig_distances[(MAP_SIZE - 1) * 2 * y + x];
}

void GEO_SetTrigDistance(int y, int x, int d) {
  if (y < 0 || x < 0 || y >= MAP_SIZE - 1 || x >= (MAP_SIZE - 1) * 2) {
    return;
  }
  geo->trig_distances[(MAP_SIZE - 1) * 2 * y + x] = d;
}

