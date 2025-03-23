#pragma once

#include "IServerListener.h"

#include <thread>
#include <atomic>

class Server
{
public:
	Server(const IServerListener& listener);

	void Close();

private:
	std::thread thread;
	std::atomic_bool tShouldClose;
};
