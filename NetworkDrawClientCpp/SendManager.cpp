#include "SendManager.h"
#include "main.h"
#include <iostream>

void SendManager::AddPacketData(char* data, size_t length)
{
	m_queuedPackets.push(data);
	m_queuedPacketSize.push(length);
}

void SendManager::SendImmediate(char* data, size_t length)
{
	if (socket.send(data, length, recipient, port) != sf::Socket::Done)
	{
		std::cout << "Error sending data" << std::endl;
	}
}

void SendManager::ProcessQueue()
{
	if (connected)
	{
		while (!m_queuedPackets.empty())
		{
			char* data = m_queuedPackets.front();
			size_t length = m_queuedPacketSize.front();

			m_queuedPackets.pop();
			m_queuedPacketSize.pop();

			//TODO Bundle packets together.
			if (socket.send(data, length, recipient, port) != sf::Socket::Done)
			{
				std::cout << "Error sending data" << std::endl;
			}
		}

		//TODO Bundle packets together.
		//if (socket.send(data, length, recipient, port) != sf::Socket::Done)
		//{
		//	std::cout << "Error sending data" << std::endl;
		//}
	}
}

