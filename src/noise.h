#ifndef NOISE_H
#define NOISE_H

#include "open-simplex-noise.h"

typedef struct GradientGenerator {
  struct osn_context *osng;
} GradientGenerator;

typedef struct VoronoiGenerator {
  size_t height;
  size_t width;
  size_t count;
  size_t *_ys;
  size_t *_xs;
  double min;
} VoronoiGenerator;

GradientGenerator *new_gradient_generator(
    long int seed,
    size_t height,
    size_t width
);
void destroy_gradient_generator(GradientGenerator *generator);

VoronoiGenerator *new_voronoi_generator(
    size_t count,
    size_t height,
    size_t width
);
void destroy_voronoi_generator(VoronoiGenerator *generator);

double get_gradient_value(GradientGenerator *generator, double y, double x);
double get_voronoi_value(VoronoiGenerator *generator, double y, double x);

#endif
