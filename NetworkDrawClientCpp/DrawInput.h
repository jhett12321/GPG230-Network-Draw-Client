#ifndef DRAWINPUT_H
#define DRAWINPUT_H

#include <queue>
#include "SFML\Graphics.hpp"

enum Mode
{
	PIXEL,
	BOX,
	LINE,
	CIRCLE
};

class DrawInput
{
public:
	void Update(sf::Event event);
	void CheckMouseMoved();

	bool clicked = false;
	bool mouseMoved = false;

	sf::Vector2i* startPos = new sf::Vector2i(0, 0);;

	Mode mode = PIXEL;
};

#endif
