#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include <stdio.h>

#include "geo.h"
#include "gen.h"

void _GEN_ErodeBack(
    Tile *tile, 
    int p0,
    int p1,
    int p2,
    int p3,
    int p4,
    int p5) {
  if (p0 - tile->flow > 2000) {
    p0 = tile->flow + 2000;
  }
  int discharge = (tile->flow * 92 + p0 * 3 + p1 + p2 + p3 + p4 + p5) / 100;
  if (discharge > 4000) {
    discharge = 4000;
  } else if (0 == discharge) {
    discharge = 1;
  }
  int allow = (TILE_SIZE / discharge) * 2;
  allow += rand() % (TILE_SIZE / discharge);
  if (tile->elevation < 0) {
    allow *= 50;
  }
  if (tile->elevation > tile->adj[tile->down]->elevation + allow) {
    tile->elevation = tile->adj[tile->down]->elevation + allow;
  }
  for (int j = 0; j < 6; ++j) {
    if (tile == tile->adj[j]->adj[tile->adj[j]->down]) {
      _GEN_ErodeBack(tile->adj[j], tile->flow, p0, p1, p2, p3, p4);
    }
  }
}

void GEO_GEN_GenerateRivers() {
  for (int y = 1; y < MAP_SIZE - 1; ++y) {
    for (int x = 1; x < MAP_SIZE - 1; ++x) {
      TILE(y, x)->flow = 0;
    }
  }

  for (int y = 1; y < MAP_SIZE - 1; ++y) {
    for (int x = 1; x < MAP_SIZE - 1; ++x) {
      Tile *last = TILE(y, x);
      if (last->elevation > 0) {
        int discharge = last->humidity / 80;
        if (discharge > 5) {
          discharge = 5;
        }
        last->flow += discharge;
        Tile *next = last->adj[last->down];
        while (next && next->elevation >= -200) {
          next->flow += discharge;
          last = next;
          next = next->adj[next->down];
        }
        _GEN_ErodeBack(
            last,
            last->flow,
            last->flow,
            last->flow,
            last->flow,
            last->flow,
            last->flow
        );
      }
    }
  }
}

