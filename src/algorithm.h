#ifndef ALG_H
#define ALG_H

typedef struct DistanceParameters {
  int any;
  int ocean;
  int river;
  int land;
  double slope_factor;
} DistanceParameters;

extern const DistanceParameters DEFAULT_DISTANCE_PARAMETERS;

void GEO_ALG_CalculateTileDistance(
    const DistanceParameters *parameters,
    Tile *tile,
    int limit,
    int (*dest)[MAP_SIZE * MAP_SIZE]
);

#endif
