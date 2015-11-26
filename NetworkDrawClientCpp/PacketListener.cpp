#include <iostream>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML\Network\UdpSocket.hpp>
#include <SFML\Graphics.hpp>
#include "App.h"
#include "PacketListener.h"
#include "Packet.h"
#include "Heatmap.h"

bool PacketListener::Update()
{
	union
	{
		char data[10000];

		Packet basePacket;
		PacketBox packetBox;
		PacketLine packetLine;
		PacketCircle packetCircle;
		PacketClientCursor packetClientCursor;
		PacketClientAnnounce packetClientAnnounce;
		PacketServerInfo packetServerInfo;
		PacketServerCursors packetServerCursors;

	} UPacket;

	std::size_t received;

	sf::IpAddress sender;

	unsigned short listenPort = App::Instance().GetListenPort();

	sf::Socket::Status result = App::Instance().GetSocket()->receive(UPacket.data, 100, received, sender, listenPort);

	if (result == sf::Socket::Done)
	{
		switch (UPacket.basePacket.type)
		{
			//This appears to be a server response. Set our window size to the values specified by the server, and set our target address to the origin of this packet.
			//If we are already connected, ignore this packet.
			case Packet::e_serverInfo:
			{
				if (!App::Instance().GetConnected())
				{
					//Set our window size.
					App::Instance().GetWindow()->setSize(sf::Vector2u(UPacket.packetServerInfo.width, UPacket.packetServerInfo.height));

					//Set our heatmap size
					App::Instance().GetHeatmap()->UpdateWindowSize(UPacket.packetServerInfo.width, UPacket.packetServerInfo.height);

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
					for (auto cursor : App::Instance().mCursors)
					{
						DELETE_NULLIFY(cursor);
					}

					App::Instance().mCursors.clear();

					for (CursorInfo cursor : UPacket.packetServerCursors.cursor)
					{
						sf::RectangleShape* cursorShape = new sf::RectangleShape(sf::Vector2f(3.0f, 5.0f));

						cursorShape->setPosition(cursor.m_posX, cursor.m_posY);

						sf::Color randCol = sf::Color(std::rand() % 255, std::rand() % 255, std::rand() % 255);
						cursorShape->setFillColor(randCol);
						cursorShape->setOutlineColor(sf::Color::White);
						cursorShape->setOutlineThickness(1.0f);

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
