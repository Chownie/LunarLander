#include "includes.h"
#include "game.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "terrain.h"

/*
 * The entire set of functions below are accompany functions for
 * performing the generation of 1D perlin noise.
 */
int RandInt(int min, int max) {
	return rand() % (max - min+1) + min;
}

float RandFloat(float a, float b) {
	float random = ((float) rand()) / (float) RAND_MAX;
	float diff = b - a;
	float r = random*diff;
	return a + r;
}

/* Integer noise function.
 *
 * Takes an integer and returns a somewhat uniform value between -1.0 and 1.0.
 * The large integers/floats are sufficiently large prime numbers used for 
 * generating the noise.
*/
float IntNoise(int x) {
	int MAX_INT = (1<<31)-1;
	x = ((x<<13)&MAX_INT)^x;
	x = (x * (x*x*15731 + 789221) + 1376312589 ) & MAX_INT;
	return 1.0 - x / 1073741824.0;
}

float Smooth1DNoise(float x) {
	return IntNoise(x)/2 + IntNoise(x-1)/4 + IntNoise(x+1)/4;
}

float Interpolate(float a, float b, int x) {
	float ft = x * M_PI;
	float f = (1 - cos(ft)) * 0.5;

	return a*(1-f) + b*f;
}

float InterpolateNoise(float x) {
	int int_x = (int)x;
	float frac_x = x - int_x;
	
	float v1 = Smooth1DNoise(int_x);
	float v2 = Smooth1DNoise(int_x+1);

	return Interpolate(v1, v2, frac_x);
}

float PerlinNoise(float x, float persistence, int octaves) {
	float total = 0.0;
	for(int i = 0; i < octaves-1; i++) {
		float frequency = pow(2, i);
		float amplitude = pow(persistence, i);

		total = total + InterpolateNoise(x * frequency) * amplitude;
	}
	return total;
}


/* Connects points by approaching the next point by height.
 * The first 3 flat areas are given a landing spot with bonus points which
 * encourages players to use fuel to resist the rightward acceleration
 * given by default.
 */
int ConnectPoint(GameState *st, int start, int end, int length, int x, int pads) {
	int height = start;
	int oldPads = pads;
	for(int i = 0; i < length; i++) {
		cchar_t ch;
		if(height < end) {
			ch = UR_CORNER;
			height++;
		} else if(height > end) {
			ch = UL_CORNER;
			height--;
		} else {
			if(oldPads == pads && pads != 0) {
				ch = LANDING_PAD;
				pads--;
				st->map[31+height][x+i] = BONUS_POINT;	
			} else {
				ch = HORIZONTAL;
			}
		}
		st->map[30+height][x+i] = ch;	
	}
	return pads;
}
