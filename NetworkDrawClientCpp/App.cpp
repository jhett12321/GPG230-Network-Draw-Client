#include <SFML\Network.hpp>
#include <SFML\Graphics.hpp>
#include "App.h"
#include "DrawInput.h"
#include "PacketSender.h"
#include "PacketListener.h"
#include "Heatmap.h"
#include "Packet.h"

App& App::Instance()
{
	static App app;
	return app;
}

bool App::Init()
{
	//Our render window.
	mWindow = new sf::RenderWindow();

	//The socket and our resolved recipient.
	mPort = 1300;
	mSocket = new sf::UdpSocket();
	mSocket->setBlocking(false);

	//App Status
	mConnected = false;

	mHelpTextEnabled = false;

	// bind the socket to a port. If we can't bind, quit.
	if (mSocket->bind(sf::Socket::AnyPort) != sf::Socket::Done)
	{
		return false;
	}

	mWindow = new sf::RenderWindow(sf::VideoMode(512, 512), "Network Draw Client (CPP)", sf::Style::Titlebar | sf::Style::Close);
	mListenPort = mSocket->getLocalPort();

	//Packet Management
	mPacketSender = new PacketSender();
	mPacketListener = new PacketListener();

	//Heatmap
	mHeatmap = new Heatmap();

	//Client Input
	mDrawInput = new DrawInput();

	return true;
}

void App::Run()
{
	//UI Text. Shows Controls, etc.

	// Load Font
	sf::Font font;

	if (!font.loadFromFile("arial.ttf"))
	{
		// error...
	}

	// Create help text
	sf::Text helpText;

	helpText.setPosition(0, mWindow->getSize().y - 30);
	helpText.setString("Controls:    [1] Pixel Brush (Click)    [2] Box Brush (Drag)    [3] Line Brush (Drag)    [4] Circle Brush (Drag) \n             [Insert] Heatmap View    [Pause/Break] Push Heatmap View to Server    [Home] Hide this menu.");
	helpText.setFont(font);
	helpText.setCharacterSize(10);
	helpText.setStyle(sf::Text::Regular);

	mHelpTextEnabled = true;

	//App timers. Used for the initial handshake, connection timeouts, and queueing packets.

	//Main DeltaT clock.
	sf::Clock deltaClock;

	//How often to bundle and send our queued packets.
	float sendInterval = 0.05f;
	float sendIntervalTimer = 0.0f;

	//How often to attempt an initial handshake.
	float initialAnnounceInterval = 2.0f;
	float initialAnnounceIntervalTimer = 0.0f;

	//How often to force through a cursor packet (for keepAlive).
	float keepAliveInterval = 2.0f;
	float keepAliveIntervalTimer = 0.0f;

	//How long to wait without receiving a packet before attempting to re-establish a new connection.
	float timeoutPeriod = 5.0f;
	float timeoutPeriodTimer = 0.0f;

	//Create our initial "announce" packet. When we receive a response, we can process our queue.
	//This is sent in our main loop.
	PacketClientAnnounce* clientAnnounce = new PacketClientAnnounce();
	clientAnnounce->type = Packet::e_clientAnnounce;

	while (mWindow->isOpen())
	{
		float deltaTime = deltaClock.restart().asSeconds();

		//Perform our input stuff.
		sf::Event event;
		while (mWindow->pollEvent(event))
		{
			//Do Draw/Input Logic
			mDrawInput->Update(event);
		}

		//We are connected and can now send shape & cursor data.
		if (mConnected)
		{
			sendIntervalTimer += deltaTime;
			keepAliveIntervalTimer += deltaTime;

			//Queue our keep alive packet.
			if (keepAliveIntervalTimer >= keepAliveInterval)
			{
				mPacketSender->AddPacketData((char*)clientAnnounce, sizeof(*clientAnnounce));

				keepAliveIntervalTimer = 0.0f;
			}

			//Process any current waiting packets. Also send a "cursor move" packet if the mouse moved.
			if (sendIntervalTimer >= sendInterval)
			{
				mDrawInput->CheckMouseMoved();

				mPacketSender->ProcessQueue();

				sendIntervalTimer = 0.0f;
			}
		}

		//Broadcast our announce packet if we are not connected.
		else
		{
			initialAnnounceIntervalTimer += deltaTime;

			if (initialAnnounceIntervalTimer >= initialAnnounceInterval)
			{
				mPacketSender->SendImmediate((char*)clientAnnounce, sizeof(*clientAnnounce));

				initialAnnounceIntervalTimer = 0.0f;
			}
		}

		//Check to see if we received any new packets.
		bool packetsReceived = mPacketListener->Update();

		//Check to see if our connection is still alive.
		if (!packetsReceived)
		{
			if (mConnected)
			{
				timeoutPeriodTimer += deltaTime;

				if (timeoutPeriodTimer > timeoutPeriod)
				{
					Disconnect();

					timeoutPeriodTimer = 0.0f;
				}
			}
		}

		else
		{
			timeoutPeriodTimer = 0.0f;
		}

		//Clear our current window.
		mWindow->clear();

		//Render the heatmap (if enabled)
		mHeatmap->DrawHeatmap(*mWindow);

		//Render our cursors
		for (sf::RectangleShape* cursor : mCursors)
		{
			mWindow->draw(*cursor);
		}

		//Render the help UI
		if (mHelpTextEnabled)
		{
			mWindow->draw(helpText);
		}

		//Display our new window.
		mWindow->display();
	}
}

void App::Disconnect()
{
	mConnected = false;
	mRecipient = nullptr;

	//Delete our cursors.
	for (auto* cursor : mCursors)
	{
		DELETE_NULLIFY(cursor);
	}

	//Clear the cursor vector.
	mCursors.clear();
}

App::~App()
{
	//Client Input
	DELETE_NULLIFY(mDrawInput);

	//Packet Management
	DELETE_NULLIFY(mPacketListener);
	DELETE_NULLIFY(mPacketSender);

	//The socket and our resolved recipient.
	DELETE_NULLIFY(mRecipient);
	mSocket->unbind();
	DELETE_NULLIFY(mSocket);

	//Delete our cursors.
	for (auto* cursor : mCursors)
	{
		DELETE_NULLIFY(cursor);
	}

	//Clear the cursor vector.
	mCursors.clear();

	//Delete our window.
	DELETE_NULLIFY(mWindow);
}