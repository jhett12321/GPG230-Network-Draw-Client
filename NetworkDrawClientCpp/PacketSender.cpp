#include <iostream>
#include <SFML\Network.hpp>
#include "App.h"
#include "PacketSender.h"

void PacketSender::AddPacketData(char* data, size_t length)
{
	std::string packetData = std::string(data, length);

	mQueuedPackets.push(packetData);
}

void PacketSender::SendImmediate(char* data, size_t length)
{
	sf::Socket::Status sendResult;

	if (App::Instance().GetRecipient() == nullptr)
	{
		sendResult = App::Instance().GetSocket()->send(data, length, sf::IpAddress::Broadcast, App::Instance().GetPort());
	}

	else
	{
		sendResult = App::Instance().GetSocket()->send(data, length, *App::Instance().GetRecipient(), App::Instance().GetPort());
	}

	if (sendResult != sf::Socket::Done)
	{
		std::cout << "Error sending data" << std::endl;
	}
}

void PacketSender::ProcessQueue()
{
	if (App::Instance().GetConnected())
	{
		std::string bundledData = "";

		while (!mQueuedPackets.empty())
		{
			std::string data = mQueuedPackets.front();

			if (bundledData.length() + data.length() < sf::UdpSocket::MaxDatagramSize)
			{
				bundledData.append(data);

				mQueuedPackets.pop();
			}

			else
			{
				if (App::Instance().GetSocket()->send(bundledData.c_str(), bundledData.length(), *App::Instance().GetRecipient(), App::Instance().GetPort()) == sf::Socket::Done)
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
			if (App::Instance().GetSocket()->send(bundledData.c_str(), bundledData.length(), *App::Instance().GetRecipient(), App::Instance().GetPort()) != sf::Socket::Done)
			{
				std::cout << "Error sending data" << std::endl;
			}
		}
	}
}

