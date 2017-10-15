#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#ifndef __WIN32
#include <pthread.h>
#endif

#include <stdio.h>

#include "geo.h"
#include "gen.h"
#include "algorithm.h"

void _GEN_CalculateCitySingleThread() {
  for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
    if (0 == i % ((MAP_SIZE * MAP_SIZE) / 100)) {
      printf("\r%i%% finished.", i / ((MAP_SIZE * MAP_SIZE) / 100));
      fflush(stdout);
    }
    Tile *tile = &geo->tiles[i];
    tile->city = 0;
    if (tile->life > 6) {
      GEO_ALG_CalculateTileDistance(tile, 240, &geo->tile_distances);
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
  printf("\r");
}

#ifndef __WIN32

typedef struct Slice {
  size_t start;
  size_t end;
} Slice;

void *_GEN_CalculateCityInSlice(void *arg) {
  Slice *slice = arg;
  int distances[MAP_SIZE * MAP_SIZE];
  for (size_t i = slice->start; i < slice->end; ++i) {
    Tile *tile = &geo->tiles[i];
    tile->city = 0;
    if (tile->life > 6) {
      GEO_ALG_CalculateTileDistance(tile, 240, &distances);
      for (int k = 0; k < MAP_SIZE * MAP_SIZE; ++k) {
        if (distances[k] < 240) {
          tile->city +=
            (geo->tiles[k].life * (240 - distances[k])) / 240;
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
  return NULL;
}

void _GEN_CalculateCityMultiThread(int thread_count) {
  pthread_t threads[thread_count];
  Slice slices[thread_count];
  int step = (MAP_SIZE * MAP_SIZE) / thread_count;
  for (int i = 0; i < thread_count - 1; ++i) {
    slices[i].start = (size_t)(i * step);
    slices[i].end = (size_t)((i + 1) * step);
    pthread_create(
        &threads[i],
        NULL,
        _GEN_CalculateCityInSlice,
        &slices[i]
    );
  }
  slices[thread_count - 1].start = (size_t)((thread_count - 1) * step);
  slices[thread_count - 1].end = MAP_SIZE * MAP_SIZE;
  pthread_create(
      &threads[thread_count - 1],
      NULL,
      _GEN_CalculateCityInSlice,
      &slices[thread_count - 1]
  );
  for (int i = 0; i < thread_count; ++i) {
    pthread_join(threads[i], NULL);
  }
}

#endif

void GEO_GEN_CalculateCity(int thread_count) {
#ifdef __WIN32
  (void)thread_count;
  _GEN_CalculateCitySingleThread();
#else
  if (thread_count > 0) {
    _GEN_CalculateCityMultiThread(thread_count);
  } else {
    _GEN_CalculateCitySingleThread();
  }
#endif
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
    GEO_ALG_CalculateTileDistance(place->vertices[j], 480, &geo->tile_distances);
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

