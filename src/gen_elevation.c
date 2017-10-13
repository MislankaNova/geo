#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include <stdio.h>

#include "geo.h"
#include "gen.h"

#include "noise.h"

void _GEN_GenerateMountain(
    Tile *tile,
    int height,
    int decline) {
  height -= rand() % decline;
  if ((height - tile->elevation) < 20) {
    return;
  }
  tile->elevation = height;
  for (int i = 0; i < 6; ++i) {
    if (tile->adj[i]) {
      _GEN_GenerateMountain(tile->adj[i], height - 50, decline);
    }
  }
}

void GEO_GEN_InitialiseElevation() {
  int size = MAP_SIZE / 3;
  GradientGenerator *g0 = new_gradient_generator(geo->seed + 1, size, size);
  GradientGenerator *g1 = new_gradient_generator(geo->seed + 2, size, size);
  GradientGenerator *g2 = new_gradient_generator(geo->seed + 3, size, size);
  GradientGenerator *g3 = new_gradient_generator(geo->seed + 4, size, size);
  GradientGenerator *g4 = new_gradient_generator(geo->seed + 5, size, size);
  for (int y = 0; y < MAP_SIZE; ++y) {
    for (int x = 0; x < MAP_SIZE; ++x) {
      double dx = y & 1 ? 0.5 : 0.0;
      double h = 0.0;
      h += pow(get_gradient_value(
          g0,
          60.2 + (double)y / 100.0,
          60.2 + ((double)x + dx) / 100.0
      ), 3) * 0.4;
      h += pow(log10(20.0 + 79.0 * get_gradient_value(
          g1,
          0.3 + (double)y / 16.0,
          0.3 + ((double)x + dx) / 80.0
      )) - 1.0, 0.8) * 0.3;
      h += pow(log10(20.0 + 79.0 * get_gradient_value(
          g1,
          24.3 + (double)y / 80.0,
          25.3 + ((double)x + dx) / 16.0
      )) - 1.0, 0.8) * 0.3;
      h = pow(h, 1.5) * 0.7;
      h += get_gradient_value(
          g2,
          0.25 + (double)y / 24.0,
          0.25 + ((double)x + dx) / 24.0
      ) * 0.3;
      h = pow(h, 1.2) * 0.7;
      h += get_gradient_value(
          g3,
          0.25 + (double)y / 32.0,
          0.25 + ((double)x + dx) / 32.0
      ) * 0.1;
      h += get_gradient_value(
          g4,
          0.05 + (double)y / 3.5,
          0.05 + ((double)x + dx) / 3.5
      ) * get_gradient_value(
          g3,
          0.05 + (double)y / 12.0,
          0.05 + ((double)x + dx) / 12.0
      ) * get_gradient_value(
          g3,
          0.05 + (double)y / 32.0,
          0.05 + ((double)x + dx) / 32.0
      ) * 0.18;
      h += get_gradient_value(
          g1,
          0.75 + (double)y / 4.0,
          0.75 + ((double)x + dx) / 4.0
      ) * 0.02;
      if (h > 0.301) {
        h = 0.301 + pow(h - 0.301, 1.2);
      }
      if (h > 0.375) {
        h = 0.375 + pow((h - 0.375) * 2.0, 0.75) * 0.5;
      }
      if (h > 0.45) {
        h = 0.45 + pow(h - 0.45, 1.2);
      }
      if (h < 0.3002 && 0 > 0.2998) {
        h = 0.3002;
      }
      if (h > 0.3 && h < 0.325) {
        h = 0.3 + pow((h - 0.3) / 0.25, 100.0) * 0.25;
      }
      TILE(y, x)->elevation = h * 10000 - 3000;
    }
  }
  destroy_gradient_generator(g0);
  destroy_gradient_generator(g1);
  destroy_gradient_generator(g2);
  destroy_gradient_generator(g3);
  destroy_gradient_generator(g4);
}

void GEO_GEN_GenerateShield() {
  int size = MAP_SIZE / 3;
  GradientGenerator *g0 = new_gradient_generator(geo->seed + 8, size, size);
  GradientGenerator *g1 = new_gradient_generator(geo->seed + 9, size, size);
  GradientGenerator *g2 = new_gradient_generator(geo->seed + 3, size, size);
  GradientGenerator *g3 = new_gradient_generator(geo->seed + 4, size, size);
  GradientGenerator *g4 = new_gradient_generator(geo->seed + 5, size, size);
  VoronoiGenerator *vg0 = new_voronoi_generator(
      (MAP_SIZE * MAP_SIZE) / 2000,
      MAP_SIZE,
      MAP_SIZE
  );
  for (int y = 0; y < MAP_SIZE; ++y) {
    for (int x = 0; x < MAP_SIZE; ++x) {
      double dx = y & 1 ? 0.5 : 0.0;
      double s = 1.0 - pow(get_voronoi_value(
          vg0, y, x
      ) / vg0->min, 1.25);
      s = s * 0.7 + s * 0.3 * get_gradient_value(
          g0,
          (double)y / 32.0,
          ((double)x + dx) / 32.0
      );

      double h = 0.0;
      h += pow(get_gradient_value(
          g4,
          60.2 + (double)y / 100.0,
          60.2 + ((double)x + dx) / 100.0
      ), 3) * 0.4;
      h += pow(log10(20.0 + 79.0 * get_gradient_value(
          g2,
          0.3 + (double)y / 16.0,
          0.3 + ((double)x + dx) / 80.0
      )) - 1.0, 0.8) * 0.3;
      h += pow(log10(20.0 + 79.0 * get_gradient_value(
          g4,
          24.3 + (double)y / 80.0,
          25.3 + ((double)x + dx) / 16.0
      )) - 1.0, 0.8) * 0.3;
      h = pow(h, 1.5) * 0.7;
      h += get_gradient_value(
          g1,
          0.25 + (double)y / 24.0,
          0.25 + ((double)x + dx) / 24.0
      ) * 0.3;
      h = pow(h, 1.2) * 0.7;
      h += get_gradient_value(
          g0,
          0.25 + (double)y / 32.0,
          0.25 + ((double)x + dx) / 32.0
      ) * 0.1;
      h += get_gradient_value(
          g3,
          0.05 + (double)y / 3.5,
          0.05 + ((double)x + dx) / 3.5
      ) * get_gradient_value(
          g2,
          0.05 + (double)y / 12.0,
          0.05 + ((double)x + dx) / 12.0
      ) * get_gradient_value(
          g1,
          0.05 + (double)y / 32.0,
          0.05 + ((double)x + dx) / 32.0
      ) * 0.18;
      h += get_gradient_value(
          g0,
          0.75 + (double)y / 4.0,
          0.75 + ((double)x + dx) / 4.0
      ) * 0.02;

      int o = MAP_SIZE / 2;
      double d = abs(y - o) > abs(x - o) ? abs(y - o) : abs (x - o) ;
      double dm = 0;
      dm += get_gradient_value(
          g0,
          20.8 + (double)y / 60.0,
          20.8 + ((double)x + dx) / 60.0
      ) * 0.9;
      dm += get_gradient_value(
          g1,
          11.2 + (double)y / 16.0,
          11.2 + ((double)x + dx) / 16.0
      ) * 0.1;
      dm *= MAP_SIZE / 2.5;

      if (s < 0.75) {
        s = pow(s, 2.0);
      }
      h = pow(h, 0.6);
      if (s > 0.6) {
        s = pow(s, 0.75);
        if (d < dm) {
          TILE(y, x)->elevation = 400 + s * 800 + h * 1200;
        } else if (d - dm < 8) {
          TILE(y, x)->elevation = 50 * (d - dm) + s * 1200 + h * 800;
        }
      }
    }
  }
  destroy_voronoi_generator(vg0);
  destroy_gradient_generator(g0);
  destroy_gradient_generator(g1);
  destroy_gradient_generator(g2);
  destroy_gradient_generator(g3);
  destroy_gradient_generator(g4);
}

void GEO_GEN_GenerateMountain() {
  GradientGenerator *g0 = new_gradient_generator(geo->seed + 5, 4, 4);
  GradientGenerator *g1 = new_gradient_generator(geo->seed + 6, 8, 8);
  GradientGenerator *g2 = new_gradient_generator(geo->seed + 7, 8, 8);
  VoronoiGenerator *vg0 = new_voronoi_generator(
      (MAP_SIZE * MAP_SIZE) / 2000,
      MAP_SIZE,
      MAP_SIZE
  );
  for (int i = 0; i < MOUNTAIN_COUNT * 5; ++i) {
    int y = MAP_SIZE / 4 + (rand() % (MAP_SIZE / 2));
    int x = MAP_SIZE / 4 + (rand() % (MAP_SIZE / 2));
    Tile *current = TILE(y, x);
    if (current->elevation < -300.0
        || current->elevation > -100.0
        || pow(get_gradient_value(
        g0,
        0.5 + (double)y / (MAP_SIZE / 4.0),
        0.5 + ((double)x) / (MAP_SIZE / 4.0)
    ), 1.2) < 0.4) {
      --i;
    } else {
      Direction direction = rand() % 6;
      int degree = (rand() % 800) + 1200;
      for (int p = rand() % 10; p < MAP_SIZE; p += rand() % 18) {
        _GEN_GenerateMountain(
            current,
            (degree + rand() % 400),
            1200 + rand() % 400
        );
        int r = rand() % 6;
        if (r < 1) {
          direction = (direction + 7) % 6;
        } else if (r < 2) {
          direction = (direction + 5) % 6;
        }
        for (int j = (rand() % 4) * (rand() % 2); j > 0; --j) {
          current = current->adj[(direction + 6) % 6];
          if (NULL == current) {
            break;
          }
        }
        if (NULL == current) {
          break;
        }
      }
    }
  }

  for (int i = 0; i < MOUNTAIN_COUNT; ++i) {
    int y = MAP_SIZE / 4 + (rand() % (MAP_SIZE / 2));
    int x = MAP_SIZE / 4 + (rand() % (MAP_SIZE / 2));
    Tile *current = TILE(y, x);
    Direction direction = (int)(get_gradient_value(
        g0,
        0.5 + (double)y / (MAP_SIZE),
        0.5 + ((double)x) / (MAP_SIZE)
    ) + (get_gradient_value(
        g1,
        0.5 + (double)y / (MAP_SIZE * 3),
        0.5 + ((double)x) / (MAP_SIZE * 3)
    ) * 18.0)) % 6;
    Direction facing = direction;
    int degree = (rand() % 600) + 3000;
    for (int p = rand() % 10; p < MAP_SIZE; p += rand() % 4) {
      _GEN_GenerateMountain(
          current,
          (degree + rand() % 800)
            * (0.85 + 0.15
                    * sin(((double)((p * MAP_SIZE) / 180) / 40.0) * PI)),
          pow((double)degree / 5000.0, 1.2) * 2400 + rand() % 600
      );
      int r = rand() % 6;
      if (r < 1) {
        direction = (direction + 7) % 6;
      } else if (r < 2) {
        direction = (direction + 5) % 6;
      }
      facing = (int)(get_gradient_value(
          g0,
          0.5 + (double)current->y / (MAP_SIZE),
          0.5 + (double)current->x / (MAP_SIZE)
      ) + (get_gradient_value(
          g1,
          0.5 + (double)current->y / (MAP_SIZE * 3),
          0.5 + (double)current->x / (MAP_SIZE * 3)
      ) * 18.0)) % 6;
      if (abs((int)direction - (int)facing) > 1) {
        direction = facing;
      }
      for (int j = (rand() % 4) * (rand() % 2); j > 0; --j) {
        current = current->adj[(direction + 6) % 6];
        if (NULL == current) {
          break;
        }
      }
      if (NULL == current) {
        break;
      }
    }
  }
  destroy_gradient_generator(g0);
  destroy_gradient_generator(g1);
  destroy_gradient_generator(g2);
  destroy_voronoi_generator(vg0);
}

void GEO_GEN_HandleEdge() {
  GradientGenerator *g0 = new_gradient_generator(geo->seed + 8, 180, 180);
  GradientGenerator *g1 = new_gradient_generator(geo->seed + 9, 180, 180);
  for (int y = 0; y < MAP_SIZE; ++y) {
    for (int x = 0; x < MAP_SIZE; ++x) {
      Tile *tile = TILE(y, x);
      int o = MAP_SIZE / 2;
      double d = abs(y - o) > abs(x - o) ? abs(y - o) : abs (x - o) ;
      double dx = y & 1 ? 0.5 : 0.0;
      double dm = 0;
      dm += get_gradient_value(
          g0,
          30.2 + (double)y / 60.0,
          20.8 + ((double)x + dx) / 60.0
      ) * 0.9;
      dm += get_gradient_value(
          g1,
          11.2 + (double)y / 16.0,
          11.2 + ((double)x + dx) / 16.0
      ) * 0.1;
      dm *= MAP_SIZE / 2.5;
      if (d > dm) {
        tile->elevation += 3000;
        tile->elevation *= 1.0 - pow(((d - dm) * 3.0) / MAP_SIZE, 0.9);
        tile->elevation -= 3000;
        if (tile->elevation < -2900) {
          tile->elevation = -2900;
        }
      }
    }
  }
  destroy_gradient_generator(g0);
  destroy_gradient_generator(g1);
}

void GEO_GEN_AssignType() {
  for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
    Tile *tile = &geo->tiles[i];
    if (tile->elevation < 0) {
      tile->type = TILE_INLAND_SEA;
    } else {
      tile->type = TILE_LAND;
    }
  }
  TILE(0, 0)->type = TILE_OCEAN;
  bool change = false;
  do {
    change = false;
    for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
      Tile *tile = &geo->tiles[i];
      if (TILE_INLAND_SEA == tile->type) {
        for (int j = 0; j < 6; ++j) {
          if (tile->adj[j] && TILE_OCEAN == tile->adj[j]->type) {
            tile->type = TILE_OCEAN;
            change = true;
            break;
          }
        }
      }
    }
  } while (change);
}

void GEO_GEN_RemoveInlandSea() {
  for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
    Tile *tile = &geo->tiles[i];
    if (TILE_INLAND_SEA == tile->type) {
      tile->type = TILE_LAND;
      tile->elevation = 5;
    }
  }
}

void GEO_GEN_RemoveDepression(
    int lowest) {
  bool change = false;
  do {
    change = false;
    for (int y = 1; y < MAP_SIZE - 1; ++y) {
      for (int x = 1; x < MAP_SIZE - 1; ++x) {
        Tile *tile = TILE(y, x);
        int e = tile->elevation;
        if (e > lowest) {
          int emin = 10000;
          for (int i = 0; i < 6; ++i) {
            if (tile->adj[i]->elevation < emin) {
              emin = tile->adj[i]->elevation;
            }
          }
          if (emin >= e) {
            tile->elevation = emin + (rand() % 3) * (rand() % 4);
            change = true;
          }
        }
      }
    }
  } while (change);
}

void GEO_GEN_RouteFlow() {
  for (int y = 1; y < MAP_SIZE - 1; ++y) {
    for (int x = 1; x < MAP_SIZE - 1; ++x) {
      Tile *tile = TILE(y, x);
      Direction emin = W;
      for (int j = 0; j < 6; ++j) {
        if (tile->adj[j]->elevation < tile->adj[emin]->elevation) {
          emin = j;
        }
      }
      tile->down = emin;
    }
  }
}

void GEO_GEN_UpdateSlope() {
  for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
    Tile *tile = &geo->tiles[i];
    for (int j = 0; j < 6; ++j) {
      if (tile->adj[j]) {
        tile->slope += abs(tile->elevation - tile->adj[j]->elevation);
      }
    }
  }
}
