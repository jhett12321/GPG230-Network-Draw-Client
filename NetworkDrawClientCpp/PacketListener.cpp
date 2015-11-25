#include <iostream>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML\Network\UdpSocket.hpp>
#include <SFML\Graphics.hpp>
#include "App.h"
#include "PacketListener.h"
#include "Packet.h"

bool PacketListener::Update()
{
	char data[100];

	std::size_t received;

	sf::IpAddress sender;

	unsigned short listenPort = App::Instance().GetListenPort();

	sf::Socket::Status result = App::Instance().GetSocket()->receive(data, 100, received, sender, listenPort);

	if (result == sf::Socket::Done)
	{
		Packet* rawPacket = (Packet*)&data;

		switch (rawPacket->type)
		{
			//This appears to be a server response. Set our window size to the values specified by the server, and set our target address to the origin of this packet.
			//If we are already connected, ignore this packet.
			case Packet::e_serverInfo:
			{
				if (!App::Instance().GetConnected())
				{
					PacketServerInfo* serverInfo = (PacketServerInfo*)&data;

					App::Instance().GetWindow()->setSize(sf::Vector2u(serverInfo->width, serverInfo->height));

					if (App::Instance().GetRecipient() == nullptr)
					{
						App::Instance().SetRecipient(new sf::IpAddress(sender));
					}

					else
					{
						sf::IpAddress* oldAddr = App::Instance().GetRecipient();
						DELETE_NULLIFY(oldAddr);
						App::Instance().SetRecipient(new sf::IpAddress(sender));
					}

					//We are now connected.
					App::Instance().SetConnected(true);
				}

				break;
			}

			//This is a packet containing other client cursors.
			case Packet::e_serverCursors:
			{
				if (App::Instance().GetConnected())
				{
					PacketServerCursors* serverCursors = (PacketServerCursors*)&data;

					for (auto cursor : App::Instance().mCursors)
					{
						DELETE_NULLIFY(cursor);
					}

					App::Instance().mCursors.clear();

					for (CursorInfo cursor : serverCursors->cursor)
					{
						sf::RectangleShape* cursorShape = new sf::RectangleShape(sf::Vector2f(5.0f, 5.0f));

						cursorShape->setPosition(cursor.m_posX, cursor.m_posY);

						App::Instance().mCursors.push_back(cursorShape);
					}
				}

				break;
			}
		}

		return true;
	}

	else if (result != sf::Socket::Done && result != sf::Socket::NotReady)
	{
		std::cout << "Error receiving data" << std::endl;
	}

	return false;
}
