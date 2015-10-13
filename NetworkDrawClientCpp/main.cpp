#include "Common.h"
#include "Packet.h"
#include <iostream>

int main()
{
	// Try and create our socket/draw window.
	if(!Common::Init())
	{
		Common::window->close();
		return 0;
	}

	//Common timer. Used for the initial handshake, and queueing packets.
	sf::Clock timer;

	float sendInterval = 0.05f;					//How often to bundle and send our queued packets.
	float initialAnnounceInterval = 2.0f;		//How often to attempt an initial handshake.
	
	//Create our initial "announce" packet. When we receive a response, we can process our queue.
	//This is sent in our main loop.
	PacketClientAnnounce* clientAnnounce = new PacketClientAnnounce();
	clientAnnounce->type = Packet::e_clientAnnounce;

	while (Common::window->isOpen())
	{
		//We are connected and can now send shape & cursor data.
		if (Common::connected)
		{
			sf::Event event;
			while (Common::window->pollEvent(event))
			{
				//Do Draw/Input Logic
				Common::drawInput->Update(event);
			}

			//Process any current waiting packets. Also send a "cursor move" packet if the mouse moved.
			if (timer.getElapsedTime().asSeconds() >= sendInterval)
			{
				Common::drawInput->CheckMouseMoved();
				Common::packetSender->ProcessQueue();

				timer.restart();
			}
		}

		//Broadcast our announce packet if we are not connected. 
		else
		{
			if (timer.getElapsedTime().asSeconds() >= initialAnnounceInterval)
			{
				Common::packetSender->SendImmediate((char*)clientAnnounce, sizeof(*clientAnnounce));

				timer.restart();
			}
		}

		//Check to see if we received any new packets.
		Common::packetListener->Update();

		//Render our cursors
		for (sf::RectangleShape* cursor : Common::cursors)
		{
			Common::window->draw(*cursor);
		}

		//Render our window.
		Common::window->display();
	}

	return 0;
}