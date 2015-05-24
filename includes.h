#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>

#define MAP_HEIGHT 80
#define MAP_WIDTH 80
#define GRAVITY 3.711 

extern cchar_t VERTICAL;
extern cchar_t HORIZONTAL;
extern cchar_t UL_CORNER;
extern cchar_t UR_CORNER;
extern cchar_t BL_CORNER;
extern cchar_t BR_CORNER;
extern cchar_t T_UPRIGHT;
extern cchar_t T_INVERSE;
extern cchar_t T_LEFT;
extern cchar_t T_RIGHT;
extern cchar_t BLANK;
extern cchar_t SHIP_SYMBOL;
extern cchar_t LANDING_PAD;
extern cchar_t BONUS_POINT;
void set_altchars();

#endif // _INCLUDES_H_
