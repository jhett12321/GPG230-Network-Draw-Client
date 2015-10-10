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

	//Common objects are ready. Initialize send timers, and initial packets.
	sf::Clock timer;
	float sendInterval = 0.05f;

	//Send our initial "announce" packet. When we receive a response, we can process our queue.
	PacketClientAnnounce clientAnnounce;
	clientAnnounce.type = Packet::e_clientAnnounce;

	Common::packetSender->SendImmediate((char*)&clientAnnounce, sizeof(clientAnnounce));

	while (Common::window->isOpen())
	{
		//We are connected and can now send shape & cursor data.
		if (Common::connected)
		{
			//Process any current waiting packets. Also send a "cursor move" packet if the mouse moved.
			if (timer.getElapsedTime().asSeconds() >= sendInterval)
			{
				Common::drawInput->CheckMouseMoved();
				Common::packetSender->ProcessQueue();

				timer.restart();
			}

			sf::Event event;
			while (Common::window->pollEvent(event))
			{
				//Do Draw/Input Logic
				Common::drawInput->Update(event);
			}
		}

		//Check to see if we received any new packets.
		Common::packetListener->Update();

		//Render our window.
		Common::window->display();
	}

	return 0;
}