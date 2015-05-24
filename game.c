#include "includes.h"
#include <stdlib.h>
#include <math.h>
#include <wchar.h>
#include "game.h"


// Initializes all of the cchar_ts on the map to BLANK
void InitializeMap(cchar_t (*Map)[MAP_HEIGHT][MAP_WIDTH], int width, int height) {
	for(int i=0; i < height; i++) {
		for(int j=0; j < width; j++){
			(*Map)[i][j] = BLANK; 
		}
	}
}

/* Returns results of collisions.
 * 2 is a successful landing on a bonus point
 * 1 is a successful landing on solid ground
 * 0 is no collision
 * -1 is a collision at too high force or on uneven ground, ends with a game over.
 */
int CollisionCheck(Ship *sh, cchar_t (*map)[MAP_HEIGHT][MAP_WIDTH], double newY, double newX) {
	cchar_t ch = (*map)[(int)round(newY)][(int)round(newX)];
	
	if(wcscmp(ch.chars, BLANK.chars) == 0) {
		return 0;
	}
	
	if(wcscmp(ch.chars, BLANK.chars) != 0 && sh->yvel > 2) {
		return -1;	
	}
	
	if(wcscmp(ch.chars, BLANK.chars) != 0 && sh->yvel <= 2) {
		if(wcscmp(ch.chars, LANDING_PAD.chars) == 0) {
			return 2;
		} else if (wcscmp(ch.chars, HORIZONTAL.chars) == 0) {
			return 1;
		} else {
			return -1;
		}
	}
	return 0;
} 

/*
 * Handles physics, controlling maximum speed, velocity and acceleration
 * plus acceleration drop-off over time.
*/
int EnactPhysics(GameState *st){
	st->ship->yvel += (1 + GRAVITY + st->ship->yaccel) * st->deltatime;
	st->ship->xvel += (1 + st->ship->xaccel) * st->deltatime;

	st->ship->yaccel *= 0.9;
	st->ship->xaccel *= 0.9;

	if(st->ship->yvel > SHIP_MAX_SPEED) {
		st->ship->yvel = SHIP_MAX_SPEED;
	} else if(st->ship->yvel < -SHIP_MAX_SPEED) {
		st->ship->yvel = -SHIP_MAX_SPEED;
	}

	double newY = st->ship->y + (st->ship->yvel * st->deltatime);
	double newX = st->ship->x + (st->ship->xvel * st->deltatime);

	int result = CollisionCheck(st->ship, &st->map, newY, newX);
	if(result != 0) {
		return result;
	}

	st->ship->y = newY;
	st->ship->x = newX;

	return 0;
}

/*
 * Handles thrusters on the ship in any relative direction.
 * 
 * Horizontal and vertical expect ints in the range -1 to 1
 * to control the state of thrusters in those directions.
 * -1 indicates a thruster in "reverse".
 */
void Thrusters(Ship *sh, int vertical, int horizontal) {
	if(sh->fuel > 0) { 
		sh->yaccel += (vertical*20);
		sh->xaccel += (horizontal*20);
		if(vertical != 0 && horizontal != 0) {
			sh->fuel -= 2;
		} else {
			sh->fuel -= 1;
		}
	}
}

/*
 * Draws the GUI for the game into the GUI panel on the right hand side of the window.
 * Due to ncurses not clearing upon refreshes, writing blank spaces to some areas
 * is needed.
 */
void DrawGameInfo(GameState *st){
	attron(COLOR_PAIR(0));
	mvwprintw(st->guiScreen, 2, 2, "                    ");
	mvwprintw(st->guiScreen, 2, 2, "Y Acceleration: %.2f m/s2", st->ship->yaccel);
	mvwprintw(st->guiScreen, 3, 2, "                    ");
	mvwprintw(st->guiScreen, 3, 2, "Y Velocity: %.2f m/s2", st->ship->yvel);
	mvwprintw(st->guiScreen, 4, 2, "                    ");
	mvwprintw(st->guiScreen, 4, 2, "X Acceleration: %.2f m/s2", st->ship->xaccel);
	mvwprintw(st->guiScreen, 5, 2, "                    ");
	mvwprintw(st->guiScreen, 5, 2, "X Velocity: %.2f m/s2", st->ship->xvel);
	mvwprintw(st->guiScreen, 6, 2, "            ");
	mvwprintw(st->guiScreen, 6, 2, "Fuel: %d", st->ship->fuel);
	mvwprintw(st->guiScreen, 7, 2, "                    ");
	mvwprintw(st->guiScreen, 7, 2, "[");
	if(st->ship->fuel < 250) {
		attron(COLOR_PAIR(3));
	} else if (st->ship->fuel < 500) {
		attron(COLOR_PAIR(2));
	} else {
		attron(COLOR_PAIR(1));
	}
	for(int i = 0; i < (int)floor(st->ship->fuel/50); i++) {
		mvwaddch(st->guiScreen, 7, 3+i, '=');
	}
	attron(COLOR_PAIR(0));
	mvwprintw(st->guiScreen, 7, 22, "]");
	mvwprintw(st->guiScreen, 8, 2, "Score: %d", st->Points);
}
