#include "PacketSender.h"
#include "Common.h"
#include <iostream>

void PacketSender::AddPacketData(char* data, size_t length)
{
	std::string packetData;// = std::string(data, length);

	packetData.assign(data, length);

	m_queuedPackets.push(packetData);
}

void PacketSender::SendImmediate(char* data, size_t length)
{
	sf::IpAddress target;

	if (Common::recipient == nullptr)
	{
		target = sf::IpAddress::Broadcast;
	}

	else
	{
		target = *Common::recipient;
	}

	if (Common::socket->send(data, length, target, Common::port) != sf::Socket::Done)
	{
		std::cout << "Error sending data" << std::endl;
	}
}

void PacketSender::ProcessQueue()
{
	if (Common::connected)
	{
		std::string bundledData = "";

		while (!m_queuedPackets.empty())
		{
			std::string data = m_queuedPackets.front();

			//TODO Bundle packets together.
			if (bundledData.length() + data.length() < sf::UdpSocket::MaxDatagramSize)
			{
				bundledData.append(data);

				m_queuedPackets.pop();
			}

			else
			{
				if (Common::socket->send(bundledData.c_str(), bundledData.length(), *Common::recipient, Common::port) == sf::Socket::Done)
				{
					bundledData = "";
				}

				else
				{
					std::cout << "Error sending data" << std::endl;
				}
			}
		}

		if (!bundledData.empty())
		{
			if (Common::socket->send(bundledData.c_str(), bundledData.length(), *Common::recipient, Common::port) != sf::Socket::Done)
			{
				std::cout << "Error sending data" << std::endl;
			}
		}
	}
}

