#ifndef TERRAIN_H_
#define TERRAIN_H_
#include "game.h"
float RandFloat(float a, float b);
float PerlinNoise(float x, float persistence, int octaves);
int ConnectPoint(GameState *st, int start, int end, int length, int x, int padc);

#endif // TERRAIN_H_

