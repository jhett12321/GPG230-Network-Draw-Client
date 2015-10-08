#ifndef MAIN_H
#define MAIN_H

#include "SFML\Network.hpp"

extern bool connected;
extern unsigned short port;
extern unsigned short listenPort;
extern sf::IpAddress recipient;
extern sf::UdpSocket socket;

#endif