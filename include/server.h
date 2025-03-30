#pragma once

#include "IServerListener.h"

#include <thread>
#include <atomic>

#include <asio.hpp>

class Server
{
public:
	Server(const IServerListener& listener);

	void Start();
	void Close();

private:
	const IServerListener& listener;

	asio::io_context context;
	asio::ip::udp::socket socket;
	asio::ip::udp::endpoint remote_endpoint;

	std::thread thread;

	size_t bytesReceived;
	unsigned char* buf;

	void StartReceive();
	void HandleReceive(const std::error_code& ec, size_t bytesReceived);
};
