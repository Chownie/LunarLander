#ifndef GAME_H_
#define GAME_H_
#include "includes.h"

#define SHIP_MAX_SPEED 10 

typedef enum {
	INTRO,
	MAPBUILD,
	GAME,
	LOSE,
	WIN,
	WINBONUS,
} State;

typedef struct Ship {
	const  cchar_t visual;
	double x;
	double y;
	double xaccel;
	double yaccel;
	double xvel;
	double yvel;
	int    fuel;
} Ship;

typedef struct GameState {
	int     state;
	double  deltatime;
	Ship    *ship;
	cchar_t map[MAP_HEIGHT][MAP_WIDTH];
	int     width;
	int     height;
	int     ch;
	WINDOW  *gameScreen;
	WINDOW  *guiScreen;
	int     Points;
} GameState;

void InitializeMap(cchar_t (*Map)[MAP_HEIGHT][MAP_WIDTH], int width, int height);
int  EnactPhysics(GameState *st);
void Thrusters(Ship *sh, int vertical, int horizontal);
void DrawGameInfo(GameState *st);

#endif // GAME_H_
