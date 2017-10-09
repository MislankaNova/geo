#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include <stdio.h>

#include "geo.h"
#include "gen.h"
#include "algorithm.h"

void GEO_GEN_CalculateCity() {
  for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
    if (0 == i % ((MAP_SIZE * MAP_SIZE) / 100)) {
      printf("%i%% finished.\n", i / ((MAP_SIZE * MAP_SIZE) / 100));
    }
    Tile *tile = &geo->tiles[i];
    tile->city = 0;
    if (tile->life > 6) {
      GEO_ALG_CalculateTileDistance(tile, 240);
      for (int k = 0; k < MAP_SIZE * MAP_SIZE; ++k) {
        if (geo->tile_distances[k] < 240) {
          tile->city +=
            (geo->tiles[k].life * (240 - geo->tile_distances[k])) / 240;
        }
      }
      if (tile->flow > RIVER_THRESHOLD) {
        tile->city = (tile->city * 21) / 20;
      }
      for (int j = 0; j < 6; ++j) {
        if (tile->adj[j] && tile->adj[j]->elevation < 0) {
          tile->city = (tile->city * 20) / 20;
          break;
        }
      }
    }
  }
}

void GEO_GEN_PlaceCity() {
  Trig *place = NULL;
  int mc = 0;
  int total = (MAP_SIZE - 1) * 2 * (MAP_SIZE - 1);
  for (int i = 0; i < total; ++i) {
    Trig *trig = &geo->trigs[i];
    int city = (
        trig->vertices[VERT_TIP]->city  +
        trig->vertices[VERT_LEFT]->city +
        trig->vertices[VERT_RIGHT]->city) / 3;
    if (city > mc) {
      place = trig;
      mc = city;
    }
  }
  place->city = mc;
  for (int j = 0; j < 3; ++j) {
    GEO_ALG_CalculateTileDistance(place->vertices[j], 480);
    for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
      if (geo->tile_distances[i] < 480) {
        geo->tiles[i].city -= ((480 - geo->tile_distances[i])
                            * (480 - geo->tile_distances[i])) / 480;
        if (geo->tiles[i].city < 0) {
          geo->tiles[i].city = 0;
        }
      }
    }
  }
}

