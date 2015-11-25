#ifndef PACKETLISTENER_H
#define PACKETLISTENER_H

class PacketListener
{
public:
	//Returns true if we received any packets this tick.
	bool Update();
};

#endif