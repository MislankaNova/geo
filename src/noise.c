#include <stdlib.h>
#include <math.h>

#include "noise.h"

double distance(double y0, double x0, double y1, double x1) {
  return pow(pow(y1 - y0, 2.0) + pow(x1 - x0, 2.0), 0.5);
}

GradientGenerator *new_gradient_generator(
    long int seed,
    size_t height,
    size_t width) {
  // These two parameters are currently unused
  // However it might be used if the implementation of
  //   the gradient noise generator is changed
  (void)height;
  (void)width;
  GradientGenerator *generator = malloc(sizeof(GradientGenerator));
  open_simplex_noise(seed, &generator->osng);
  return generator;
}

void destroy_gradient_generator(GradientGenerator *generator) {
  open_simplex_noise_free(generator->osng);
  free(generator);
}

VoronoiGenerator *new_voronoi_generator(
    size_t count,
    size_t height,
    size_t width) {
  VoronoiGenerator *generator = malloc(sizeof(VoronoiGenerator));
  generator->count = count;
  generator->_ys = calloc(count, sizeof(size_t));
  generator->_xs = calloc(count, sizeof(size_t));
  generator->height = height;
  generator->width = width;
  for (size_t i = 0; i < count; ++i) {
    generator->_ys[i] = rand() % height;
    generator->_xs[i] = rand() % width;
  }
  generator->min = -1.0;
  for (size_t y = 0; y < height; ++y) {
    for (size_t x = 0; x < width; ++x) {
      double v = get_voronoi_value(generator, y, x);
      if (v > generator->min) {
        generator->min = v;
      }
    }
  }
  return generator;
}

void destroy_voronoi_generator(VoronoiGenerator *generator) {
  free(generator->_ys);
  free(generator->_xs);
  free(generator);
}

double get_gradient_value(GradientGenerator *generator, double y, double x) {
  return open_simplex_noise2(generator->osng, y, x) / 2.0 + 0.5;
}

double get_voronoi_value(VoronoiGenerator *generator, double y, double x) {
  double min = fmin(
      fmin(y, generator->height - y),
      fmin(x, generator->width - x)
  );
  min = generator->width + generator->height;
  double min2 = min;
  for (size_t i = 0; i < generator->count; ++i) {
    double d = distance(y, x, generator->_ys[i], generator->_xs[i]);
    if (d < min) {
      min = d;
    } if (d < min2 && d >= min) {
      min2 = d;
    }
  }
  return min;
}
