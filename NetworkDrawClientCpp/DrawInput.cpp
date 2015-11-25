#include <SFML/Graphics.hpp>
#include "App.h"
#include "DrawInput.h"
#include "Packet.h"
#include "PacketSender.h"

DrawInput::DrawInput()
{
	mMousePos = new sf::Vector2i(0, 0);
	mStartPos = new sf::Vector2i(0, 0);
}

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
		sf::Vector2i currentMousePos = sf::Mouse::getPosition(*App::Instance().GetWindow());

		mMousePos->x = currentMousePos.x;
		mMousePos->y = currentMousePos.y;
		mMouseMoved = true;
	}

	if (!mClicked)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1))
		{
			mMode = PIXEL;
		}

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2))
		{
			mMode = BOX;
		}

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3))
		{
			mMode = LINE;
		}

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4))
		{
			mMode = CIRCLE;
		}
	}

	if (!mClicked && sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		sf::Vector2i currentMousePos = sf::Mouse::getPosition(*App::Instance().GetWindow());

		mStartPos->x = currentMousePos.x;
		mStartPos->y = currentMousePos.y;

		mClicked = true;
	}

	switch (mMode)
	{
		case PIXEL:
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				PacketPixel pixel;
				pixel.type = Packet::e_pixel;

				pixel.x = mMousePos->x;
				pixel.y = mMousePos->y;

				pixel.r = 1.0f;
				pixel.g = 0.0f;
				pixel.b = 0.5f;

				App::Instance().GetPacketSender()->AddPacketData((char*)&pixel, sizeof(pixel));
			}

			break;
		}
		case BOX:
		{
			if (mClicked && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				PacketBox box;

				box.type = Packet::e_box;

				if (mStartPos->x < mMousePos->x)
				{
					box.x = mStartPos->x;
					box.w = mMousePos->x - mStartPos->x;
				}

				else
				{
					box.x = mMousePos->x;
					box.w = mStartPos->x - mMousePos->x;
				}

				if (mStartPos->y < mMousePos->y)
				{
					box.y = mStartPos->y;
					box.h = mMousePos->y - mStartPos->y;
				}

				else
				{
					box.y = mMousePos->y;
					box.h = mStartPos->y - mMousePos->y;
				}

				box.r = 1.0f;
				box.g = 0.0f;
				box.b = 1.0f;

				App::Instance().GetPacketSender()->AddPacketData((char*)&box, sizeof(box));
			}

			break;
		}
		case LINE:
		{
			if (mClicked && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				PacketLine line;

				line.type = Packet::e_line;

				line.x1 = mStartPos->x;
				line.y1 = mStartPos->y;

				line.x2 = sf::Mouse::getPosition(*App::Instance().GetWindow()).x;
				line.y2 = sf::Mouse::getPosition(*App::Instance().GetWindow()).y;

				line.r = 1.0f;
				line.g = 0.0f;
				line.b = 0.0f;

				App::Instance().GetPacketSender()->AddPacketData((char*)&line, sizeof(line));
			}

			break;
		}
		case CIRCLE:
		{
			if (mClicked && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				PacketCircle circle;

				circle.type = Packet::e_circle;

				circle.x = mStartPos->x;
				circle.y = mStartPos->y;

				int currentRadius = 0;

				if (mStartPos->x < mMousePos->x)
				{
					currentRadius = mMousePos->x - mStartPos->x;
				}

				else
				{
					currentRadius = mStartPos->x - mMousePos->x;
				}

				if (mStartPos->y < mMousePos->y)
				{
					if (currentRadius < mMousePos->y - mStartPos->y)
					{
						currentRadius = mMousePos->y - mStartPos->y;
					}
				}

				else
				{
					if (currentRadius < mStartPos->y - mMousePos->y)
					{
						currentRadius = mStartPos->y - mMousePos->y;
					}
				}

				circle.radius = currentRadius;

				circle.r = 1.0f;
				circle.g = 0.0f;
				circle.b = 1.0f;

				App::Instance().GetPacketSender()->AddPacketData((char*)&circle, sizeof(circle));
			}

			break;
		}
	}

	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		if (mClicked)
			mClicked = false;
	}
}

void DrawInput::CheckMouseMoved()
{
	if (mMouseMoved)
	{
		CursorInfo cursorInfo;

		cursorInfo.m_posX = mMousePos->x;
		cursorInfo.m_posY = mMousePos->y;

		sf::Vector2u windowSize = App::Instance().GetWindow()->getSize();

		if (mMousePos->x > 0 && mMousePos->y > 0 && mMousePos->x < windowSize.x && mMousePos->y < windowSize.y)
		{
			PacketClientCursor clientCursorPacket;

			clientCursorPacket.type = Packet::e_clientCursor;
			clientCursorPacket.cursor = cursorInfo;

			App::Instance().GetPacketSender()->AddPacketData((char*)&clientCursorPacket, sizeof(clientCursorPacket));
		}

		mMouseMoved = false;
	}
}

