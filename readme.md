![Example Output](/example-000000005a63ca5c-smooth-elevation-2x.png)

# geo: A Simple Procedural Terrain Generator

*geo* is a procedural terrain generator written in C.
It is capable of generating realistic landmasses with features like plains, hills, mountain ranges, rivers, and cities.

## How to Get Around

Use arrow keys to move the map.
Use square bracket keys to zoom in and out.
Use Page Up and Page Down to change map display mode.
Press Spacebar to generate a new map.
Press S to save the current map image.

There are ten display modes:

- Elevation
- Smooth elevation
- Cultural Area
- Smooth Cultural Area
- Land Hardness
- Humidity
- Roughness of terrain
- Life rating
- City rating
- Cultural rating
- Distance from the currently selected tile (takes a lot of time to calculate)

## Map Representation

The generated terrain is reprensented by map tiles which form a hexagonal grid,
and a triangular grid whose nodes are the centre of tiles,
and whose edges corresponds to the adjacency of tiles.
Rivers flows along the edges of the triangular grid,
while cities are placed at the centre of the triangles.

The size of a map tile is 10,000 metres by 10,000 metres.

## Generation Steps

First, the base elevation is generated,
using a combination of multiple gradient noises.
Mountains ranges, as well as smaller mountains,
are then put on top of the base elevation.
The elevation is then modulated to better differentiate flat, low-lying plains,
from hilly highlands.
Another set of noises then outline the coastline,
turning the landmass into an ocean.
After that, all depressions or 'sinks' are removed,
so water could flow from anywhere on the land into the ocean.

Humidity of each tile is calculated using the Monte Carlo Method,
by tracking wet air which moves randomly through the map.
The water flux flowing down each tile is calculated,
headward erosion than creates valleys whose width vary with the amount of water flux.
The rate of erosion depends on the hardness of the land tile,
which is determined by the way in which the land is formed at the tile eroded.
Rivers form at places where the water flux is above a certain threshold.

Each tile will receive a life rating,
based on humidity, proximity to river, elevation, and roughness of terrain.
The city rating of a tile is determined by the sum of life ratings of tiles within a certain distance away.
Cities are founded on tiles with the greatest city rating on the map.
Once a city is founded, the city ratings of nearby tiles are decreased,
and more cities are placed based on the updated city ratings of tiles.

When a city is placed, it brings up the cultural rating of nearby tiles.
A culture is founded at the point of greatest cultural rating,
from where its influence spreads.
Like cities, when cultures are founded, they decrease the cultural rating of nearby tiles.

![Example of Area Placement](/example-000000005a63ca5c-area-2x.png)

## Acknowledgement

I was inspired by Martin O'Leary's article [*Generating fantasy maps*](https://mewo2.com/notes/terrain/),
which contains a lot of useful information about terrain generation.

*geo* uses smcameron's [open-simplex-noise-in-c](https://github.com/smcameron/open-simplex-noise-in-c).

