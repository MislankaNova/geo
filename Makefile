VPATH  = src

CC     = gcc
CFLAGS = -xc -std=c11 -W -Wall -Wextra -Werror -pedantic -Iopen-simplex-noise-in-c

LDLIBS = -lm -lSDL2

.PHONY: all clean object

all: geo

geo: main.o view.o geo.o gen_elevation.o gen_climate.o gen_river.o gen_demography.o alg_path.o noise.o open-simplex-noise.o

view.o: view.h

geo.o: gen_elevation.o gen_climate.o gen_river.o gen_demography.o alg_path.o

gen_elevation.o: noise.o gen.h gen_elevation.c

gen_climate.o: gen.h gen_climate.c

gen_river.o: gen.h gen_river.c

gen_demography.o: gen.h gen_demography.c

alg_path.o: algorithm.h alg_path.c

noise.o: open-simplex-noise.o noise.h noise.c

open-simplex-noise.o: open-simplex-noise-in-c/open-simplex-noise.h open-simplex-noise-in-c/open-simplex-noise.c
	cd open-simplex-noise-in-c && make open-simplex-noise.o
	mv open-simplex-noise-in-c/open-simplex-noise.o open-simplex-noise.o

clean:
	rm -f *.o
	rm -f geo
