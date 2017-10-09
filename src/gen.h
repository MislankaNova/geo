#ifndef GEN_H
#define GEN_H

#include "geo.h"

void GEO_GEN_InitialiseElevation();

void GEO_GEN_GenerateShield();

void GEO_GEN_GenerateMountain();

void GEO_GEN_HandleEdge();

void GEO_GEN_RemoveDepression(
    int lowest);

void GEO_GEN_AssignType();

void GEO_GEN_RemoveInlandSea();

void GEO_GEN_RouteFlow();

void GEO_GEN_UpdateSlope();

void GEO_GEN_CalculateHumidity();

void GEO_GEN_GenerateRivers(bool erode);

void GEO_GEN_GenerateLakes();

void GEO_GEN_CalculateLife();

void GEO_GEN_CalculateCity();

void GEO_GEN_PlaceCity();

#endif
