#pragma once

#include <vector>

class IServerListener
{
public:
	virtual void OnClientConnected() = 0;
	virtual void OnClientDisconnected() = 0;
	virtual void OnBytesReceived(std::vector<unsigned char> bytes) const = 0;
};