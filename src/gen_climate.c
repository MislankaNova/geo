#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "geo.h"
#include "gen.h"

void GEO_GEN_CalculateHumidity() {
  for (int y = 0; y < MAP_SIZE; ++y) {
    for (int x = 0; x < MAP_SIZE; ++x) {
      TILE(y, x)->humidity = 10;
    }
  }
  for (int y = 0; y < MAP_SIZE; ++y) {
    for (int x = 0; x < 16; ++x) {
      if (TILE(y, x)->elevation < 0) {
        for (int c = 0; c < 10; c++) {
          Tile *current = TILE(y, x);
          Tile *next;
          for (int p = 2000; p > 0; --p) {
            int r = rand() % 100;
            if (r < 5) {
              next = current->adj[W];
            } else if (r < 18) {
              next = current->adj[NW];
            } else if (r < 30) {
              next = current->adj[SW];
            } else if (r < 48) {
              next = current->adj[NE];
            } else if (r < 60) {
              next = current->adj[SE];
            } else {
              next = current->adj[E];
            }
            if (NULL == next) {
              break;
            } else if (next->elevation >= 0) {
              int de = next->elevation - current->elevation;
              if (de > 0) {
                next->humidity += fmax(p * 0.000015 * de, p / 320);
                p -= de / 10;
              } else {
                next->humidity += p * 0.000002 * de;
                next->humidity += p / 320;
                p -= de / 16;
              }
            } else {
              next->humidity += p / 320;
            }
            current = next;
          }
        }
      }
    }
  }
  for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
    if (geo->tiles[i].humidity > 3000) {
      geo->tiles[i].humidity = 3000;
    }
  }
  int *inc = calloc(MAP_SIZE * MAP_SIZE, sizeof(int*));
  for (int c = 0; c < 5; ++c) {
    for (int y = 1; y < MAP_SIZE - 1; ++y) {
      for (int x = 1; x < MAP_SIZE - 1; ++x) {
        Tile *tile = TILE(y, x);
        int ht = tile->humidity;
        for (int i = 0; i < 6; ++i) {
          ht += tile->adj[i]->humidity;
        }
        inc[MAP_SIZE * y + x] = ht / 7;
      }
    }
    for (int y = 1; y < MAP_SIZE - 1; ++y) {
      for (int x = 1; x < MAP_SIZE - 1; ++x) {
        TILE(y, x)->humidity = inc[MAP_SIZE * y + x];
      }
    }
  }
  free(inc);
}

void GEO_GEN_CalculateLife() {
  for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
    Tile *tile = &geo->tiles[i];
    if (tile->elevation < -150) {
      tile->life = 1;
    } else if (tile->elevation < 0) {
      tile->life = 1;
    } else if (tile->elevation > 3000) {
      tile->life = 0;
    } else {
      tile->life = 0;
      if (tile->humidity < 120) {
        tile->life += tile->humidity / 20;
      } else if (tile->humidity < 1200) {
        tile->life += 7;
      } else {
        tile->life += 7;
      }
      for (int j = 0; j < 6; ++j) {
        if (tile->adj[j] && tile->adj[j]->elevation < 0) {
          tile->life += 2;
          break;
        }
      }
      int flow = tile->flow;
      for (int j = 0; j < 6; ++j) {
        if (tile->adj[j]) {
          flow += tile->adj[j]->flow / 12;
        }
      }
      if (flow > 2000) {
        tile->life += 10;
      } else {
        tile->life += tile->flow / 200;
      }
      if (tile->elevation > 1200) {
        tile->life -= (tile->elevation - 1200) / 240;
      }
      int r = tile->slope;
      r = log(r * r) * 6;
      if (r > 80) {
        tile->life -= (r - 80) / 20;
      }
      if (tile->life < 0) {
        tile->life = 0;
      } else if (tile->life > 20) {
        tile->life = 20;
      }
    }
  }
}
