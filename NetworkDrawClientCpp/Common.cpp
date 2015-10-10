#include "Common.h"
#include "DrawInput.h"
#include "PacketSender.h"

//Our render window.
sf::RenderWindow* Common::window;

//The Renderable Cursors sent by the server.
std::vector<sf::RectangleShape*> Common::cursors;

//The socket and our resolved recipient.
sf::UdpSocket* Common::socket;
sf::IpAddress* Common::recipient;
bool Common::connected;

//Port Numbers
unsigned short Common::listenPort; //Assigned automatically.

//Packet Management
PacketSender* Common::packetSender;
PacketListener* Common::packetListener;

//Client Input
DrawInput* Common::drawInput;

bool Common::Init()
{
	connected = false;
	socket = new sf::UdpSocket();

	socket->setBlocking(false);

	// bind the socket to a port. If we can't bind, quit.
	if (socket->bind(sf::Socket::AnyPort) != sf::Socket::Done)
	{
		return false;
	}

	window = new sf::RenderWindow(sf::VideoMode(512, 512), "Network Draw Client (CPP)");
	listenPort = socket->getLocalPort();

	//Packet Management
	packetSender = new PacketSender();
	packetListener = new PacketListener();

	//Client Input
	drawInput = new DrawInput();

	return true;
}