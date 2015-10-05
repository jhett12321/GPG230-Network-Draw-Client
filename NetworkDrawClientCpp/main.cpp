#include <SFML/Graphics.hpp>
#include "Packet.h"
#include "SendManager.h"
#include "main.h"
#include <iostream>

bool connected = false;
unsigned short port = 1360;
unsigned short listenPort = 1360; //Default. Should be changed during binding.

enum Mode
{
	PIXEL,
	BOX,
	LINE,
	CIRCLE
};


void DrawLogic(sf::Vector2i startPos, Mode mode, bool clicked, sf::RenderWindow window, SendManager sendManager)
{
	if (!clicked)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1))
		{
			mode = PIXEL;
		}

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2))
		{
			mode = BOX;
		}

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3))
		{
			mode = LINE;
		}

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4))
		{
			mode = CIRCLE;
		}
	}

	if (!clicked && sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		clicked = true;

		startPos = sf::Mouse::getPosition(window);
	}

	switch (mode)
	{
		case PIXEL:
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				PacketPixel pixel;
				pixel.type = Packet::e_pixel;
				sf::Vector2i pos = sf::Mouse::getPosition(window);

				pixel.x = pos.x;
				pixel.y = pos.y;

				pixel.r = 1.0f;
				pixel.g = 0.0f;
				pixel.b = 0.5f;

				sendManager.AddPacketData((char*)&pixel, sizeof(pixel));
				delete &pixel;
			}

			else
			{
				clicked = false;
			}

			break;
		}
		case BOX:
		{
			if (clicked && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				PacketBox box;

				box.type = Packet::e_box;

				sf::Vector2i currentPos = sf::Mouse::getPosition(window);

				if (startPos.x < currentPos.x)
				{
					box.x = startPos.x;
					box.w = currentPos.x - startPos.x;
				}

				else
				{
					box.x = currentPos.x;
					box.w = startPos.x - currentPos.x;
				}

				if (startPos.y < currentPos.y)
				{
					box.y = startPos.y;
					box.h = currentPos.y - startPos.y;
				}

				else
				{
					box.y = currentPos.y;
					box.h = startPos.y - currentPos.y;
				}

				box.r = 1.0f;
				box.g = 0.0f;
				box.b = 1.0f;

				sendManager.AddPacketData((char*)&box, sizeof(box));

				clicked = false;
				delete &box;
			}

			break;
		}
		case LINE:
		{
			if (clicked && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				PacketLine line;

				line.type = Packet::e_line;

				line.x1 = startPos.x;
				line.y1 = startPos.y;

				line.x2 = sf::Mouse::getPosition(window).x;
				line.y2 = sf::Mouse::getPosition(window).y;

				line.r = 1.0f;
				line.g = 0.0f;
				line.b = 0.0f;

				sendManager.AddPacketData((char*)&line, sizeof(line));

				clicked = false;
			}

			break;
		}
		case CIRCLE:
		{
			if (clicked && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				PacketCircle circle;

				circle.type = Packet::e_circle;

				circle.x = startPos.x;
				circle.y = startPos.y;

				circle.radius = 20; //TODO change to mouse pos.

				//circle.w = .x - startPos.x;
				//circle.h = sf::Mouse::getPosition(window).y - startPos.y;

				circle.r = 0.0f;
				circle.g = 1.0f;
				circle.b = 0.0f;

				sendManager.AddPacketData((char*)&circle, sizeof(circle));

				clicked = false;

				break;
			}
		}
	}
}

int main()
{
	sf::UdpSocket receiveSocket;
	receiveSocket.setBlocking(false);

	// bind the socket to a port. If we can't bind, don't continue.
	if (receiveSocket.bind(sf::Socket::AnyPort) != sf::Socket::Done)
	{
		return 0;
	}
	listenPort = receiveSocket.getLocalPort();

	//We found an available port. Create our draw window and initialize packet sending;
	sf::RenderWindow window(sf::VideoMode(512, 512), "Network Draw Client (CPP)");

	SendManager sendManager;

	bool mouseMoved = false;
	bool clicked = false;
	Mode mode = PIXEL;

	sf::Vector2i startPos = sf::Mouse::getPosition(window);

	sf::Clock timer;
	float sendInterval = 0.05f;

	//Send our initial "announce" packet. When we receive a response, we can process our queue.
	PacketClientAnnounce clientAnnounce;
	clientAnnounce.type = Packet::e_clientAnnounce;

	sendManager.SendImmediate((char*)&clientAnnounce, sizeof(clientAnnounce));

	while (window.isOpen())
	{
		//We are connected and can now send shape & cursor data.
		if (connected)
		{
			//Process any current waiting packets. Also send a "cursor move" packet if the mouse moved.
			if (timer.getElapsedTime().asSeconds() >= sendInterval)
			{
				sendManager.ProcessQueue();

				if (mouseMoved)
				{
					sf::Vector2i mousePos = sf::Mouse::getPosition(window);

					CursorInfo cursorInfo;

					cursorInfo.m_posX = mousePos.x;
					cursorInfo.m_posY = mousePos.y;

					PacketClientCursor clientCursorPacket;

					clientCursorPacket.type = Packet::e_clientCursor;
					clientCursorPacket.cursor = cursorInfo;

					sendManager.AddPacketData((char*)&clientCursorPacket, sizeof(clientCursorPacket));
					mouseMoved = false;
				}

				timer.restart();
			}

			sf::Event event;
			while (window.pollEvent(event))
			{
				//Close the Window
				if (event.type == sf::Event::Closed)
				{
					window.close();
				}

				//The Mouse Moved.
				if (event.type == sf::Event::MouseMoved)
				{
					mouseMoved = true;
				}

				//Do Drawing Logic
				DrawLogic(startPos, mode, clicked, window, sendManager);
			}
		}

		//We are not connected. Check to see if we have received a response to our announce packet to finish handshaking.
		else
		{
			char data[100];
			std::size_t received;

			sf::IpAddress sender;

			bool result = receiveSocket.receive(data, 100, received, sender, listenPort);

			if (result == sf::Socket::Done)
			{
				//TODO read data and verify it is a correct packet.
				recipient = sender;
				connected = true;
			}

			else if (result != sf::Socket::Done && result != sf::Socket::NotReady)
			{
				std::cout << "Error receiving data" << std::endl;
			}
		}
	}

	return 0;
}