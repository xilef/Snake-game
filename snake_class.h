/********************************************************/
/* Snake class header									*/
/********************************************************/
#pragma once

#include <list>

using std::list;

enum direction {UP = 72, LEFT = 75, RIGHT = 77, DOWN = 80};

struct position {
	unsigned char xpos;
	unsigned char ypos;

	bool operator== (const position rhs) const {
		if (xpos == rhs.xpos && ypos == rhs.ypos)
			return true;
		return false;
	}
};

class Snake {
private:
	direction dir;
	list <position> pos;

public:
	Snake ();
	void Init (position hpos, const unsigned char len, const unsigned char size, direction d);
	void removeTail ();
	void addHead (const position newHead);
	void moveOnwards (const position newHead);
	
	inline void setDirection (direction d) { dir = d; };

	inline list <position> getPosition () const { return (pos); };
	inline direction getDirection () const { return (dir); };

	bool isCoordAvailable (const position currPos) const;
};