#include <SFML\Graphics.hpp>
#include <iostream>
#include "App.h"
#include "Packet.h"

int main()
{
	// Try and create our socket/draw window.
	if (!App::Instance().Init())
	{
		App::Instance().GetWindow()->close();
		return 0;
	}

	App::Instance().Run();

	return 0;
}