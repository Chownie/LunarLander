#include "includes.h"

cchar_t VERTICAL;
cchar_t HORIZONTAL;
cchar_t UL_CORNER;
cchar_t UR_CORNER;
cchar_t BL_CORNER;
cchar_t BR_CORNER;
cchar_t T_UPRIGHT;
cchar_t T_INVERSE;
cchar_t T_LEFT;
cchar_t T_RIGHT;
cchar_t BLANK;
cchar_t SHIP_SYMBOL;
cchar_t LANDING_PAD;
cchar_t BONUS_POINT;

// Initialize the characters with attributes to allow usage.
void set_altchars() {
	setcchar(&VERTICAL,   L"│", WA_NORMAL, 4, NULL);
	setcchar(&HORIZONTAL, L"─", WA_NORMAL, 4, NULL);
	setcchar(&UL_CORNER,  L"┌", WA_NORMAL, 4, NULL);
	setcchar(&UR_CORNER,  L"┐", WA_NORMAL, 4, NULL);
	setcchar(&BL_CORNER,  L"└", WA_NORMAL, 4, NULL);
	setcchar(&BR_CORNER,  L"┘", WA_NORMAL, 4, NULL);
	setcchar(&T_UPRIGHT,  L"┬", WA_NORMAL, 4, NULL);
	setcchar(&T_INVERSE,  L"┴", WA_NORMAL, 4, NULL);
	setcchar(&T_LEFT,     L"┤", WA_NORMAL, 4, NULL);
	setcchar(&T_RIGHT,    L"├", WA_NORMAL, 4, NULL);
	setcchar(&BLANK,      L" ", WA_NORMAL, 4, NULL);
	setcchar(&SHIP_SYMBOL,L"¤", WA_NORMAL, 4, NULL);
	setcchar(&LANDING_PAD,L"═", WA_NORMAL, 4, NULL);
	setcchar(&BONUS_POINT,L"2", WA_NORMAL, 4, NULL);
}
