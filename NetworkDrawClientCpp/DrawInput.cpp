#include "DrawInput.h"
#include "Packet.h"
#include "SFML/Graphics.hpp"
#include "Common.h"

void DrawInput::Update(sf::Event event)
{
	//Close the Window
	if (event.type == sf::Event::Closed)
	{
		exit(0);
	}

	//The Mouse Moved.
	if (event.type == sf::Event::MouseMoved)
	{
		mousePos = sf::Mouse::getPosition(*Common::window);
		mouseMoved = true;
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

		startPos = sf::Mouse::getPosition(*Common::window);
	}

	switch (mode)
	{
		case PIXEL:
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				PacketPixel pixel;
				pixel.type = Packet::e_pixel;

				pixel.x = mousePos.x;
				pixel.y = mousePos.y;

				pixel.r = 1.0f;
				pixel.g = 0.0f;
				pixel.b = 0.5f;

				Common::packetSender->AddPacketData((char*)&pixel, sizeof(pixel));
			}

			break;
		}
		case BOX:
		{
			if (clicked && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				PacketBox box;

				box.type = Packet::e_box;

				if (startPos.x < mousePos.x)
				{
					box.x = startPos.x;
					box.w = mousePos.x - startPos.x;
				}

				else
				{
					box.x = mousePos.x;
					box.w = startPos.x - mousePos.x;
				}

				if (startPos.y < mousePos.y)
				{
					box.y = startPos.y;
					box.h = mousePos.y - startPos.y;
				}

				else
				{
					box.y = mousePos.y;
					box.h = startPos.y - mousePos.y;
				}

				box.r = 1.0f;
				box.g = 0.0f;
				box.b = 1.0f;

				Common::packetSender->AddPacketData((char*)&box, sizeof(box));
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

				line.x2 = sf::Mouse::getPosition(*Common::window).x;
				line.y2 = sf::Mouse::getPosition(*Common::window).y;

				line.r = 1.0f;
				line.g = 0.0f;
				line.b = 0.0f;

				Common::packetSender->AddPacketData((char*)&line, sizeof(line));
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

				float currentRadius = 0.0f;

				if (startPos.x < mousePos.x)
				{
					currentRadius = mousePos.x - startPos.x;
				}

				else
				{
					currentRadius = startPos.x - mousePos.x;
				}

				if (startPos.y < mousePos.y)
				{
					if (currentRadius < mousePos.y - startPos.y)
					{
						currentRadius = mousePos.y - startPos.y;
					}
				}

				else
				{
					if (currentRadius < startPos.y - mousePos.y)
					{
						currentRadius = startPos.y - mousePos.y;
					}
				}

				circle.radius = currentRadius;

				circle.r = 1.0f;
				circle.g = 0.0f;
				circle.b = 1.0f;

				Common::packetSender->AddPacketData((char*)&circle, sizeof(circle));
			}

			break;
		}
	}

	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		if (clicked)
			clicked = false;
	}
}

void DrawInput::CheckMouseMoved()
{
	if (mouseMoved)
	{
		CursorInfo cursorInfo;

		cursorInfo.m_posX = mousePos.x;
		cursorInfo.m_posY = mousePos.y;

		PacketClientCursor clientCursorPacket;

		clientCursorPacket.type = Packet::e_clientCursor;
		clientCursorPacket.cursor = cursorInfo;

		//Common::packetSender->AddPacketData((char*)&clientCursorPacket, sizeof(clientCursorPacket));
		mouseMoved = false;
	}
}

