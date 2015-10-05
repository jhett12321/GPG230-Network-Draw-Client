#ifndef SENDMGR_H
#define SENDMGR_H

#include <queue>
#include "SFML\Network.hpp"

class SendManager
{
public:
	SendManager();
	~SendManager();

	sf::UdpSocket socket;

	std::queue<char*> m_queuedPackets;
	std::queue<size_t> m_queuedPacketSize;

	void ProcessQueue();
	void AddPacketData(char* data, size_t length);
	void SendImmediate(char* data, size_t length);
};

#endif
