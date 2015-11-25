#ifndef DRAWINPUT_H
#define DRAWINPUT_H

#include <SFML\System\Vector2.hpp>
#include "Macros.h"

namespace sf
{
	class Event;
}

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

protected:
	RO_DATA_PROPERTY(bool, Clicked);
	RO_DATA_PROPERTY(bool, MouseMoved);
	RO_DATA_PROPERTY(Mode, Mode);

	RO_PTR_PROPERTY(sf::Vector2i, StartPos);
	RO_PTR_PROPERTY(sf::Vector2i, MousePos);

	DrawInput();
};

#endif