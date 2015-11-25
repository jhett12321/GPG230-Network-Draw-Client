#ifndef APP_H
#define APP_H

#include <vector>
#include "Macros.h"

namespace sf
{
	class RenderWindow;
	class UdpSocket;
	class IpAddress;
	class RectangleShape;
}

class PacketSender;
class PacketListener;
class DrawInput;

class App
{
public:
	static App& Instance();
	~App();

	//Initializer. Returns false if we are unable to bind to a port.
	bool Init();

	void Run();

	void Disconnect();

	//The Renderable Cursors sent by the server.
	std::vector<sf::RectangleShape*> mCursors;

protected:
	//Our render window.
	RO_PTR_PROPERTY(sf::RenderWindow, Window);

	//The socket and our resolved recipient.
	RO_PTR_PROPERTY(sf::UdpSocket, Socket);
	PUBLIC_PTR_PROPERTY(sf::IpAddress, Recipient);

	//App Status
	PUBLIC_DATA_PROPERTY(bool, Connected);

	//Port Numbers
	PUBLIC_DATA_PROPERTY(unsigned short, Port); //Target port.
	PUBLIC_DATA_PROPERTY(unsigned short, ListenPort); //Assigned automatically.

	//Packet Management
	RO_PTR_PROPERTY(PacketSender, PacketSender);
	RO_PTR_PROPERTY(PacketListener, PacketListener);

	//Client Input
	RO_PTR_PROPERTY(DrawInput, DrawInput);
};

#endif
