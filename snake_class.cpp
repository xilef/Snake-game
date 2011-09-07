/********************************************************/
/* Snake class function definitions						*/
/********************************************************/

#include "stdafx.h"
#include "snake_class.h"
#include <list>

using std::list;

Snake::Snake () {
}

/********************************************************/
/* Init function:										*/
/* Sets a starting point and direction					*/
/********************************************************/
void Snake::Init (position hpos, const unsigned char len, const unsigned char size, direction d) {
	int ctr;

	if (!pos.empty ())
		pos.clear ();

	for (ctr = 0; ctr < len; ctr++) {
		addHead (hpos);
		hpos.xpos += size;
	}

	setDirection (d);
}

/********************************************************/
/* Remove last coordinate								*/
/********************************************************/
void Snake::removeTail() {
	pos.pop_back ();
}

/********************************************************/
/* Add the current head coordinate						*/
/********************************************************/
void Snake::addHead (const position newHead) {
	pos.push_front (newHead);
}

void Snake::moveOnwards (const position newHead) {
	addHead (newHead);
	removeTail ();
}

/********************************************************/
/* Checks the passed coordinate if it is not occupied	*/
/* by the snake's body									*/
/********************************************************/
bool Snake::isCoordAvailable (const position currPos) const {
	list <position>::const_iterator bodyPos;

	for (bodyPos = pos.begin (); bodyPos != pos.end (); bodyPos++) {
		if (currPos == *bodyPos)
			return (false);
	}
	return true;
}