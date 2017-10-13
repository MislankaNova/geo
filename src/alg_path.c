#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "geo.h"

void _ALG_ResetTileDistances() {
  for (int y = 0; y < MAP_SIZE; ++y) {
    for (int x = 0; x < MAP_SIZE; ++x) {
      GEO_SetTileDistance(y, x, 0x0FFFFFFF);
    }
  }
}

void _ALG_SetTileDistance(Tile *tile, int limit, int distance) {
  if (distance > limit
      || distance >= GEO_GetTileDistance(tile->y, tile->x)) {
    return;
  }
  GEO_SetTileDistance(tile->y, tile->x, distance);
  for (size_t j = 0; j < 6; ++j) {
    if (tile->adj[j]) {
      if (tile->elevation < 0 || tile->adj[j]->elevation < 0) {
        _ALG_SetTileDistance(tile->adj[j], limit, distance + 5);
      } else if ((j == tile->down
              && tile->elevation - tile->adj[j]->elevation < 16
              && tile->adj[j]->flow > RIVER_THRESHOLD
          ) || ((j == (tile->adj[j]->down + 3) % 6)
              && tile->adj[j]->elevation - tile->elevation < 16
              && tile->flow > RIVER_THRESHOLD
          )) {
        _ALG_SetTileDistance(tile->adj[j], limit, distance + 8);
      } else {
        int r = tile->adj[j]->slope;
        r = log(r);
        if (r < 0) {
          r = 0;
        }
        r = pow(r, 2.4);
        _ALG_SetTileDistance(tile->adj[j], limit, distance + r + 12);
      }
    }
  }
}

void _ALG_SetTrigDistance(Trig *trig, int limit, int distance) {
  if (distance > limit || distance >= GEO_GetTrigDistance(trig->y, trig->x)) {
    return;
  }
  GEO_SetTrigDistance(trig->y, trig->x, distance);
  for (int j = 0; j < 6; ++j) {
  }
}

void GEO_ALG_CalculateTileDistance(Tile *tile, int limit) {
  _ALG_ResetTileDistances();
  _ALG_SetTileDistance(tile, limit, 0);
}

