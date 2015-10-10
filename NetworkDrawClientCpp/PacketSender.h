#ifndef PACKETSENDER_H
#define PACKETSENDER_H

#include <queue>
#include "SFML\Network.hpp"

class PacketSender
{
public:
	std::queue<std::string> m_queuedPackets;

	void ProcessQueue();
	void AddPacketData(char* data, size_t length);
	void SendImmediate(char* data, size_t length);

	sf::Clock timer;
	float sendInterval = 0.05f;
};

#endif
