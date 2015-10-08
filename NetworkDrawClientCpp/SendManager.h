#ifndef SENDMGR_H
#define SENDMGR_H

#include <queue>
#include "SFML\Network.hpp"

class SendManager
{
public:
	std::queue<std::string> m_queuedPackets;

	void ProcessQueue();
	void AddPacketData(char* data, size_t length);
	void SendImmediate(char* data, size_t length);
};

#endif
