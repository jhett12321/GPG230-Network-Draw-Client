#ifndef COMMON_H
#define COMMON_H

#include "SFML\Graphics.hpp"
#include "SFML\Network.hpp"
#include "PacketSender.h"
#include "PacketListener.h"
#include "DrawInput.h"

class Common
{
public:
	//Our render window.
	static sf::RenderWindow* window;

	//The Renderable Cursors sent by the server.
	static std::vector<sf::RectangleShape*> cursors;

	//Initializer. Returns false if we are unable to bind to a port.
	static bool Init();

	//The socket and our resolved recipient.
	static sf::UdpSocket* socket;
	static sf::IpAddress* recipient;
	static bool connected;

	//Port Numbers
	static const unsigned short port = 1300; //Target port.
	static unsigned short listenPort; //Assigned automatically.

	//Packet Management
	static PacketSender* packetSender;
	static PacketListener* packetListener;

	//Client Input
	static DrawInput* drawInput;

};

#endif
