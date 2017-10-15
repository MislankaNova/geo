#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>

#include "geo.h"
#include "algorithm.h"

const DistanceParameters DEFAULT_DISTANCE_PARAMETERS = {
  0,
  5,
  8,
  12,
  2.4
};

void _ALG_ResetTileDistances(int (*dest)[MAP_SIZE * MAP_SIZE]) {
  for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
    (*dest)[i] = INT_MAX;
  }
}

void _ALG_SetTileDistance(
    int (*dest)[MAP_SIZE * MAP_SIZE],
    const DistanceParameters *parameters,
    Tile *tile,
    int limit,
    int distance) {
  if (distance > limit
      || distance >= (*dest)[tile->y * MAP_SIZE + tile->x]) {
    return;
  }
  (*dest)[tile->y * MAP_SIZE + tile->x] = distance;
  distance += parameters->any;
  for (size_t j = 0; j < 6; ++j) {
    if (tile->adj[j]) {
      if (tile->elevation < 0 || tile->adj[j]->elevation < 0) {
        _ALG_SetTileDistance(
            dest,
            parameters,
            tile->adj[j],
            limit,
            distance + parameters->ocean
        );
      } else if ((j == tile->down
              && tile->elevation - tile->adj[j]->elevation < 16
              && tile->adj[j]->flow > RIVER_THRESHOLD
          ) || ((j == (tile->adj[j]->down + 3) % 6)
              && tile->adj[j]->elevation - tile->elevation < 16
              && tile->flow > RIVER_THRESHOLD
          )) {
        _ALG_SetTileDistance(
            dest,
            parameters,
            tile->adj[j],
            limit,
            distance + parameters->river
        );
      } else {
        int r = tile->adj[j]->slope;
        r = log(r);
        if (r < 0) {
          r = 0;
        }
        r = pow(r, parameters->slope_factor);
        _ALG_SetTileDistance(
            dest,
            parameters,
            tile->adj[j],
            limit,
            distance + r + parameters->land
        );
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

void GEO_ALG_CalculateTileDistance(
    const DistanceParameters *parameters,
    Tile *tile,
    int limit,
    int (*dest)[MAP_SIZE * MAP_SIZE]) {
  if (!parameters) {
    parameters = &DEFAULT_DISTANCE_PARAMETERS;
  }
  _ALG_ResetTileDistances(dest);
  _ALG_SetTileDistance(dest, parameters, tile, limit, 0);
}

