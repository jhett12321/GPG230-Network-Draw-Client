#include "PacketListener.h"
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Socket.hpp>
#include "Common.h"
#include <iostream>
#include "Packet.h"

void PacketListener::Update()
{
	char data[100];

	std::size_t received;

	sf::IpAddress* sender = new sf::IpAddress();

	sf::Socket::Status result = Common::socket->receive(data, 100, received, *sender, Common::listenPort);

	if (result == sf::Socket::Done)
	{
		//TODO read data and verify it is a correct packet.
		Packet* rawPacket = (Packet*)&data;

		switch (rawPacket->type)
		{
			//This appears to be a server response. Set our window size to the values specified by the server, and set our target address to the origin of this packet.
			//If we are already connected, ignore this packet.
			case Packet::e_serverInfo:
			{
				if (!Common::connected)
				{
					PacketServerInfo* serverInfo = (PacketServerInfo*)&data;

					Common::window->setSize(sf::Vector2u(serverInfo->width, serverInfo->height));
					Common::recipient = sender;

					//We are now connected.
					Common::connected = true;
				}

				break;
			}

			//This is a packet containing other client cursors.
			case Packet::e_serverCursors:
			{
				if (Common::connected)
				{
					PacketServerCursors* serverCursors = (PacketServerCursors*)&data;

					Common::cursors.clear();

					for (sf::RectangleShape* cursor : Common::cursors)
					{
						delete cursor;
					}

					for (CursorInfo cursor : serverCursors->cursor)
					{
						sf::RectangleShape* cursorShape = new sf::RectangleShape(sf::Vector2f(5.0f, 5.0f));

						cursorShape->setPosition(cursor.m_posX, cursor.m_posY);

						Common::cursors.push_back(cursorShape);
					}
				}

				break;
			}

			//We received a packet type that we can't understand.
			default:
			{
				return;
			}
		}
	}

	else if (result != sf::Socket::Done && result != sf::Socket::NotReady)
	{
		std::cout << "Error receiving data" << std::endl;
	}
}
