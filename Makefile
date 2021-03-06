VPATH  = src

CC     = gcc
CFLAGS = -xc -std=c11 -W -Wall -Wextra -Werror -pedantic -Iopen-simplex-noise-in-c

LDLIBS = -lm -lSDL2 -lpthread

.PHONY: all clean object

all: geo

geo: main.o view.o geo.o alg_path.o
	$(CC) *.o $(CFLAGS) $(LDLIBS) -o geo

view.o: geo.h view.h geo.o

geo.o: geo.h gen_elevation.o gen_climate.o gen_river.o gen_demography.o

gen_elevation.o: geo.h noise.o gen.h gen_elevation.c

gen_climate.o: geo.h gen.h gen_climate.c

gen_river.o: geo.h gen.h gen_river.c

gen_demography.o: geo.h gen.h gen_demography.c alg_path.o

alg_path.o: geo.h algorithm.h alg_path.c

noise.o: open-simplex-noise.o noise.h noise.c

open-simplex-noise.o: open-simplex-noise-in-c/open-simplex-noise.h open-simplex-noise-in-c/open-simplex-noise.c
	cd open-simplex-noise-in-c && make open-simplex-noise.o
	mv open-simplex-noise-in-c/open-simplex-noise.o open-simplex-noise.o

clean:
	rm -f *.o
	rm -f geo
