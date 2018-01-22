#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include <stdio.h>

#include "geo.h"
#include "gen.h"

static const int SOFT_FLOW_PARAMETERS[] = {
  1000,
  500,
  200,
  200,
  200,
  200,
  100,
  100,
  100,
  100,
  0
};

static const int AVERAGE_FLOW_PARAMETERS[] = {
  790,
  6,
  2,
  1,
  1,
  0,
  0,
  0,
  0,
  0,
  0
};

static const int HARD_FLOW_PARAMETERS[] = {
  600,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};

static const int VERY_HARD_FLOW_PARAMETERS[] = {
  400,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};

void _GEN_ErodeBack(
    Tile *tile,
    int previous[static 10]) {
  Hardness hardness = tile->base_hardness;
  if (tile->base_elevation - tile->elevation < 200) {
    hardness = tile->hardness;
  }
  int const *parameters = AVERAGE_FLOW_PARAMETERS;
  switch (hardness) {
    case HARDNESS_SOFT: parameters = SOFT_FLOW_PARAMETERS; break;
    case HARDNESS_AVERAGE: parameters = AVERAGE_FLOW_PARAMETERS; break;
    case HARDNESS_HARD: parameters = HARD_FLOW_PARAMETERS; break;
    case HARDNESS_VERY_HARD: parameters = VERY_HARD_FLOW_PARAMETERS; break;
  }
  int discharge = parameters[0] * tile->flow;
  for (int i = 0; i < 10; ++i) {
    discharge += parameters[i + 1] * previous[i];
  }
  if (discharge > 4000000) {
    discharge = 4000000;
  } else if (0 == discharge) {
    discharge = 1;
  }
  int allow = (TILE_SIZE * 2000) / discharge;
  allow += rand() % ((TILE_SIZE * 1000) / discharge);
  if (tile->elevation < 0) {
    allow *= 50;
  }
  if (tile->elevation > tile->adj[tile->down]->elevation + allow) {
    tile->elevation = tile->adj[tile->down]->elevation + allow;
  }
  int next[10];
  next[0] = tile->flow;
  for (int i = 0; i < 9; ++i) {
    next[i + 1] = previous[i];
  }
  for (int j = 0; j < 6; ++j) {
    if (tile == tile->adj[j]->adj[tile->adj[j]->down]) {
      _GEN_ErodeBack(tile->adj[j], next);
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
        int previous[10];
        for (int i = 0; i < 10; ++i) {
          previous[i] = last->flow;
        }
        _GEN_ErodeBack(last, previous);
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
