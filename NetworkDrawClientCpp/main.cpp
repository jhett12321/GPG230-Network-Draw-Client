#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Packet.h"
#include <iostream>

enum Mode
{
	PIXEL,
	BOX,
	LINE,
	CIRCLE
};

void sendData(char* data, size_t length)
{
	sf::UdpSocket socket;
	sf::IpAddress recipient = "127.0.0.1";

	if (socket.send(data, length, recipient, 1300) != sf::Socket::Done)
	{
		std::cout << "Error sending data" << std::endl;
	}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(512, 512), "Network Draw Client (CPP)");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	PacketBox packetBox;
	packetBox.x = 10;
	packetBox.y = 20;

	packetBox.w = 100;
	packetBox.h = 100;

	packetBox.r = 1.0f;
	packetBox.g = 1.0f;
	packetBox.b = 1.0f;

	bool clicked = false;
	Mode mode = PIXEL;

	sf::Vector2i startPos = sf::Mouse::getPosition(window);

	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

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

						sendData((char*)&pixel, sizeof(pixel));
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

						sendData((char*)&box, sizeof(box));

						clicked = false;
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

						

						sendData((char*)&line, sizeof(line));

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

						//circle.w = sf::Mouse::getPosition(window).x - startPos.x;
						//circle.h = sf::Mouse::getPosition(window).y - startPos.y;

						circle.r = 0.0f;
						circle.g = 1.0f;
						circle.b = 0.0f;

						sendData((char*)&circle, sizeof(circle));

						clicked = false;

						break;
					}
				}
			}
		}

		//window.clear();
		//window.draw(shape);
		//window.display();
	}

	return 0;
}