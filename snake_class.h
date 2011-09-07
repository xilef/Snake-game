/********************************************************/
/* Snake class header									*/
/********************************************************/

#ifndef SNAKE_H
#define SNAKE_H
struct position {
	int xpos;
	int ypos;
	position *next;
	position *prev;
};

class Snake {
private:
	int direction;
	position *pos;

public:
	Snake (int xpos, int ypos, int len, int size, int dir);
	void setDirection (int dir);
	void setPosition (position *newPos);
	void removeTail ();
	void addHead (int xpos, int ypos);
	void moveOnwards (int xpos, int ypos);
	int getDirection () { return direction; };
	position *getPosition () { return pos; };
	bool isCoordAvailable (int xpos, int ypos);
};
#endif