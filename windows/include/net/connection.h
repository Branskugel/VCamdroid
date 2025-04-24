#pragma once

#include <memory>
#include <functional>
#include <asio.hpp>

#include "receiver.h"

/// <summary>
/// Manages a TCP connection
/// </summary>
class Connection : public std::enable_shared_from_this<Connection>
{
	friend class Server;

public:
	using tcp = asio::ip::tcp;
	using udp = asio::ip::udp;
	using OnDisconnectedListener = std::function<void(std::shared_ptr<Connection>)>;

	Connection(const Receiver& receiver, tcp::socket socket, std::string& name, OnDisconnectedListener onDisconnectedListener);
private:

	OnDisconnectedListener onDisconnectedListener;

	tcp::socket socket;
	const Receiver& receiver;

	bool active;
	std::string name;

	/// <summary>
	/// Connected devices only send their name once after connecting.
	/// That is handled directly by the server when creating a new connection.
	/// This function is used only to detect when a device disconnects.
	/// </summary>
	void Read();

	void Send(std::string message);
	void Send(unsigned char* bytes, size_t size);
	void Close(bool stoppedByServer = false);
};
