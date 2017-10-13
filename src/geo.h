#ifndef GEO_H
#define GEO_H

#define PI 3.1415926535897932384626

#define MAP_SIZE 320

#define TILE(y, x) GEO_GetTile(y, x)
#define TRIG(y, x) GEO_GetTrig(y, x)

#define TILE_SIZE 10000
#define MOUNTAIN_COUNT (MAP_SIZE / 120)
#define RIVER_TRY ((MAP_SIZE * MAP_SIZE) / 120)
#define LAKE_TRY ((MAP_SIZE * MAP_SIZE) / 200)
#define MAX_CITY_COUNT 40

#define RIVER_THRESHOLD 320

typedef enum Direction {
  W,
  NW,
  NE,
  E,
  SE,
  SW,
  N,
  S
} Direction;

typedef enum TileType {
  TILE_LAND,
  TILE_LAKE,
  TILE_OCEAN,
  TILE_INLAND_SEA
} TileType;

typedef enum VertexPosition {
  VERT_TIP,
  VERT_LEFT,
  VERT_RIGHT
} VertexPosition;

typedef struct Tile {
  int id;
  TileType type;
  int y;
  int x;
  int elevation;
  int slope;
  int humidity;
  int life;
  int city;
  int flow;
  Direction down;
  struct Tile *adj[6];
} Tile;

typedef struct Trig {
  int id;
  int y;
  int x;
  int elevation;
  int slope;
  int city;
  Tile *vertices[3];
  struct Trig *adj[8];
} Trig;

typedef struct Geo {
  long int seed;
  Tile *tiles;
  Trig *trigs;
  int *tile_distances;
  int *trig_distances;
} Geo;

extern Geo *geo;

void GEO_NewGeo(long int seed);
void GEO_DestroyGeo();

Tile *GEO_GetTile(int y, int x);
Trig *GEO_GetTrig(int y, int x);
int GEO_GetTileDistance(int y, int x);
void GEO_SetTileDistance(int y, int x, int d);
int GEO_GetTrigDistance(int y, int x);
void GEO_SetTrigDistance(int y, int x, int d);

#endif
