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

void GEO_GEN_WalkRivers() {
  for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
    Tile *tile = &geo->tiles[i];
    Tile *down = tile->adj[tile->down];
    if (tile->flow > RIVER_THRESHOLD) {
      RiverNode *river = malloc(sizeof(RiverNode));
      RiverNode *mid = malloc(sizeof(RiverNode));
      RiverNode *last = malloc(sizeof(RiverNode));
      river->x = (double)tile->x;
      river->y = (double)tile->y;
      river->flow = tile->flow;
      river->elevation = tile->elevation;
      river->next = mid;
      last->x = (double)down->x;
      last->y = (double)down->y;
      last->flow = down->flow;
      last->elevation = down->elevation;
      last->next = NULL;
      if (tile->y & 1) {
        river->x += 0.5;
      }
      if (down->y & 1) {
        last->x += 0.5;
      }
      mid->x = (river->x + last->x) / 2.0;
      mid->y = (river->y + last->y) / 2.0;
      mid->x += 0.25 - ((double)rand() / (double)RAND_MAX) * 0.5;
      mid->y += 0.25 - ((double)rand() / (double)RAND_MAX) * 0.5;
      mid->flow = (river->flow + last->flow) / 2;
      mid->elevation = (river->elevation + last->elevation) / 2;
      mid->next = last;
      tile->river = river;
    }
  }
}
