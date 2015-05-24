#include "includes.h" 
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>
#include <wchar.h>
#include <panel.h>

#include "terrain.h"
#include "game.h"

void DrawBorders(GameState *st) {
	wborder_set(st->gameScreen, &VERTICAL, &VERTICAL, &HORIZONTAL, &HORIZONTAL, &UL_CORNER, &T_UPRIGHT, &BL_CORNER, &BR_CORNER);
	wborder_set(st->guiScreen, &VERTICAL, &VERTICAL, &HORIZONTAL, &HORIZONTAL, &T_UPRIGHT, &UR_CORNER, &T_INVERSE, &BR_CORNER);	
}

// State gamestate to a default, allowing for map restarts without losing points.
void setState(GameState *st, Ship *sh) {
	sh->x = 2.0;
	sh->y = 2.0;
	sh->xvel = 1.8;
	sh->yvel = 2.0;
	sh->xaccel = 0.0;
	sh->yaccel = 10.0;
	sh->fuel = 1000;
	st->state = INTRO;
}

void GameFSM(GameState *st) {
	switch (st->state) {
		// State for the intro section, showing the welcome message.
		case INTRO: {
			mvwprintw(stdscr, 24, 20, "Lunar Lander");
			mvwprintw(stdscr, 25, 20, "Press an arrow key to start");
			wborder(stdscr, '|', '|', '-', '-', '+', '+', '+', '+');
			if(st->ch == KEY_DOWN || st->ch == KEY_UP || st->ch == KEY_LEFT || st->ch == KEY_RIGHT) {
				st->state = MAPBUILD;
						clear();
				}
				refresh();	
				break;
			}
		// State for building the map, connecting points.
		case MAPBUILD: {
			int points[21];
			for(int i = 0; i < 16; i++) {
				float input = (float)i*(RandFloat(0.0, 1.0)*0.1);
				points[i] = (int)floor(PerlinNoise(input, 1, 32));
			}
			DrawBorders(st);			
			
			int padc = 3;
			for(int i = 0; i < 16; i++) {
				padc = ConnectPoint(st, points[i], points[i+1], 5, i*5, padc);
			}

			wrefresh(st->gameScreen);	
			wrefresh(st->guiScreen);
			st->state = GAME;
			
			break;
		}
		// State containing game logic, checking inputs and physics.
		case GAME: {
			curs_set(0);

			for(int j = 0; j < st->height; j++) {
				for(int i = 0; i < st->width; i++) {
					mvwadd_wch(st->gameScreen, j, i, &st->map[j][i]);
				}
			}
			DrawBorders(st);

			int vertical = 0;
			int horizontal = 0;

			if(st->ch == KEY_DOWN) {
				vertical = 1;
			} else if (st->ch == KEY_UP) {
				vertical = -1;
			}

			if(st->ch == KEY_LEFT) {
				horizontal = -1;
			} else if (st->ch == KEY_RIGHT) {
				horizontal = 1;
			}

			if(horizontal != 0 || vertical != 0) {
				Thrusters(st->ship, vertical, horizontal);
			}

			int result = EnactPhysics(st);
			if(result == -1) {
				clear();
				st->state = LOSE;
			} else if(result == 1){
				clear();
				st->state = WIN;
			} else if (result == 2) {
				clear();
				st->state = WINBONUS;
			}

			mvwadd_wch(st->gameScreen, st->ship->y, st->ship->x, &st->ship->visual);
	
			DrawGameInfo(st);

			wrefresh(st->guiScreen);
			wrefresh(st->gameScreen);
			break;
		// Win state with bonus points for landing on a specific tile.
		case WINBONUS:
			curs_set(0);
			
			mvwprintw(st->gameScreen, 12, 12, "C O N G R A T U L A T I O N S !");
			mvwprintw(st->gameScreen, 14, 12, "You earned %d points", st->ship->fuel+200);
			mvwprintw(st->gameScreen, 16, 12, "Press any arrow key to play again");
			mvwprintw(st->gameScreen, 18, 12, "M O O N   B A S E   S U C C E S S !");
			if(st->ch == KEY_DOWN || st->ch == KEY_UP || st->ch == KEY_LEFT || st->ch == KEY_RIGHT) {
				setState(st, st->ship);
				st->state = INTRO;
				st->Points = st->ship->fuel+200;
				clear();
			}

			DrawGameInfo(st);
			wrefresh(st->gameScreen);
			wrefresh(st->guiScreen);
			break;
		// Normal win state for landing on any flat land under a certain speed.
		case WIN:
			curs_set(0);
			
			mvwprintw(st->gameScreen, 12, 12, "C O N G R A T U L A T I O N S !");
			mvwprintw(st->gameScreen, 14, 12, "You earned %d points", st->ship->fuel+100);
			mvwprintw(st->gameScreen, 16, 12, "Press any arrow key to play again");
			if(st->ch == KEY_DOWN || st->ch == KEY_UP || st->ch == KEY_LEFT || st->ch == KEY_RIGHT) {
				setState(st, st->ship);
				st->state = INTRO;
				st->Points = st->ship->fuel+100;
				clear();
			}

			DrawGameInfo(st);
			wrefresh(st->gameScreen);
			wrefresh(st->guiScreen);
			break;
		// Landing at too high a speed or landing on uneven terrain.
		case LOSE:
			curs_set(0);

			mvwprintw(st->gameScreen, 12, 12, "Y O U   L O S E");
			mvwprintw(st->gameScreen, 14, 12, "You left a crater %.1f metres wide.", (float)st->ship->fuel/10);
			mvwprintw(st->gameScreen, 15, 12, "You lose 20 points.");
			mvwprintw(st->gameScreen, 16, 12, "Press any arrow key to play again");
				
			if(st->ch == KEY_DOWN || st->ch == KEY_UP || st->ch == KEY_LEFT || st->ch == KEY_RIGHT) {
				setState(st, st->ship);
				st->state = INTRO;
				st->Points -= 20;	
				clear();
			}
			DrawGameInfo(st);					
			wrefresh(st->gameScreen);
			wrefresh(st->guiScreen);	
			break;
		}
	}
}


int main() {
	// Literally magic, not even the ncurses devs know what these things do anymore.
	setlocale(LC_ALL, ""); // For some reason emptying locale allows UTF-8 (?)
	set_altchars();        // Allows use of the keypad
	srand(time(NULL));
	initscr();             // Initialize the screen
	keypad(stdscr, TRUE);  // Double make sure we can use the keypad

	cbreak();              // These three lines ensure no garbage is printed to screen
	nonl();                // when inputs are used.
	noecho();

	nodelay(stdscr, TRUE); // Don't block when waiting for input
	start_color();		   // Enable colours

	init_pair(1, COLOR_WHITE, COLOR_GREEN);
	init_pair(2, COLOR_WHITE, COLOR_YELLOW);
	init_pair(3, COLOR_WHITE, COLOR_RED);	

	WINDOW *gui_scr = newwin(20, 30, 1, 81);
	WINDOW *game_scr = newwin(40, 80, 1, 1);

	leaveok(gui_scr, TRUE);
	leaveok(game_scr, TRUE);

	Ship ship  = { .visual = SHIP_SYMBOL, .x = 2.0, .y = 2.0, .xvel = 1.8, .yvel = 2, .xaccel = 0.0,
					.yaccel = 10.0, .fuel = 1000};

	GameState state = { .state = INTRO, .ship = &ship, .gameScreen = game_scr, .guiScreen = gui_scr, .width = 80, .height = 50, .deltatime = 0 };

	InitializeMap(&state.map, state.width, state.height);

	struct timespec elapsed;
	long previousTime = 0;
	for (;;) {
		clock_gettime(CLOCK_MONOTONIC, &elapsed);
		if(elapsed.tv_nsec > previousTime) {
			state.deltatime = (double)(elapsed.tv_nsec - previousTime)*1.0e-9;
			previousTime = elapsed.tv_nsec;
		}
		int c = getch();
		if(c==27) {
			break;
		}
		state.ch = c;
		GameFSM(&state);
		curs_set(0);
		usleep(1/60);	
	}

	endwin();
}
