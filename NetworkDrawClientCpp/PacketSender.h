#ifndef PACKETSENDER_H
#define PACKETSENDER_H

#include <queue>

class PacketSender
{
public:
	std::queue<std::string> mQueuedPackets;

	void ProcessQueue();
	void AddPacketData(char* data, size_t length);
	void SendImmediate(char* data, size_t length);
};

#endif
