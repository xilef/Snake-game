/********************************************************/
/* Snake class function definitions						*/
/********************************************************/

#include "stdafx.h"
#include "snake_class.h"

/********************************************************/
/* Constructor:											*/
/* Sets a starting point and direction					*/
/********************************************************/
Snake::Snake (int xpos, int ypos, int len, int size, int dir) {
	int ctr;

	setPosition (NULL);

	for (ctr = 0; ctr < len; ctr++) {
		this->addHead (xpos + (ctr * size), ypos);
	}

	this->setDirection (dir);
}

void Snake::setPosition (position *newPos) {
	pos = newPos;
}

void Snake::setDirection (int dir) {
	direction = dir;
}

/********************************************************/
/* Search for the last node, eg: next pointer is NULL	*/
/* while keeping track of the previous node				*/
/********************************************************/
void Snake::removeTail() {
	position *currPos, *temp;

	currPos = getPosition ();
	while (currPos->next != NULL) {
		temp = currPos;
		currPos = currPos->next;
	}
	free (temp->next);
	temp->next = NULL;
}

/********************************************************/
/* Add a node at the start and set the next pointer		*/
/* to point to the current head node					*/
/* If first then set to NULL the previous pointer		*/
/********************************************************/
void Snake::addHead (int xpos, int ypos) {
	position *newHeadPos, *currHeadPos;

	/* Create new head node */
	newHeadPos = new position;

	newHeadPos->xpos = xpos;
	newHeadPos->ypos = ypos;
	newHeadPos->prev = NULL;

	currHeadPos = this->getPosition();
	newHeadPos->next = currHeadPos;
	if (currHeadPos != NULL) {
		currHeadPos->prev = newHeadPos;
	}

	this->setPosition (newHeadPos);
}

void Snake::moveOnwards (int xpos, int ypos) {
	this->addHead (xpos, ypos);
	this->removeTail ();
}

/********************************************************/
/* Checks the passed coordinate if it is not occupied	*/
/* by the snake's body									*/
/********************************************************/
bool Snake::isCoordAvailable (int xpos, int ypos) {
	position *currPos;

	currPos = getPosition ();
	while (currPos->next != NULL) {
		if (currPos->xpos == xpos && currPos->ypos == ypos) {
			return false;
		}
		currPos = currPos->next;
	}
	return true;
}